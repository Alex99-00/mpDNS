/*

  Programm/Lib : mpDNSlib
  Datei        : dns.c
  Version      : v1.1r
  
  Beschreibung:
  DNS Basis Modul.
  
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if (defined(_WIN32))
#include <windows.h>
#else
#include <netinet/in.h> /*htons...*/
#include <arpa/inet.h>  /*inet_ntop...*/
#endif

#include <assert.h>

#include <db.h>
#include <error.h>
#include <alloc.h>
#include <dns.h>
#include <debug.h>
#include <typen.h>
#include <udp.h>


/* Enthält alle zur Verfügung stehenden Zonennamen. */
char ** zones;
/* Anzahl zur Verfügung stehender Zonen */
int numZones;

/* Forwarder IP */
char * forwarder;


/*
  mpDNSlib initialisieren.
  host        = MySQL Hostname. ! IP nehmen, "ich" bin der DNS !
  user        = Username auf MySQL Server.
  passwd      = Passwort auf MySQL Server.
  db          = Datenbank Name.
  port        = Port des MySQL Server.
  eDebugLevel = Log/Debug Level.
  logCallback = Funktionszeiger für Log Funktion.
  fwd         = Forwarder IP.
  return = (-1): Nicht möglich.
             0 : Ini ok.
*/
int iniDNSlib(char * host, char * user, char * passwd, char * db, DNS_USI port,
              int eDebugLevel, LogFunktion logCallback, char * fwd
) {

    setLogging( eDebugLevel, logCallback );
    
    if ( iniDB( host, user, passwd, db, port ) < 0 ) {
         printLog( "iniDNSlib.iniDB() error",LOG_L1 );
         return (-1);
    }
    
    /* Alle zur Verfügung stehende Zonennamen abfragen. */
    if ( (numZones = queryZoneNames( &zones )) <= 0 ) {
         printLog( "iniDNSlib.queryZoneNames() error",LOG_L1 );
         return (-1);
    }
    
    forwarder = MALLOC( strlen(fwd)*sizeof(char) );
    memmove( forwarder, fwd, strlen(fwd)+1 );

    return 0;
}

/*
  mpDNSlib aufräumen.
  return = void.
*/
void deIniDNSlib(void) {
    int i=0;
    assert( (zones != NULL) && (numZones >= 0) );
    
    deIniDB();

    /* Zonennamen freigeben. */
    while ( i < numZones ) {
            FREE( zones[i++] );
    }
    FREE(zones);
    
    setLogging( 0, NULL );
    
    /* Forwarder Speicher freigeben. */
    if ( forwarder != NULL )
         FREE(forwarder);
}

/*
  Testet ob ein Zonenname in der "zones Tabelle" vorhanden ist.
  zone   = Zu testender Zonenanme.
  return =  (-1): Keine Zone gefunden.
           (>=0): Position in "zones Tabelle".
*/
int testZoneName( char * zone ) {
    int i = 0;
    assert( (zones != NULL) && (zone != NULL) );
    
    while ( i < numZones ) {
    
            if ( strcmp( zone, zones[i] ) == 0 ) {
                 return i;
            }
            i++;
    }
    return (-1);
}

/*
  Suchen eines Zonennamen.
  Erst direkten Treffer suchen, dann kleinstmöglichen passenden Zonennamen.
  zone   = Zu findender Zonenname.
  return =  (-1): Keine Zone gefunden.
           (>=0): Position in "zones Tabelle".
*/
int findZoneName( char * zone ) {
    int zoneNamePos;
    
    assert( zone != NULL );
    
    if ( (zoneNamePos=testZoneName( zone )) >= 0 ) {
         return zoneNamePos;
    }
    
    {
     char buf[256];
     int len = strlen(zone)+1;
     int src=len, dst=0;
     
     while ( src >= 0 ) {
     
        while ( src >= 0 ) {
                if ( zone[src] == '.' )
                     break;
                src--;
        }
     
        dst = len-src;
        memmove( buf, &zone[src+1], dst );
     
        if ( (zoneNamePos=testZoneName( buf )) >= 0 ) {
             return zoneNamePos;
        }
     
        src--;
     }
     
    }
        
    return (-1);
}

