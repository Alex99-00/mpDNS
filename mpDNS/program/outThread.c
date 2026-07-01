/*

  Programm/Lib : mpDNS
  Datei        : outThread.c
  Version      : v0.1r
  
  Beschreibung:
  Posix Thread der die Einträge der Ausgangs Queue versendet.
  
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define __USE_BSD /* usleep */
#include <unistd.h>
#undef __USE_BSD

#define __USE_POSIX /* sigemptyset, sigaddset */
#include <signal.h>
#undef __USE_POSIX

#include <main.h>
#include <processing.h>
#include <queue.h>
#include <udp.h>
#include <error.h>
#include <debug.h>

/*---------------------------------------------------------------------------*/
/* declaration in processing.c */
/*extern pthread_mutex_t mutexIn;*/
extern pthread_mutex_t mutexOut;
/*extern Queue queueIn;*/
extern Queue queueOut;
extern pthread_t /*inThread,*/ outThread/*, processingThread*/;
extern int /*runInThread,*/ runOutThread/*, runProcessingThread*/;
extern int errorThread;
/* Port/IP des Servers. */
extern unsigned short int dnsPort;
extern char * dnsIP;

/* Socket zum Senden/Empfangen */
extern int ssocket;
/*---------------------------------------------------------------------------*/


/* 
  Thread der die Einträge der Ausgangs Queue versendet.
  data   = An den Thread übergebene, wahlfreie Daten(PThread default).
  return = Wahlfreie Daten(PThread default). 
*/
void *outThreadStart( void * data ) {
    Node * node;
    int wb;
    char logBuf[512];
    
    /* Signale werden im main Thread behandelt */    
    sigset_t sigs, oldsigs;

    sigemptyset( &sigs );
    sigaddset( &sigs, SIGINT );
    sigaddset( &sigs, SIGTERM );
    sigaddset( &sigs, SIGHUP );
    sigaddset( &sigs, SIGABRT );
    pthread_sigmask(SIG_SETMASK, &sigs, &oldsigs);

    /* Warten bis Thread loslaufen darf. */
    while ( ! runOutThread ) { usleep(10000); }

    /* Thread */
    while ( runOutThread ) {
            /*
              Warten bis Daten in queueOut vorhanden sind.
              Daten aus Queue austragen.
              Daten versenden.
              Daten löschen.
            */
            
            while ( NODE_IS_EMPTY( queueOut.first ) ) { 
                    if ( ! runOutThread )
                         break;
                    usleep(100);
            }

            if ( ! runOutThread )
                 break;


            if ( pthread_mutex_lock( &mutexOut ) != 0 ) {
                 dnsLog( "outThread mutex lock error", LOG_L1 );
                 errorThread = 1;
                 break;
            }
            
            node = queueRemoveFirstElement( &queueOut );
            
            if ( pthread_mutex_unlock( &mutexOut ) != 0 ) {
                 dnsLog( "outThread mutex unlock error", LOG_L1 );
                 errorThread = 1;
                 break;
            }
            
            wb = writeUdp( ssocket, node->buf, node->bufLen, node->addr );
            
            if ( wb != node->bufLen ) {                 
                 sprintf( logBuf, "outThreadStart->writeUdp: wb != node->bufLen -> %d != %d", wb, node->bufLen );
                 dnsLog( logBuf, 1 );
            }

            nodeFree( node );
            node = NULL;
    }

    /* Thread aufräumen. */
    if ( node != NULL )
         nodeFree( node );
    pthread_mutex_destroy( &mutexOut );
    dnsLog( "outThread cleanup ok", LOG_L2 );
    pthread_exit( NULL );
    return NULL;
}

/*EOF*/
