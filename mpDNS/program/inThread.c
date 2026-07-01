/*

  Programm/Lib : mpDNS
  Datei        : inThread.c
  Version      : v0.2r
  
  Beschreibung:
  Posix Thread der auf Anfragen wartet und die eingangs Queue mit 
  eingehenden Anfragen füllt.
  
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define __USE_BSD /* usleep */
#include <unistd.h>
#undef __USE_BSD

#define __USE_POSIX /* sigemptyset, sigaddset */
#include <signal.h>
#undef __USE_POSIX


#include <main.h>
#include <processing.h>
#include <error.h>
#include <alloc.h>
#include <queue.h>
#include <udp.h>
#include <debug.h>


/*---------------------------------------------------------------------------*/
/* Deklaration in processing.c */
extern pthread_mutex_t mutexIn;
/*extern pthread_mutex_t mutexOut;*/
extern Queue queueIn;
extern Queue queueOut;
extern pthread_t inThread/*, outThread, processingThread*/;
extern int runInThread/*, runOutThread, runProcessingThread*/;
extern int errorThread;

/* Port/IP des Servers. */
extern unsigned short int dnsPort;
extern char * dnsIP;

/* Socket zum Senden/Empfangen */
extern int ssocket;
/*---------------------------------------------------------------------------*/


/* 
  Thread der auf eigehende verbindungen wartet und diese in die eingangs Queue
  stellt.
  data   = An den Thread übergebene, wahlfreie Daten(PThread default).
  return = Wahlfreie Daten(PThread default).
*/
void *inThreadStart( void * data ) {
    int rb;
    struct sockaddr_in *addr;
    char buf[4096];
    size_t maxBuf = 4096;
    Node *node;
    
    /* Signale werden im main Thread behandelt. */
    sigset_t sigs, oldsigs;
    
    sigemptyset( &sigs );
    sigaddset( &sigs, SIGINT );
    sigaddset( &sigs, SIGTERM );
    sigaddset( &sigs, SIGHUP );
    sigaddset( &sigs, SIGABRT );
    pthread_sigmask(SIG_SETMASK, &sigs, &oldsigs);
    
    
    /* Warten bis Thread loslaufen darf. */
    while ( ! runInThread ) { usleep(1000); } 
    
    
    /* Speicher für sockaddr_in Struktur besorgen. */
    addr = (struct sockaddr_in *)MALLOC( sizeof(struct sockaddr_in) );
    if ( addr == NULL ) {
         runInThread = 0;
         errorThread = 1;
    }
    
    /* Thread */
    while ( runInThread ) {
            
            /* Bei langsamer Verarbeitung warten. */
            if ( ( queueCountElements( &queueIn ) > 100 ) || 
                 ( queueCountElements( &queueOut ) > 100 ) ) {
                 usleep(100);
                 continue;
            }
            
            /* 
              Polling! 
              Eingehende Pakete lesen.
            */
            rb = readUdp( ssocket, buf, maxBuf, addr, 1000*50/*usec, auf Daten warten*/ );
            
            if ( rb <= 0 ) {
                 /* Polling leer. */
                 continue;
            } else {
                 /* 
                    Paket gelesen, nun an Queue anhängen.
                    Neue Node erzeugen. 
                    Speicher für Frage besorgen.
                    Frage in Speicher kopiern.
                    Sender Adresse/Port einhängen.
                    Node in Queue anhängen(Mutex gesichert).
                    Neuen Speicher für addr besorgen
                    (alter wird in nodeFree() freigegeben).                    
                 */
                 
                 node = nodeNew();
                 if ( node == NULL ) {
                      errorThread = 1;
                      break;
                 }
                 
                 node->buf = MALLOC( rb * sizeof(char) );
                 if ( node->buf == NULL ) {
                      errorThread = 1;
                      break;
                 }
                 
                 memmove( node->buf, buf, rb );
                 node->bufLen = rb;
                 node->addr = addr;
                 addr = NULL;
                 
                 if ( pthread_mutex_lock( &mutexIn ) != 0 ) {
                      dnsLog( "inThread mutex lock error", LOG_L1 );
                      errorThread = 1;
                      break;
                 }
                 
                 queueAppend( &queueIn, node );                 
                 
                 if ( pthread_mutex_unlock( &mutexIn ) != 0 ) {
                      dnsLog( "inThread mutex unlock error", LOG_L1 );
                      errorThread = 1;
                      break;
                 }
                 
                 node = NULL;
                 
                 addr = (struct sockaddr_in *)MALLOC( sizeof(struct sockaddr_in) );
                 if ( addr == NULL ) {
                      errorThread = 1;
                      break;
                 }
            }/*else*/
            
    }

         
    /* Thread aufräumen. */
    if ( node != NULL )
         nodeFree( node );
    FREE( addr );
    pthread_mutex_destroy( &mutexIn );
    dnsLog( "inThread cleanup ok", LOG_L2 );
    pthread_exit( NULL );

    return NULL;
}

/*EOF*/