/*
  Forwarder fragen.
  msg    = Frage.
  msgLen = Länge der Frage.
  return = (-1): Fehler bei frage an Forwarder.
             0 : Antwort erhalten.
*/
int askForwarder( char ** msg, size_t * msgLen ) {
    char buf[513];
    int sock, rb;
    struct sockaddr_in addr;

    memset( &addr, 0, sizeof(addr) );
    /* Forwarder Socket erstellen */
    genSockaddressStruct( 53, forwarder, &addr);
    sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    
    /* Frage abschicken. */
    if ( (writeUdp( sock, *msg, *msgLen, &addr )) != ((int)(*msgLen)) ) {
         printLog( "askForwarder.writeUdp error", LOG_L1 );
         return (-1);
    }
    
    /* Auf Antwort warten. */
    if ( (rb = readUdp( sock, buf, 513, &addr, 1000*1000*5/*3sek.*/ )) > 0 ) {
    
         /* Antwort in msg eintragen. */
         *msg = REALLOC( *msg, rb * sizeof(char) );
         if ( *msg == NULL ) {
              return (-1);
         }
         
         memmove( *msg, buf, (size_t)rb );
         *msgLen = rb;
         
    } else {
         return (-1);
    }
    
    closeUdpSocket( sock );
    
    return 0;
}

/*
  RR an Antwort anhängen.
  rr        = Anzuhängende RRs.
  numEntrys = Anzahl RRs die anzuhängen sind.
  msg       = Antwort an die die RRs anzuhängen sind.
  msgLen    = Aktuelle Länge von Antwort.
  free      = Verarbeitete RRs freigeben(1=ja|0=nein).
  return    = Eine "RCODE_" Nummer.
*/
int processRRs( DNSrr *** rr, int numEntrys, char ** msg, size_t * msgLen, int free ) {
    int i;
    int len=(*msgLen);
    char * ptr;

    /* Gesamtgröße berechnen. */
    i=0;
    while ( i < numEntrys ) {
            len += (int)((*rr)[i++]->rrlen);
    }
    
    /* Speicher besorgen. */
    ptr = MALLOC( len * sizeof(char) );    
    if ( ptr == NULL ) {
         i=0;
         while ( i < numEntrys ) {
                 FREE( (*rr)[i]->rdata );
                 FREE( (*rr)[i] );
                 i++;
         }
         FREE( *rr );
         return RCODE_SERVER_FAILURE;
    }
    memmove( ptr, *msg, (size_t)*msgLen );
    FREE(*msg);
    *msg = ptr;

    /* RRs anhängen. */
    i=0;
    while ( i < numEntrys ) {
            
            appendRR( (*rr)[i], *msg, *msgLen );

            (*msgLen) += (size_t)(*rr)[i]->rrlen;
      
            if ( free == 1 ) {
                 FREE( (*rr)[i]->rdata );
                 FREE( (*rr)[i] );
            }
            
            i++;
    }
    
    if ( free == 1 ) {
         FREE( *rr );
    }
    
    return RCODE_NO_ERROR;
}

/*
  PTR Anfrage verarbeiten.
  msg    = Antwort ablage.
  msgLen = Aktuelle länge von msg.
  q      = Frage.
  h      = Header der Frage/Antwort.
  return = (-1): Fehler.
             0 : Ok
             1 : Forwarder fragen.
*/
int processDNSqueryPTR( char ** msg, size_t * msgLen, DNSQuestion * q, DNSHeader * h ) {
    char buf[256];
    int numEntrys;
    DNSrr ** rr;
    
    { char *pos;
      pos = strstr( q->qname, ".in-addr.arpa" );
      memmove( buf, q->qname, pos-q->qname );
      buf[pos-q->qname] = '\0';
    }
    
    if ( (numEntrys = queryPTR( buf, &rr )) < 0 ) {
          return (-1);
    }
    
    if ( numEntrys == 0 ) 
         return 1;
    
    if ( processRRs( &rr, numEntrys, msg, msgLen, 1 ) != RCODE_NO_ERROR )
         return (-1);
    
    h->ancount += numEntrys;
    h->aa = 1;
    h->rcode = RCODE_NO_ERROR;    
    
    return 0;
}

