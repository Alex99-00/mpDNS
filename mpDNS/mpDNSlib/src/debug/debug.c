/*

  Programm/Lib : mpDNSlib
  Datei        : debug.c
  Version      : v0.2r
  
  Beschreibung:
  Log/Debug Funktionalität.
  
*/


#include <stdio.h>
#include <typen.h>
#include <debug.h>


/* Funktionszeiger auf Log Funktion */
static LogFunktion logFunktion;

/* Debug/Log Level der mpDNSlib*/
static int debugLevel;

/* 
  Setzen der Log Funktion und des Log Levels.
  eDebugLevel = Log Level.
  logCallback = Log Funktion die aufgerufen werden soll.
  return = void.
*/
void setLogging( int eDebugLevel, LogFunktion logCallback ) {
     logFunktion = logCallback;
     debugLevel = eDebugLevel;
}

/* 
  In Log datei schreiben. 
  msg   = Nachricht.
  level = Level der Nachricht.
*/
void printLog( char * msg, int level ) {
    if ( logFunktion == NULL ) 
         return;

    if ( debugLevel >= level )
         logFunktion( msg, level );
}

/*
  Ausgabe eines Speicherbereichs in Hex.
  stream = Ausgabe Stream.
  buf    = Speicherbereich der Byteweise ausgegeben werden soll.
  len    = Anzahl Bytes aus buf, die ausgegeben werden sollen.
*/
void printBuf( FILE * stream, char * buf, int len ) {
    int i;
 
    for ( i = 0; i < len; i++ ) {
          fprintf( stream, "%2.2X ", (buf[i] & 0x000000ff) );
    }    
    printf("\n");
}

/*EOF*/
