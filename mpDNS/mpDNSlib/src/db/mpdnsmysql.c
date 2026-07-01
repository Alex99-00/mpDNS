/*

  Programm/Lib : mpDNSlib
  Datei        : mpdnsmysql.c
  Version      : v0.7r
  
  Beschreibung:
  Posix Thread der Anfragen aus der eingangs Queue bearbeitet und die
  Antworten in die ausgangs Queue stellt.
  
  Besonderheit:
  Die Daten in der MySQL Datenbank müssen! eine gültige Syntax haben.
  Es werden keine Prüfungen vorgenommen.
  
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if (defined(_WIN32))
#include <windows.h>
#include <config-win.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#include <netinet/in.h> /*htons...*/
#endif

#include <mpdnsmysql.h>
#include <alloc.h>
#include <error.h>
#include <dns.h>
#include <debug.h>


/* MySQL Verbindung. Dauerhaft. */
MYSQL mysql;

/*
  Mit MySQL verbinden.
  host   = MySQL Hostname. ! IP nehmen, "ich" bin der DNS !
  user   = Username auf MySQL Server.
  passwd = Passwort auf MySQL Server.
  db     = Datenbank Name.
  port   = Port des MySQL Server.
  return = (-1): Keine Verbindung möglich.
             0 : Verbindung hergestellt.
*/
int connectToMysql( char * host, char * user, char * pass, char * db, unsigned int port ) {

    mysql_init( &mysql );
    
    /*                      MYSQL, host, user, pass, db, port, unixSocket, client_flag */
    if (mysql_real_connect( &mysql, host, user, pass, db, port, 0, 0 ) == NULL) {
        lastError = 4;
        return (-1);
    }

    return 0;
}

/*
  MySQL Verbindung abbauen.
*/
void disconnectFromMysql() {
    mysql_close( &mysql );
}

/*
  MySQL Fehler in Log Datei schreiben.
*/
void mysqlErrorToLog(void) {
    char buf[4096]; /* assume is is enough !!! */
    
    sprintf( buf, "%s:%d\n", mysql_error( &mysql ), mysql_errno( &mysql )  );
    
    printLog( buf, LOG_L1 );
}