/*
  Entry(MX,A,NS,CNAME) Anfrage verarbeiten.
  msg    = Antwort ablage.
  msgLen = Aktuelle länge von msg.
  q      = Frage.
  h      = Header der Frage/Antwort.
  return = (-1): Fehler.
             0 : Ok
             1 : Forwarder fragen.
*/
int processDNSqueryEntry( char ** msg, size_t * msgLen, DNSQuestion * q, DNSHeader * h ) {
    char buf[256];
    int numEntrys, zoneNamePos;
    DNSrr ** rr;
    
    if ( (zoneNamePos = findZoneName( q->qname )) < 0 ) {
         return 1;
    }
    
    /* Tabellenname aufbauen. */
    strConvert( zones[zoneNamePos], buf, '.', '_' );

    /* RRs abfragen. */
    if ( (numEntrys = queryEntry( buf, q->qname, q->qtype, &rr )) < 0 ) {
          return (-1);
    }
    
    /* Reine A Anfrage/Ergebnis. */
    if ( ((numEntrys > 0) && (q->qtype == RRT_A)) ) {
         if ( processRRs( &rr, numEntrys, msg, msgLen, 1 ) != RCODE_NO_ERROR ) {
              return (-1);
         }
         h->ancount += numEntrys;
         h->aa = 1;
         h->rcode = RCODE_NO_ERROR;
         return 0;
    }
    
    
    /* "Echte" CNAME Abfrage */
    if ( ( (numEntrys > 0) && (q->qtype == RRT_CNAME) ) ) {
         DNSrr ** rrA;
         int numEntrysA;
         char buf2[257];
         
         convertDomainName( rr[0]->rdata, buf2, 0 );
         
         if ( processRRs( &rr, numEntrys, msg, msgLen, 1 ) != RCODE_NO_ERROR ) {
              return (-1);
         }
         h->ancount += numEntrys;
         
         /* A Einträge für CNAME abfragen */
         if ( (numEntrysA = queryEntry( buf, buf2, RRT_A, &rrA )) < 0 ) {
               return (-1);
         }
                 
         if ( processRRs( &rrA, numEntrysA, msg, msgLen, 1 ) != RCODE_NO_ERROR ) {
              return (-1);
         }
         h->arcount += numEntrysA;
         
         h->aa = 1;
         h->rcode = RCODE_NO_ERROR;
         
         return 0;
    }
    
    
    /* Ist A ein CNAME ? */
    if ( ( (numEntrys == 0) && (q->qtype == RRT_A) ) ) {
         DNSrr ** rrA;
         int numEntrysA;
         char buf2[257];
         
         FREE( rr );
         
         if ( (numEntrys = queryEntry( buf, q->qname, RRT_CNAME, &rr )) < 0 ) {
               return (-1);
         }
         
         
         if ( numEntrys != 1 ) { /* nur einen CNAME erlauben, ist eindeutig! */
              h->aa = 0;
              h->rcode = RCODE_NAME_ERROR;
              return 0;
         }
         
         convertDomainName( rr[0]->rdata, buf2, 0 );
         
         if ( processRRs( &rr, numEntrys, msg, msgLen, 1 ) != RCODE_NO_ERROR ) {
              return (-1);
         }
         h->ancount += numEntrys;
         
         /* A Einträge für CNAME abfragen */
         if ( (numEntrysA = queryEntry( buf, buf2, RRT_A, &rrA )) < 0 ) {
               return (-1);
         }
                 
         if ( processRRs( &rrA, numEntrysA, msg, msgLen, 1 ) != RCODE_NO_ERROR ) {
              return (-1);
         }
         h->ancount += numEntrysA;
         
         h->aa = 1;
         h->rcode = RCODE_NO_ERROR;
         
         return 0;
    }
    
    
    if ( (numEntrys > 0) && (q->qtype == RRT_MX) ) { /* Additional MX processing */
         DNSrr ** rrA;
         int numEntrysA, i;
         char buf2[257];
         
         if ( processRRs( &rr, numEntrys, msg, msgLen, 0 ) != RCODE_NO_ERROR ) {
              return (-1);
         }
         h->ancount += numEntrys;
    
    
         i=0;
         while ( i < numEntrys ) {
         
                 convertDomainName( &rr[i]->rdata[2], buf2, 0 );
                 if ( (zoneNamePos = findZoneName( buf2 )) < 0 ) {
                      return 1;
                 }
    
                 strConvert( zones[zoneNamePos], buf, '.', '_' );
         
         
                 if ( (numEntrysA = queryEntry( buf, buf2, RRT_A, &rrA )) < 0 ) {
                       return (-1);
                 }
                 
                 if ( processRRs( &rrA, numEntrysA, msg, msgLen, 1 ) != RCODE_NO_ERROR ) {
                      return (-1);
                 }
                 h->arcount += numEntrysA;

                 FREE( rr[i]->rdata );
                 FREE( rr[i] );
                 i++;
         }
         FREE( rr );
         
         h->aa = 1;
         h->rcode = RCODE_NO_ERROR;
         
         return 0;
    }
    
    /* NS Anfrage/Ergebnis */
    if ( (numEntrys > 0) && (q->qtype == RRT_NS) ) { 
         DNSrr ** rrA;
         int numEntrysA;
         char buf2[257];
         
         convertDomainName( rr[0]->rdata, buf2, 0 );
         
         if ( processRRs( &rr, numEntrys, msg, msgLen, 1 ) != RCODE_NO_ERROR ) {
              return (-1);
         }
         h->ancount += numEntrys;
         
         /* A Einträge für NS abfragen */
         if ( (numEntrysA = queryEntry( buf, buf2, RRT_A, &rrA )) < 0 ) {
               return (-1);
         }
                 
         if ( processRRs( &rrA, numEntrysA, msg, msgLen, 1 ) != RCODE_NO_ERROR ) {
              return (-1);
         }
         h->arcount += numEntrysA;
         
         h->aa = 1;
         h->rcode = RCODE_NO_ERROR;
    }
    
    return 0;
}


