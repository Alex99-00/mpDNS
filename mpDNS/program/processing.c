/*

  Programm/Lib : mpDNS
  Datei        : processing.c
  Version      : v0.2r
  
  Beschreibung:
  Initialisierung Posix Threads.
  Enthält auch die orginal Variablen der Threads und Mutexes!
  
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

#include <processing.h>
#include <queue.h>
#include <udp.h>
#include <alloc.h>
#include <inThread.h>
#include <outThread.h>
#include <processingThread.h>


/*---------------------------------------------------------------------------*/

/* Mutex der In/Out-Queue und der Log Datei. */
pthread_mutex_t mutexIn      = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexOut     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexLogFile = PTHREAD_MUTEX_INITIALIZER;

/* Threads. */
pthread_t inThread, outThread, processingThread;

/* Start/Stop Variablen der Threads. */
int runInThread, runOutThread, runProcessingThread;

/* Fehlervariable */
int errorThread;

/* Queue In/Out */
Queue queueIn, queueOut;

/* Port/IP des Servers. */
unsigned short int dnsPort;
char * dnsIP;

/* Socket zum Senden/Empfangen */
int ssocket;
/*---------------------------------------------------------------------------*/

/* 
  Erzeugen der Threads.
  port   = DNS Port.
  ip     = DNS IP.
  return = (-1): Fehler bei Thread Erzeugung.
             0 : Threads erzeugt.
*/
int createThreads( unsigned short int port, char * ip ) {
    assert( ip != NULL );

    dnsPort = port;
    dnsIP = MALLOC( strlen(ip)+1 );
    memmove( dnsIP, ip, strlen(ip)+1 );

    /* Fehler in einem Thread auf NEIN. */
    errorThread = 0;

    /* Start/Stop Variablen auf Stop. */
    runInThread         = 0;
    runOutThread        = 0; 
    runProcessingThread = 0;

    /* In/Out-Queue initialisieren. */
    queueInit( &queueIn );
    queueInit( &queueOut );
    
    /* Gibt immer null zurück!?! */
    pthread_mutex_init( &mutexIn, NULL );
    pthread_mutex_init( &mutexOut, NULL );
    pthread_mutex_init( &mutexLogFile, NULL );

    
    /* 
      Gibt im Fehlerfall EAGAIN zurück.
      (returns EAGAIN on error, no resourcen available or PTHREAD_THREADS_MAX 
       are reached.)
    */
    if ( pthread_create( &processingThread, NULL, processingThreadStart, NULL ) < 0 ) {
         return (-1);
    }
    
    if ( pthread_create( &inThread, NULL, inThreadStart, NULL ) < 0 ) {
         return (-1);
    }
    
    if ( pthread_create( &outThread, NULL, outThreadStart, NULL ) < 0 ) {
         return (-1);
    }

    /* UDP Socket erstellen. */
    if ( (ssocket = getUdpServerSocket( dnsPort, dnsIP )) < 0 ) {
         return (-1);
    }
    
    return 0;
}

/* 
  Auf Thread Beendigung warten und Log Datei Mutex löschen.
*/
void waitForThreads() {
    Node * node;
    
    pthread_join( inThread, NULL );
    pthread_join( outThread, NULL );
    pthread_join( processingThread, NULL );
    
    /*
      Gibt EBUSY zurück, wenn eine Mutex noch gesperrt ist. Aber die Threads
      laufen nur aus wenn alle nicht gesperrt sind.
    */
    
    /* In inThread/outThread
    pthread_mutex_destroy( &mutexIn );
    pthread_mutex_destroy( &mutexOut );  */
    
    pthread_mutex_destroy( &mutexLogFile );

    while ( queueCountElements( &queueOut ) > 0 ) {
            node = queueRemoveFirstElement( &queueOut );
            nodeFree( node );
    }

    while ( queueCountElements( &queueIn ) > 0 ) {
            node = queueRemoveFirstElement( &queueIn );
            nodeFree( node );
    }
    
    FREE( dnsIP );
    closeUdpSocket( ssocket );
}

/*
  Threads starten.
*/
void runThreads() {
    runInThread         = 1;
    runOutThread        = 1; 
    runProcessingThread = 1;
}

/*
  Threads stopen.
*/
void stopThreads() {    
    runInThread         = 0;
    runOutThread        = 0; 
    runProcessingThread = 0;
}

/*EOF*/