/*
  SOA RR für eine Zone abfragen.
  zone   = Zonenname.
  rr     = Ablageplatz für gefundene RRs.
  return = (-1) : Fehler.
           (>=0): Anzahl gefundener Einträge.
*/
int queryMysqlSOA( char * zone, DNSrr *** rr ) {
    MYSQL_RES *result;
    MYSQL_ROW row;
    size_t numRows;
    char query[1024];
    int i;
    DNS_UI temp;

    /* Anfrage aufbauen. */
    memset( query, 0, 1024 );
    strcat( query, "SELECT Name, Mail, Serial, Refresh, Retry, Expire, Minimum\
    FROM SOA WHERE Name = '" );
    strcat( query, zone );
    strcat( query, "'" );
    printLog(query, LOG_L3);
    
    /* Anfrage an MySQL senden. */
    if ( mysql_query( &mysql, query ) != 0 ) {         
         mysqlErrorToLog();
         lastError = 5;
         return (-1);
    }
    
    /* Resultat der Anfrage abholen. */
    if ( (result = mysql_store_result( &mysql )) == NULL ) {
         mysqlErrorToLog();
         lastError = 6;
         return (-1);
    }
    
    /* Anzahl Resultate merken. */
    numRows = (size_t)mysql_num_rows( result );
    
    /* Entsprechend Speicher besorgen, für numRows RR Strukturen. */
    *rr = MALLOC( sizeof(DNSrr *) * numRows );
    if ( *rr == NULL ) {
         return (-1);
    }

    /* Alle Resultate in RRs umwandeln. */
    i = 0;
    while ( (row = mysql_fetch_row( result )) != NULL ) {
            
            /* Für jeden RR Speicher besorgen und "aus nullen". */
            (*rr)[i] = MALLOC( sizeof(DNSrr) );
            
            if ( (*rr)[i] == NULL ) {
                 return (-1);
            }
            memset( (*rr)[i], 0, sizeof(DNSrr) );

            /* SOA Daten in RR eintragen. */
            convertDomainName( row[0], (*rr)[i]->name, 1 );
            (*rr)[i]->rrlen = strlen( row[0] )+1/* len of first label */+1/* root label */;
   
            /* hton(l)(s) in codeRR() */
            (*rr)[i]->type = RRT_SOA;
            (*rr)[i]->class = RRC_IN;
            (*rr)[i]->ttl = atoi( row[6] );
            (*rr)[i]->rrlen += ( 2 + 2 + 4 );
            
            /* full size -> realloc nachdem de exakte länge bekannt ist. */
            (*rr)[i]->rdata = MALLOC( (size_t)(512 * sizeof(char)) );
            if ( (*rr)[i]->rdata == NULL ) {
                 return (-1);
            }
   
            /* SOA Name. */
            convertDomainName( row[0], (*rr)[i]->rdata, 1 );
            (*rr)[i]->rdlength = strlen(row[0])+1+1;
   
            /* SOA Mail. */
            convertDomainName( row[1], &(*rr)[i]->rdata[(*rr)[i]->rdlength], 1 );
            (*rr)[i]->rdlength += strlen(row[1])+1+1;

            (*rr)[i]->rrlen += (*rr)[i]->rdlength;   

            /* Serial. */
            temp = atoi( row[2] );
            temp = htonl( temp );
            memmove( &(*rr)[i]->rdata[(*rr)[i]->rdlength], &temp, 4 );
            (*rr)[i]->rdlength += 4;
            
            /* Refresh. */
            temp = atoi(row[3]);
            temp = htonl( temp );
            memmove( &(*rr)[i]->rdata[(*rr)[i]->rdlength], &temp, 4 );
            (*rr)[i]->rdlength += 4;
                  
            /* Retry. */
            temp = atoi(row[4]);
            temp = htonl( temp );
            memmove( &(*rr)[i]->rdata[(*rr)[i]->rdlength], &temp, 4 );
            (*rr)[i]->rdlength += 4;
                  
            /* Expire. */
            temp = atoi(row[5]);
            temp = htonl( temp );
            memmove( &(*rr)[i]->rdata[(*rr)[i]->rdlength], &temp, 4 );
            (*rr)[i]->rdlength += 4;
                  
            /* Minimum. */
            temp = atoi(row[6]);
            temp = htonl( temp );
            memmove( &(*rr)[i]->rdata[(*rr)[i]->rdlength], &temp, 4 );
            (*rr)[i]->rdlength += 4;
                
            /* Interne Gesamtlänge der RR Struktur merken. */
            (*rr)[i]->rrlen += (4 + 4 + 4 + 4 + 4 + 2 /* (*rr)[i]->rdlength -> uint16 */);
   
            /* Endgültige Größe setzen. */
            (*rr)[i]->rdata = REALLOC( (*rr)[i]->rdata, ((size_t)(*rr)[i]->rdlength)*sizeof(char) );
            if ( (*rr)[i]->rdata == NULL ) {
                 return (-1);
            }

            i++; /* Nächstes Resultat */
    }/* while */
    
    /* Resultat freigeben. */
    mysql_free_result( result );
    
    /* Anzahl RRs zurückgeben. */
    return (int)numRows;
}

/*
  PTR RR für eine Adresse abfragen.
  address = IP (1.10.168.192 NICHT 192.168.10.1). 
  rr      = Ablageplatz für gefundene RRs.
  return  = (-1) : Fehler.
            (>=0): Anzahl gefundener Einträge.
*/
int queryMysqlPTR( char * address, DNSrr *** rr ) {
    MYSQL_RES *result;
    MYSQL_ROW row;
    size_t numRows;
    char query[1024];
    int i;
    
    /* Anfrage aufbauen. */
    memset( query, 0, 1024 );
    strcat( query, "SELECT Address, Name, TTL FROM PTR WHERE Address = '" );
    strcat( query, address );
    strcat( query, "'" );
    printLog(query, LOG_L3);
        
    
    /* Anfrage an MySQL senden. */
    if ( mysql_query( &mysql, query ) != 0 ) {         
         mysqlErrorToLog();
         lastError = 5;
         return (-1);
    }
    
    /* Resultat der Anfrage abholen. */
    if ( (result = mysql_store_result( &mysql )) == NULL ) {
         mysqlErrorToLog();
         lastError = 6;
         return (-1);
    }
    
    /* Anzahl Resultate merken. */
    numRows = (size_t)mysql_num_rows( result );
    
    /* Entsprechend Speicher besorgen, für numRows RR Strukturen. */        
    *rr = MALLOC( sizeof(DNSrr *) * numRows );
    if ( *rr == NULL ) {
         return (-1);
    }
    
    /* Alle Resultate in RRs umwandeln. */
    i = 0;
    while ( (row = mysql_fetch_row( result )) != NULL ) {
            
            /* Für jeden RR Speicher besorgen und "aus nullen". */
            (*rr)[i] = MALLOC( sizeof(DNSrr) );
            
            if ( (*rr)[i] == NULL ) {
                 return (-1);
            }
            memset( (*rr)[i], 0, sizeof(DNSrr) );
            
            
            /* PTR Daten in RR eintragen. */
            memset( query, 0, 1024 );/* reuse query buffer */
            
            /* Adresse. */
            memmove( query, row[0], strlen(row[0]) );
            memmove( &query[strlen(row[0])], ".in-addr.arpa", 13+1/*0*/);
            convertDomainName( query, (*rr)[i]->name, 1 );
            (*rr)[i]->rrlen = strlen( (*rr)[i]->name )+1/* root label */;
            
            /* hton(l)(s) in codeRR() */
            (*rr)[i]->type = RRT_PTR;
            (*rr)[i]->class = RRC_IN;
            (*rr)[i]->ttl = atoi( row[2] );
            (*rr)[i]->rrlen += ( 2 + 2 + 4 );
         
            /* Name. */
            (*rr)[i]->rdata = MALLOC( (strlen(row[1])+1/*first label*/+1/*root label*/) * sizeof(char) );
            if ( (*rr)[i]->rdata == NULL ) {
                 return (-1);
            }
            convertDomainName( row[1], (*rr)[i]->rdata, 1 );

            /* Endgrößen. */
            (*rr)[i]->rdlength = (strlen(row[1])+1/*first label*/+1/*root label*/);
            (*rr)[i]->rrlen += (*rr)[i]->rdlength;
            (*rr)[i]->rrlen += 2;/* rdlength -> uint16 */
        
            i++; /* Nächstes Resultat */
    }/* while */

    /* Resultat freigeben. */    
    mysql_free_result( result );

    /* Anzahl RRs zurückgeben. */    
    return (int)numRows;
}