/*
  SOA Anfrage verarbeiten.
  msg    = Antwort ablage.
  msgLen = Aktuelle länge von msg.
  q      = Frage.
  h      = Header der Frage/Antwort.
  return = (-1): Fehler.
             0 : Ok
             1 : Forwarder fragen.
*/
int processDNSquerySOA( char ** msg, size_t * msgLen, DNSQuestion * q, DNSHeader * h ) {
    int numEntrys, zoneNamePos;
    DNSrr ** rr;
    
    /* testZoneName -> muß genau passen. */
    if ( (zoneNamePos = testZoneName( q->qname )) < 0 ) {
          return 1;
    }

    /* SOA Abfragen. */
    if ( (numEntrys = querySOA( q->qname, &rr )) < 0 ) {
          return (-1);
    }

    /* RRs verarbeiten. */
    if ( processRRs( &rr, numEntrys, msg, msgLen, 1 ) != RCODE_NO_ERROR )
         return (-1);

    h->ancount += numEntrys;
    h->aa = 1;
    h->rcode = RCODE_NO_ERROR;
    
    return 0;
}


/*
  Eine Anfrage verarbeiten.
  msg    = Anfrage.
  msgLen = Länge der Anfrage.
  return = (-1): Fehler. !Antwort versenden!
             0 : Ok.
*/
int processDNSquery( char ** msg, size_t * msgLen ) {
    DNSHeader h;
    DNSQuestion q;
    DNS_USI qtype;
    int ret, orgMsgLen;
    char buf[513];
        
    if ( (*msg == NULL) || ((*msgLen) > 512) ) {         
         return (-1);
    }
    
    /* Header und Frage decodieren. */
    if ( (extractHeader( *msg, *msgLen, &h ) < 0) || 
         (extractQuestion( *msg, *msgLen, &q ) < 0) 
       ) {
       
         /* ...assume ID is correct...nothing else what i can do... */
       
         h.qr = 1; h.opcode = OPCODE_QUERY; h.aa = 0;  h.tc = 0;
         h.rd = 0; h.ra = 0; h.z = 0;
       
         h.rcode = RCODE_FORMAT_ERROR;
         
         h.qdcount = 0; h.ancount = 0;
         h.nscount = 0; h.arcount = 0;
         codeHeader( *msg, &h );
         
         return 0;
    }
    
    /* Orginal sichern */
    orgMsgLen = *msgLen;
    memmove( buf, *msg, *msgLen );
    
    h.qr = 1; /* send always an answer ...or should i query the resolver? ;-) */

    if ( h.opcode != OPCODE_QUERY ) { /* Es wird nur QUERY unterstützt */
         h.rcode = RCODE_NOT_IMPLEMENTET;
         codeHeader( *msg, &h );
         return 0;
    }
    

    /* A,NS,CNAME,MX sind "Entrys". */
    qtype = q.qtype;
    if ( (q.qtype == RRT_A) || (q.qtype == RRT_NS) || (q.qtype == RRT_CNAME) || (q.qtype == RRT_MX) )
          qtype = RRT_A;
    
    /* Je nach Typ Abfragen. */
    switch ( qtype ) {
    
       case RRT_A:/* && _NS && _CNAME && _MX */
               if ( (ret = processDNSqueryEntry( msg, msgLen, &q, &h )) == 0 ) {
                    codeHeader( *msg, &h );
               }
       break;
       
       case RRT_PTR:
               if ( (ret = processDNSqueryPTR( msg, msgLen, &q, &h )) == 0 ) {
                    codeHeader( *msg, &h );
               }
       break;
       
       case RRT_SOA:
               if ( (ret = processDNSquerySOA( msg, msgLen, &q, &h )) == 0 ) {
                    codeHeader( *msg, &h );
               }
       break;
       
       case RRT_ALL:{ /* Kein forwarder für "ALL", weil "ALL" recht nutzlos ist */
               DNSQuestion tq = q;
               
               tq.qtype = RRT_SOA;
               if ( (ret = processDNSquerySOA( msg, msgLen, &tq, &h )) == (-1) )
                    break;
               
               tq.qtype = RRT_A;
               if ( (ret = processDNSqueryEntry( msg, msgLen, &tq, &h )) == (-1) )
                    break;
               
               tq.qtype = RRT_NS;
               if ( (ret = processDNSqueryEntry( msg, msgLen, &tq, &h )) == (-1) )
                    break;
                
               tq.qtype = RRT_MX;
               if ( (ret = processDNSqueryEntry( msg, msgLen, &tq, &h )) == (-1) )
                    break;
               
               codeHeader( *msg, &h );
               
       }break;
       
       default:
               h.rcode = RCODE_NOT_IMPLEMENTET; /* Not Implementet */
               codeHeader( *msg, &h );
       break;
    }/*switch*/
    
    /* 
      Wenn eine Abfrage mit eins zurückkehrt, dann Forwarder fragen. 
      (-1) ist Server Fehler.
    */
    switch ( ret ) {
       case 1:
         if ( askForwarder( msg, msgLen ) != 0 ) {
	      printLog( "ask forwarder", LOG_L1 );
              h.ancount = 0;
              h.aa = 0;
              h.rcode = RCODE_NAME_ERROR;
              codeHeader( *msg, &h );
         }
       break;
       
       case (-1):
              *msgLen = orgMsgLen;
              memmove( *msg, buf, *msgLen );
              extractHeader( *msg, *msgLen, &h );
              h.rcode = RCODE_SERVER_FAILURE;
              codeHeader( *msg, &h );
       break;
    }
    
    return 0;
}

