/*

  Programm/Lib : mpDNSlib
  Datei        : error.c
  Version      : v0.2r
  
  Beschreibung:
  Enthält alle Fehlermeldungen. Sowie die Variable die die letzte Fehlernummer
  enthält und eine Funktion die die letzte Fehlermeldung zurückgibt.
  
*/

/* Enthält letzte Fehlernummer. */
int lastError = 0;

/* Meldung zur Nummer. */
char *errorMsg[] = 
{
"",                    /* 0 */
"Out of memory",       /* 1 */

"Config file error",   /* 2 */

"Can´t open file",     /* 3 */

"Can´t connect to SQL Server", /* 4 */
"SQL query error",     /* 5 */
"SQL result error",    /* 6 */

"DNS Header too short", /* 7 */
"DNS Message too short" /* 8 */

};

#include <error.h>

/*
  Gibt zeiger auf letzte Fehlermeldung zurück. NICHT freigeben, ist statisch.
*/
char * lastErrorMsg(void) {
    return errorMsg[lastError];
}

/*EOF*/