/*
  Entry RR(A, MX, NS, CNAME) für eine Zone abfragen.
  zone   = Zonenname.
  name   = Name des Records(Domainname).
  type   = Typ der zu suchenden RRs.
  rr     = Ablageplatz für gefundene RRs.
  return = (-1) : Fehler.
           (>=0): Anzahl gefundener Einträge.
*/
int queryMysqlEntry( char * zone, char * name, unsigned short int type, DNSrr *** rr ) {
    MYSQL_RES *result;
    MYSQL_ROW row;
    size_t numRows;
    char query[1024];
    int i;
    
    /* Anfrage aufbauen. */
    memset( query, 0, 1024 );
    strcat( query, "SELECT Name, Data, Type, TTL FROM " );
    strcat( query, zone );
    strcat( query, "_RR " );
    strcat( query, "WHERE Name = '" );
    strcat( query, name );
    strcat( query, "' AND " );    
    strcat( query, "Type = '" );
    sprintf( &query[strlen(query)], "%u", type );
    strcat( query, "'" );
    printLog(query, LOG_L3);
    
    /* Anfrage an MySQL senden. */
    if ( mysql_query( &mysql, query ) != 0 ) {
         mysqlErrorToLog();
         lastError = 5;
         return (-1);
    }
    
    /* Resultat der Anfrage abholen. */
    if ( (result = mysql_store_result( &mysql )) == NULL ) {
         mysqlErrorToLog();
         lastError = 6;
         return (-1);
    }
    
    /* Anzahl Resultate merken. */
    numRows = (size_t)mysql_num_rows( result );
    
    /* Entsprechend Speicher besorgen, für numRows RR Strukturen. */
    *rr = MALLOC( sizeof(DNSrr *) * numRows );
    if ( *rr == NULL ) {
         return (-1);
    }

    /* Alle Resultate in RRs umwandeln. */
    i = 0;
    while ( (row = mysql_fetch_row( result )) != NULL ) {
    
            /* Für jeden RR Speicher besorgen und "aus nullen". */
            (*rr)[i] = MALLOC( sizeof(DNSrr) );
            
            if ( (*rr)[i] == NULL ) {
                 return (-1);
            }
            memset( (*rr)[i], 0, sizeof(DNSrr) );

            /* Entry Daten in RR eintragen. */
            convertDomainName( row[0], (*rr)[i]->name, 1 );
            (*rr)[i]->rrlen = strlen( row[0] )+1+1/* first label len. + root label */;
     
            /* hton(l)(s) in codeRR() */
            (*rr)[i]->type  = type;
            (*rr)[i]->class = RRC_IN;
            (*rr)[i]->ttl   = atoi( row[3] );
            (*rr)[i]->rrlen += ( 2 + 2 + 4 );


            /* Je nach Typ speicher besorgen und "aus nullen". */
            switch( type ) {
               case RRT_A:
                     (*rr)[i]->rdlength = 4; /* ipv4 */
               break;
       
               case RRT_NS:
                     (*rr)[i]->rdlength = strlen( row[1] ) +1/*first label len*/+1/*root label*/;
               break;
       
               case RRT_CNAME:
                     (*rr)[i]->rdlength = strlen( row[1] ) +1/*first label len*/+1/*root label*/;
               break;
               
               case RRT_MX:{
                     (*rr)[i]->rdlength = 2/*16bit number*/;
                     (*rr)[i]->rdlength += strlen( &row[1][strcspn( row[1], " " )+1/*steht sonst auf wspace*/] ) +1/*first label len*/+1/*root label*/;
               }break;
            }        
            
            (*rr)[i]->rdata = MALLOC( (*rr)[i]->rdlength * sizeof(char) );
            if ( (*rr)[i]->rdata == NULL ) {
                 return (-1);
            }
            memset((*rr)[i]->rdata, 0, (*rr)[i]->rdlength * sizeof(char) );
            
            
            /* Je nach Typ RR.rdata mit Daten füllen. */
            switch( type ) {
               case RRT_A:{
                       char buf[4] = {'\0','\0','\0','\0'};
                       char * data = row[1];
                       int q=0,q2=0,q3=0,len=(int)strlen(data);
                       
                       while ( q <= len ) {

                               if ( (data[q] == '.') || (data[q] == '\0') ) {
                       
                                    if ( q2 > 0 ) {
                                    
                                         buf[q2/*4*/]='\0';
                                         (*rr)[i]->rdata[q3] = atoi(buf);
                                         q3++;
                                         q2=0;
                                         
                                    }
                                    q++;
                                    continue;
                               }
                       
                               buf[q2++] = data[q];
                               q++;
                       }
               }
               break;
       
               case RRT_NS:
                       convertDomainName( row[1], (*rr)[i]->rdata, 1 );
               break;
       
               case RRT_CNAME:
                       convertDomainName( row[1], (*rr)[i]->rdata, 1 );
               break;
               
               case RRT_MX:{
                     /* z.B.: "10 nathan.ax", "20 jaba.nathan.ax" */
                     DNS_USI num;
                     size_t lenNum = strcspn( row[1], " " );
                     row[1][lenNum] = '\0';
                     num = atoi( row[1] );
                     num = htons( num );
                     memmove( (*rr)[i]->rdata , &num, 2 );
                     convertDomainName( &row[1][lenNum+1], &((*rr)[i]->rdata[2]), 1 );
               }break;
            }

            (*rr)[i]->rrlen += 2;/* rdlength -> uint16 */
            (*rr)[i]->rrlen += (*rr)[i]->rdlength; /* rdata */

            i++; /* Nächstes Resultat */
    }/* while */

    /* Resultat freigeben. */    
    mysql_free_result( result );
    
    /* Anzahl RRs zurückgeben. */
    return (int)numRows;
}