/*
  Anhängen eines RR an eine Antwort.
  rr     = Anzuhängender RR.
  msg    = Antwort.
  pos    = Position in Antwort an der geschrieben werden soll.
  return = 0: Ok.
*/
int appendRR( DNSrr * rr, char * msg, int pos ) {
    int i;
    DNS_UI tmpl;
    DNS_USI tmps;
    
    i = (int)strlen( rr->name )+1/*root label*/;
    
    memmove( &msg[pos], rr->name, i );
    pos += i;
    i = pos;
    
    tmps = htons( rr->type );
    memmove( &msg[i], &tmps, 2 );
    i += 2;
        
    tmps = htons( rr->class );
    memmove( &msg[i], &tmps, 2 );
    i += 2;
        
    tmpl = htonl( rr->ttl );
    memmove( &msg[i], &tmpl, 4 );
    i += 4;
        
    tmps = htons( rr->rdlength );
    memmove( &msg[i], &tmps, 2 );
    i += 2;
        
    memmove( &msg[i], rr->rdata, rr->rdlength );
    
    return 0;
}

/*
  Header Struktur in Antwort eintragen.
  msg    = Antwort.
  header = Header der eingetragen werden soll.
  return = 0: Ok.
*/
int codeHeader( char * msg, DNSHeader * header ) {
    DNS_UCH * imsg = (DNS_UCH *)msg;
    DNS_USI tmp;
    assert( header != NULL && msg != NULL );
    
    tmp = htons( header->id );
    memmove( imsg, &tmp, 2 );
   
    imsg[2] = (header->qr << 7) ^ (header->opcode << 6) ^ (header->aa << 2) ^ 
              (header->tc << 1) ^ (header->rd);
             
    imsg[3] = (header->ra << 7) ^ (header->z << 6) ^ (header->rcode);
    
    tmp = htons( header->qdcount );
    memmove( &imsg[4], &tmp, 2 );
    
    tmp = htons( header->ancount );
    memmove( &imsg[6], &tmp, 2 );
    
    tmp = htons( header->nscount );
    memmove( &imsg[8], &tmp, 2 );
    
    tmp = htons( header->arcount );
    memmove( &imsg[10], &tmp, 2 );
    
    return 0;
}

