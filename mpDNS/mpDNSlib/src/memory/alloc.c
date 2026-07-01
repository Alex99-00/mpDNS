/*

  Programm/Lib : mpDNSlib
  Datei        : memory.c
  Version      : v0.1r
  
  Beschreibung:
  Speicherverwaltung.
  Setzt entsprechende Fehlermeldungen und testet NULL Zeiger ab.
*/


#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <debug.h>

/*
  malloc() equivalent.
  Auﬂer das Fehlernummer gesetzt wird.
  size   = Anzahl Bytes die zu besorgen sind.
  return = Zeiger auf Speicherplatz.
*/
void * MALLOC (size_t size) {

  void *result = malloc (size);

  if (result == NULL) {
      lastError = 1;      
      printLog( "malloc error", LOG_L1 );
      return NULL;
  }

  return result;
  
}

/*
  realloc() equivalent.
  Auﬂer das Fehlernummer gesetzt wird.
  ptr    = Alter Speicherplatz.
  size   = Anzahl Bytes die zu besorgen sind.
  return = Zeiger auf Speicherplatz.
*/
void * REALLOC (void *ptr, size_t size) {

  void *result = realloc (ptr, size);
  
  if (result == NULL) {
      lastError = 1;
      printLog( "realloc error", LOG_L1 );
      return NULL;
  }

  return result;
}


/*
  free() equivalent.
  Auﬂer das Fehlernummer gesetzt wird.
  ptr    = Freizugebender Speicher.
  return = NULL.
*/
void * FREE (void *ptr) {

  if ( ptr != NULL ) {
       free(ptr);
  }
  
  return NULL;
  
}


/*EOF*/