/*
  Alle Zonennamen die zur Verfügung stehen abfragen.
  zones  = Ablageplatz für gefundene Zonennamen.
  return = (-1) : Fehler.
           (>=0): Anzahl gefundener Einträge.
*/
int queryMysqlZoneNames( char *** zones ) {
    MYSQL_RES *result;
    MYSQL_ROW row;
    size_t numRows;
    char query[1024];
    int i;
    
    /* Anfrage aufbauen. */
    memset( query, 0, 1024 );
    strcat( query, "SELECT Name FROM SOA WHERE 1" );
    printLog(query, LOG_L3);
    
    /* Anfrage an MySQL senden. */
    if ( mysql_query( &mysql, query ) != 0 ) {
         mysqlErrorToLog();
         lastError = 5;
         return (-1);
    }
    
    /* Resultat der Anfrage abholen. */
    if ( (result = mysql_store_result( &mysql )) == NULL ) {
         mysqlErrorToLog();
         lastError = 6;
         return (-1);
    }
    
    /* Anzahl Resultate merken. */
    numRows = (size_t)mysql_num_rows( result );
    
    /* Entsprechend Speicher besorgen, für numRows Tabellenplätze in zones. */        
    *zones = MALLOC( sizeof(char *) * numRows );
    if ( *zones == NULL ) {
         return (-1);
    }
    
    /* Alle Resultate in zones Tabelle eintragen. */
    i = 0;
    while ( (row = mysql_fetch_row( result )) != NULL ) {
            
            /* Für jeden Tabellenplatz Speicher besorgen. */
            (*zones)[i] = MALLOC( strlen(row[0])+1 );
    
            if ( (*zones)[i] == NULL ) {
                 return (-1);
            }
            
            /* Zonennamen in Tabelle eintragen. */
            memmove( (*zones)[i], row[0], strlen(row[0])+1/*null byte*/ );
            
            i++; /* Nächstes Resultat */
    }/* while */

    /* Resultat freigeben. */    
    mysql_free_result( result );

    /* Anzahl Zonen zurückgeben. */    
    return (int)numRows;
}

/*EOF*/