/*
  Extrahiert aus einer Anrage den Header.
  msg    = Anfrage.
  msgLen = Länge der Farge.
  header = Ort zum ablegen der Header Daten.
*/
int extractHeader( char * msg, int msglen, DNSHeader * header ) {
    DNS_UCH * imsg = (DNS_UCH *)msg;
    DNS_USI tmp;
    assert( header != NULL && msg != NULL );
    
    if ( msglen < DNSHEADERLEN ) {
         lastError = 7;
         return (-1);
    }
     
    memmove( &tmp, &imsg[0], 2 );
    header->id = ntohs( tmp );
    
    header->qr     = imsg[2] >> 7;
    header->opcode = (imsg[2] & 0x78) >> 3;
    header->aa     = (imsg[2] & 0x04) >> 2;
    header->tc     = (imsg[2] & 0x02) >> 1;
    header->rd     = imsg[2] & 0x01;
    
    header->ra     = imsg[3] >> 7;
    header->z      = (imsg[3] & 0x70) >> 4;
    header->rcode  = imsg[3] & 0x0f;
    
    memmove( &tmp, &imsg[4], 2 );
    header->qdcount = ntohs(tmp);
    
    memmove( &tmp, &imsg[6], 2 );
    header->ancount = ntohs(tmp);
    
    memmove( &tmp, &imsg[8], 2 );
    header->nscount = ntohs(tmp);
    
    memmove( &tmp, &imsg[10], 2 );
    header->arcount = ntohs(tmp);
    
    return 0;
}

/*
  Extrahiert aus einer Anfrage die Frage.
  msg      = Anfrage.
  msgLen   = Länge der Farge.
  question = Ort zum ablegen der Frage Daten.
*/
int extractQuestion( char * msg, int msglen, DNSQuestion * question ) {
    assert( question != NULL && msg != NULL );
    
    if ( msglen < DNSHEADERLEN + 1 ) {
         lastError = 8;
         return (-1);
    }
    
    {
     int p = DNSHEADERLEN;
     DNS_UCH * imsg = (DNS_UCH *)msg;
     DNS_USI tmps;

     convertDomainName( &msg[p], question->qname, 0 );
     p += strlen(question->qname) + 1/*root label*/ + 1/*behind root label*/;
     
     memmove( &tmps, &imsg[p], 2 );
     question->qtype = ntohs( tmps );
     
     p += 2;

     memmove( &tmps, &imsg[p], 2 );
     question->qclass = ntohs( tmps );
    }
    
    return 0;
}

