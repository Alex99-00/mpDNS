
#ifndef _CONFIGPARSER_H_
#define _CONFIGPARSER_H_

#define MAXCONFIGLINELENGTH 1024


/* Typ/Struktur für key,value Wertepaar */
typedef struct {
        char * key;
        char * value;
}ConfigNode;


/* Typ/Struktur für key,value Wertepaare Tabelle */
typedef struct {
        ConfigNode * data; /* Tabelle */
        int          len;  /* Tabellenlänge */
}Config;


Config * readConfig( char * path, FILE * logFile );
Config * freeConfig( Config * config );
void printConfig( Config * config, FILE * outFile );

#endif

/*EOF*/
