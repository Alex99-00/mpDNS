/*

  Programm/Lib : mpDNS
  Datei        : processingThread.c
  Version      : v0.2r
  
  Beschreibung:
  Posix Thread der Anfragen aus der eingangs Queue bearbeitet und die
  Antworten in die ausgangs Queue stellt.
  
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define __USE_BSD  /* usleep */
#include <unistd.h>
#undef __USE_BSD

#define __USE_POSIX /* sigemptyset, sigaddset */
#include <signal.h>
#undef __USE_POSIX

#include <main.h>
#include <processing.h>
#include <queue.h>
#include <error.h>
#include <dns.h>

/*---------------------------------------------------------------------------*/
/* declaration in processing.c */
extern pthread_mutex_t mutexIn;
extern pthread_mutex_t mutexOut;
extern Queue queueIn;
extern Queue queueOut;
extern pthread_t inThread, outThread, processingThread;
extern int runInThread, runOutThread, runProcessingThread;
extern int errorThread;
/*---------------------------------------------------------------------------*/


/*
  Thread der Anfragen aus der eingangs Queue bearbeitet und die
  Antworten in die ausgangs Queue stellt.
  data   = An den Thread übergebene, wahlfreie Daten(PThread default).
  return = Wahlfreie Daten(PThread default).
*/
void *processingThreadStart( void * data ) {
    Node * node;

    /* Signale werden im main Thread behandelt */
    sigset_t sigs, oldsigs;
    
    sigemptyset( &sigs );
    sigaddset( &sigs, SIGINT );
    sigaddset( &sigs, SIGTERM );
    sigaddset( &sigs, SIGHUP );
    sigaddset( &sigs, SIGABRT );
    pthread_sigmask(SIG_SETMASK, &sigs, &oldsigs);    

    /* Warten bis Thread loslaufen darf. */
    while ( ! runProcessingThread ) { usleep(1000); }
    
    /* Thread */
    while ( runProcessingThread ) { 
            /*
              Warten bis Daten in der eingangs Queue anliegen.
              Daten aus eingangs Queue austragen(Mutex gesichert).
              Daten verarbeiten(mpDNSlib).
              Daten in ausgangs Queue anhängen.
            */
    
            while ( NODE_IS_EMPTY( queueIn.first ) ) { 
                    if ( ! runProcessingThread )
                         break;
                    usleep(100);
            }
            
            if ( ! runProcessingThread )
                 break;
            
            if ( pthread_mutex_lock( &mutexIn ) != 0 ) {
                 dnsLog( "processingThread mutex lock(queueIn) error", LOG_L1 );
                 errorThread = 1;
                 break;
            }
            
            node = queueRemoveFirstElement( &queueIn );
            
            if ( pthread_mutex_unlock( &mutexIn ) != 0 ) {
                 dnsLog( "processingThread mutex unlock(queueIn) error", LOG_L1 );
                 errorThread = 1;
                 break;
            }

            if ( processDNSquery( &node->buf, &node->bufLen ) < 0 ) {
                 nodeFree( node ); /* UDP -> einfach löschen. */
            } else {
            
            
                 if ( pthread_mutex_lock( &mutexOut ) != 0 ) {
                      dnsLog( "processingThread mutex lock(queueOut) error", LOG_L1 );
                      errorThread = 1;
                      break;
                 }
                 
                 queueAppend( &queueOut, node );
                 
                 if ( pthread_mutex_unlock( &mutexOut ) != 0 ) {
                      dnsLog( "processingThread mutex unlock(queueOut) error", LOG_L1 );
                      errorThread = 1;
                      break;
                 }
            }/*else*/

    }
    
    /* Thread aufräumen. */
    dnsLog( "processingThread cleanup ok", LOG_L2 );
    pthread_exit( NULL );
    return NULL;
}

/*EOF*/