/*
  Wandelt einen Domainnamen in ein 
  Label(lenByte,bytes,...,...,lenByte,bytes) um und umgekehrt.
  dnIn      = Umzuwandelnder Domainname.
  dnOut     = Ziel der Umwandlung.
  direction = 1(Label2Name) | 0(Name2Lebel)
  return    = 0: Ok.
*/
int convertDomainName( char * dnIn, char * dnOut, int direction ) {
    assert( (dnIn != NULL) && (dnOut != NULL) );

    if ( direction == 0 ) {

         int srcpos = 1, dstpos = 0, len = dnIn[0];

         while ( len > 0 ) {
                 memmove( &dnOut[dstpos], &dnIn[srcpos], (size_t)len );
                 dstpos += len;
                 srcpos += len;
                 
                 len = dnIn[srcpos++];
                 
                 if ( len > 0 )
                      dnOut[dstpos++] = '.';/* else root label */
         }
         dnOut[dstpos] = '\0';
         
    } else {
         
         int srcpos=0, dstpos=1, maxLen = strlen( dnIn );
         int labelLenPos=0;
         
         while ( srcpos <=/* nicht < weil root label testen -> '\0' */ maxLen ) {
         
                 dnOut[dstpos] = dnIn[srcpos++];
                 
                 if ( dnOut[dstpos] == '.' ) {
                 
                      dnOut[labelLenPos] = dstpos-labelLenPos-1;
                      labelLenPos = dstpos;
                 
                 } else {
                 
                      if ( dnOut[dstpos] == '\0' ) {
                           dnOut[labelLenPos] = dstpos-labelLenPos-1;
                           break;
                      }
                 
                 }
                 dstpos++;
         }

    }
    
    return 0;
}

/*
  Kopiert/Wandelt in einer Zeichenkette ein angebbares Zeichen gegen
  ein anderes angebbares Zeichen um.
  in      = Zu Kopierende/Wandelnde Zeichenkette.
  out     = Ziel Zeichenkette
  replace = Zeichen das ersetzt weren soll.
  ers     = Ersetzungszeichen.
  return  = void.
*/
void strConvert( char * in, char * out, char replace, char ers ) {
    int i, len;
    len = (int)strlen(in);
    for ( i = 0; i <= len; i++ ) {
          if ( in[i] == replace ) {
               out[i] = ers;
          } else {
               out[i] = in[i];
          }
    }
}

/*
  Druckt einen Header in lesbarer Form auf stream aus.
  header = Auszugebender Header.
  stream = Ausgabe stream.
  return = void.
*/
void printHeader( DNSHeader * header, FILE * stream ) {

    fprintf(stream, "id:\t%hu\n", header->id );
    
    fprintf(stream, "qr:\t%u -> ", header->qr );
    fprintf(stream, "%s\n", ( (header->qr==0) ? "query" : "response" ) );
    
    fprintf(stream, "opcode:\t%u -> ", header->opcode );
    switch(header->opcode) {
       case 0: fprintf(stream, "standard query\n"); break;
       case 1: fprintf(stream, "inverse query\n"); break;
       case 2: fprintf(stream, "status request\n"); break;
    }
    
    fprintf(stream, "aa:\t%u -> ", header->aa );
    fprintf(stream, "%s\n", ((header->aa==0) ? "non authoritative answer" : "authoritative answer" ) );
        
    fprintf(stream, "tc:\t%u -> ", header->tc );
    fprintf(stream, "%s\n", ((header->tc==0) ? "not truncated" : "truncated" ) );
    
    fprintf(stream, "rd:\t%u -> ", header->rd );
    fprintf(stream, "%s\n", ((header->rd==0) ? "no recursion desired" : "recursion desired") );
    
    fprintf(stream, "ra:\t%u -> ", header->ra );
    fprintf(stream, "%s\n", ((header->ra==0) ? "no recursion available" : "recursion available" ) );
    
    fprintf(stream, "z:\t%u -> ", header->z );
    fprintf(stream, "%s\n", ((header->z==0) ? "reserved" : "reserved" ) );
    
    fprintf(stream, "rcode:\t%u -> ", header->rcode );
    switch(header->rcode) {
       case 0: fprintf(stream, "no error\n"); break;
       case 1: fprintf(stream, "format error\n"); break;
       case 2: fprintf(stream, "server failure\n"); break;
       case 3: fprintf(stream, "name error\n"); break;
       case 4: fprintf(stream, "not implemented\n"); break;
       case 5: fprintf(stream, "refused\n"); break;
    }
    
    fprintf(stream, "qdcount:\t%hu\n", header->qdcount );
    fprintf(stream, "ancount:\t%hu\n", header->ancount );
    fprintf(stream, "nscount:\t%hu\n", header->nscount );
    fprintf(stream, "arcount:\t%hu\n", header->arcount );

}

