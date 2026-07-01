#include <stdio.h>

#if (defined(_WIN32))
#include <windows.h>
#define socklen_t size_t
#else
#include <unistd.h>
#include <sys/socket.h>   /* Socket Funktionen */
#include <netinet/in.h>   /* Konstanten */
#include <arpa/inet.h>    /* Umwandlungsfunktionen, Host-->Network...*/
#include <netdb.h>        /* Info-Funktionen fuer Sockets, gethostbyname(...)... */
#include <sys/time.h>     /* timeval */
#endif

#include <debug.h>

/*
  Lesen von Daten auf UDP Socket.
  sock   = Socket von dem gelesen werden soll.
  buf    = Speicherplatz für gelesene Daten.
  maxBuf = Maximal zu lesende Daten.
  addr   = Speicherplatz um Senderadresse abzulegen.
  usec   = Zeit die maximal gewartet werden soll.
  return = Anzahl gelesener Bytes.
*/
int readUdp( int sock, char * buf, size_t maxBuf, struct sockaddr_in * addr, int usec ) {
    int rb = 0;
    socklen_t addrLen = sizeof( *addr );

    fd_set readSet;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = usec;
    
    FD_ZERO( &readSet );
    FD_SET( sock, &readSet );
    
    if ( select( sock+1, &readSet, NULL, NULL, &tv ) > 0 ) {
      if ( FD_ISSET( sock, &readSet ) ) {
           if ( (rb = recvfrom( sock, (void *) buf, maxBuf, 0, (struct sockaddr *)addr, &addrLen )) < 0 ){

           }
      }
    }
    
    return rb;
}

/*
  Schreiben auf UDP Socket.
  sock       = Socket auf den geschrieben werden soll.
  buf        = Daten die zu schreiben sind.
  writeBytes = Anzahl zu schreibender Daten.
  addr       = Adresse an die gesendet werden soll.
  return     = Anzahl geschriebener Daten.
*/
int writeUdp( int sock, char * buf, size_t writeBytes, struct sockaddr_in * addr ) {
    int wb;     
    static socklen_t addrLen = sizeof( *addr );
     
    if ( (wb = sendto( sock, (void *)buf, writeBytes, 0, (struct sockaddr *)addr, addrLen )) < 0 ) {
    }
    
    return wb;
}

/*
  Eine sockaddr_in Struktur mit Daten füllen. 
  port = Port für sockaddr_in Struktur.
  name = Name für sockaddr_in Struktur.
  addr = Speicherort(muß schon Speicher für existieren).
*/
int genSockaddressStruct( unsigned short int port, char *name, struct sockaddr_in * addr ) {
    struct hostent *hostinfo; /* Zeiger auf statischen! Speicherplatz->nicht freigeben*/

    /* Auf INADDR_ANY Konstante testen. */
    if ( memcmp(name, "INADDR_ANY", 10)==0 ) {
    
         addr->sin_addr.s_addr = htonl(INADDR_ANY);
         
    } else { /* Sonst angegebenen Hostnamen auswerten/testen. */
    
         hostinfo = gethostbyname( name );
         
         if ( hostinfo == NULL ) {
              printLog( "genSockaddressStruct.hostinfo() error", LOG_L1 );
              return (-1);
         }
         
         addr->sin_addr = *((struct in_addr *) hostinfo->h_addr_list[0]); /* IP */
         
    }
    
    addr->sin_family = AF_INET;       /* Adress Familie*/
    addr->sin_port   = htons( port ); /* Port in NetworkByteOrder */

    return 0;
}

/*
  UDP "Server" Socket erzeugen.
  port   = Portnummer.
  name   = Name auf den gebunden werden soll.
  return =  (-1): Fehler.
           (>=0): Socket descriptor.
*/
int getUdpServerSocket( unsigned short int port, char *name ) {
    struct sockaddr_in addr;   /* Haelt die Server Daten */
    int sock;

#if (defined(_WIN32))
	{
	 WSADATA wdata;
     if ( WSAStartup( 0x0202, &wdata ) < 0 ) {
          printLog( "getUdpServerSocket.WSAStartup() error", LOG_L1 );
          return (-1);
     }
	}
#endif

    if ( genSockaddressStruct( port, name, &addr ) < 0 ) {
         printLog( "getUdpServerSocket.genSockaddressStruct() error", LOG_L1 );
         return (-1);
    }
    
    /* UDP/IP sock erstellen. */
    sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( sock < 0 ) {
         printLog( "getUdpServerSocket.socket() error", LOG_L1 );
         return (-1);
    }
    
    /* sock an Server Adresse/Port binden. */
    if ( ( bind( sock, (struct sockaddr *) &addr, sizeof(addr) ) < 0 ) ) {
         printLog( "getUdpServerSocket.bind() error", LOG_L1 );
         return (-1);
    }
    
    /* Komplet Initialisierten Server socket zurueckgeben. */
    return sock;
}


/*
  UDP Client Socket erstellen.
  return = (-1): Fehler.
          (>=0): Socket descriptor.
*/
int getUdpClientSocket() {
    int sock;

#if (defined(_WIN32))
	{
	 WSADATA wdata;
     if ( WSAStartup( 0x0202, &wdata ) < 0 ) {
          printLog( "getUdpClientSocket.WSAStartup() error", LOG_L1 );
          return (-1);
     }
	}
#endif

    /* UDP/IP socket erstellen. */
    sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( sock < 0 ) {
         printLog( "getUdpClientSocket.socket() error", LOG_L1 );
         return (-1);
    }
    
    /* Komplet Initialisierten Client socket zurueckgeben. */
    return sock;
}


/*
  Schließen eines UDP Sockets.
  sock = Zu schließender Socket.
*/
void closeUdpSocket( int sock ) {

#if (defined (_WIN32))
    closesocket(sock);
    WSACleanup();
#else
    (void)close(sock);
#endif

}

/*EOF*/