/*
  Druckt eine Typ Angabe in lesbarer Form auf stream aus.
  type   = Auszugebender Typ.
  stream = Ausgabe stream.
  return = void.
*/
void printType( DNS_USI type, FILE * stream ) {    
    switch( type ) {
       /* type */
       case   1: fprintf(stream, "A"); break;
       case   2: fprintf(stream, "NS"); break;
       case   3: fprintf(stream, "MD"); break;
       case   4: fprintf(stream, "MF"); break;
       case   5: fprintf(stream, "CNAME"); break;
       case   6: fprintf(stream, "SOA"); break;
       case   7: fprintf(stream, "MB"); break;
       case   8: fprintf(stream, "MG"); break;
       case   9: fprintf(stream, "MR"); break;
       case  10: fprintf(stream, "NULL"); break;
       case  11: fprintf(stream, "WKS"); break;
       case  12: fprintf(stream, "PTR"); break;
       case  13: fprintf(stream, "HINFO"); break;
       case  14: fprintf(stream, "MINFO"); break;
       case  15: fprintf(stream, "MX"); break;
       case  16: fprintf(stream, "TXT"); break;
       /* qtype */
       case 252: fprintf(stream, "AXFR"); break;
       case 253: fprintf(stream, "MAILB"); break;
       case 254: fprintf(stream, "MAILA"); break;
       case 255: fprintf(stream, "ALL"); break;
    }
}

/*
  Druckt eine Klassen Angabe in lesbarer Form auf stream aus.
  class  = Auszugebende Klasse.
  stream = Ausgabe stream.
  return = void.
*/
void printClass( DNS_USI class, FILE * stream ) {
    switch( class ) {
       case 1: fprintf(stream, "IN"); break;
       case 2: fprintf(stream, "CS"); break;
       case 3: fprintf(stream, "CH"); break;
       case 4: fprintf(stream, "HS"); break;
    }
}

/*
  Druckt eine Frage in lesbarer Form auf stream aus.
  question  = Auszugebende Frage.
  stream    = Ausgabe stream.
  return    = void.
*/
void printQuestion( DNSQuestion * question, FILE * stream ) {
    fprintf(stream, "qname:\t%s\n", question->qname );
    
    fprintf(stream, "qtype:\t%u -> ",question->qtype );
    printType( question->qtype, stream );
    fprintf(stream, "\n");
    
    fprintf(stream, "qclass:\t%u -> ",question->qclass );
    printClass( question->qclass, stream );
    fprintf(stream, "\n");
}

/*
  Druckt einen RR in lesbarer Form auf stream aus(bis auf "rdata").
  rr     = Auszugebender RR.
  stream = Ausgabe stream.
  return = void.
*/
void printRR( DNSrr * rr, FILE * stream ) {
    int i;
    fprintf(stream, "name: %s\n", rr->name );
    fprintf(stream, "type: %d\n", rr->type );
    fprintf(stream, "class: %d\n", rr->class );
    fprintf(stream, "ttl: %d\n", rr->ttl );
    fprintf(stream, "rdlength: %d\n", rr->rdlength );
    
    fprintf(stream, "rdata: " );
    for ( i=0; i<rr->rdlength; i++ ) 
          fprintf(stream, "%u ", (DNS_UCH)rr->rdata[i] );
    fprintf(stream, "\n" );
}

/*EOF*/
