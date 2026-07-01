#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include <alloc.h>
#include <error.h>
#include <debug.h>
#include <configparser.h>


/*
  Parsen einer eingelesenen Zeile.
  lineBuf = Zeile.
  config  = Config Struktur, zum ablegen der Daten.
  cpos    = Position in Config Struktur.
  col     = Position in Zeile.
*/
int parseConfigLine( char * lineBuf, Config * config, int cpos, int * col ) {
    char keyBuf[MAXCONFIGLINELENGTH];
    char valueBuf[MAXCONFIGLINELENGTH];
    int lineLen, i, state;
    
    assert( (lineBuf != NULL) && (config != NULL) );
    
    lineLen = strlen(lineBuf);    

    /* Leere Zeile */
    if ( lineLen == 1 ) 
         return 0; /* Eine Zeile verarbeitet aber kein key,value Wertepaar */
    
    /* Kommentar Zeile */
    if ( lineBuf[0] == (*("#")) ) {
         return 0; /* Eine Zeile verarbeitet aber kein key,value Wertepaar */
    }
    
    /* Key */
    if (sscanf( lineBuf, "%[^ \t=]", keyBuf ) != 1 ) {
        *col = 0;
        return (-1); /* Fehlerhafte Zeile verarbeitet */
    }
    
    /* " = " */
    i = strlen(keyBuf);
    state = 0;
    while ( i < lineLen ) {
            
            /*printf( ":%s", &lineBuf[i] );*/
            
            if ( (lineBuf[i] == (*(" "))) || (lineBuf[i] == (*("="))) || 
                 (lineBuf[i] == '\t') ) {

                  if ( lineBuf[i] == (*("=")) )
                       state = 1;

                  i++;

            } else {

                  if ( state == 0 ) {                       
                       *col = i;
                       return (-1); /* Fehlerhafte Zeile verarbeitet */
                  } else {
                       break;
                  }

            }
    }
    
    /* "Value" */
    if (sscanf( &lineBuf[i], "\"%[^\"]\"\n", valueBuf ) != 1 ) {
        *col = i - 1;/* wenn nicht, wieder einen zurück, auf letzten gültigen */
        return (-1);
    }
    
    /* In Config Struktur Speicher anlegen */
    config->data[cpos].key   = MALLOC( (strlen(keyBuf)+1) * sizeof(char) );
    if ( config->data[cpos].key == NULL ) {
         return (-2);
    }
    
    config->data[cpos].value = MALLOC( (strlen(valueBuf)+1) * sizeof(char) );
    if ( config->data[cpos].value == NULL ) {
         return (-2);
    }
    
    /* Werte aus Zwischenspeicher umkopieren */
    memmove( config->data[cpos].key, keyBuf, strlen(keyBuf)+1 );
    memmove( config->data[cpos].value, valueBuf, strlen(valueBuf)+1 );
    
    return 1; /* Eine Zeile verarbeitet */
}


/*
  Einlesen der Config Datei und ablegen der gefundenen Werte in Config 
  Struktur.
  path    = path/name der Config Datei.
  logFile = Dateizeiger der Log Datei.
  return  = Config Struktur oder NULL bei Fehler.
*/
Config * readConfig( char * path, FILE * logFile ) {
    Config * config;
    FILE * configFile;
    char lineBuf[MAXCONFIGLINELENGTH];
    int cpos, cmax, row=0, col=0;
    
    assert( path != NULL );
    
    /* Config Datei öffnen */
    if ( (configFile = fopen( path, "rb" )) == NULL ) {
         lastError = 3;
         return NULL;
    }
    
    
    cpos = 0;  /* Position in config->data Tabelle */
        
    /* Speicher für Config Struktur holen */
    config       = MALLOC( sizeof(Config) );
    if ( config == NULL ) {
         return (NULL);
    }
    
    /* Speicher für config->data Tabelle holen */
    config->data = MALLOC( sizeof(ConfigNode) * 16 );
    if ( config->data == NULL ) {
         return (NULL);
    }
    
    cmax = 16; /* Max. Plätze in config->data ...momentan */
    
    /* Zeilenweise einlesen */
    while ( fgets( lineBuf, MAXCONFIGLINELENGTH, configFile ) != NULL ) {
            
            /*printf( "%s", lineBuf );*/
            
            /* Zeile parsen */
            switch ( parseConfigLine( lineBuf, config, cpos, &col ) ) {
            
                     case (0): /* Zeile ohne key,value Wertepaar aber Syntax ok */
                           row++;
                           continue;
                     break;
                     
                     case (1): /* Zeile mit key,value Wertepaar, Syntax ok */
                           row++;
                           cpos++;                           
                           /* Speicher für config->data Tabelle holen, falls nötig */
                           if ( cpos == cmax ) {
                                cmax += 16;
                                config->data = REALLOC( config->data, 
                                                sizeof(ConfigNode) * cmax );
                                if ( config->data == NULL ) {
                                     freeConfig( config );
                                     return (NULL);
                                }
                           }
                     break;
                     
                     case (-1): /* Fehlerhafte Zeile */
                           fprintf( logFile, "Config file error on Row: %d Col: %d\n", row, col );
                           freeConfig( config );
                           fclose( configFile );
                           lastError = 2;
                           return NULL;
                     break;
                     
                     case (-2): /* Kein Speicher mehr frei */
                           /*fprintf( logFile, "Config file error on Row: %d Col: %d\n", row, col );*/
                           freeConfig( config );
                           fclose( configFile );                           
                           return NULL;
                     break;
                     
            }

    }
    
    /* Endgröße für config-data Tabelle erzeugen */
    config->data = REALLOC( config->data, sizeof(ConfigNode) * cpos );
    if ( config->data == NULL ) {
         return (NULL);
    }
    
    /* Endgröße für config-data Tabelle eintragen */
    config->len = cpos;
    
    fclose( configFile );
    
    return config; /* Config Struktur zurückgeben */
}

Config * freeConfig( Config * config ) {
    int i;
    
    assert( config != NULL );
    
    /* Alle Plätze in config->data Tabelle durchgehen und die key,value Paare freigeben */
    for ( i = 0; i < config->len; i++ ) {
          FREE( config->data[i].key );
          FREE( config->data[i].value );
    }
    
    FREE( config->data ); /* config->data Tabelle freigeben */
    FREE( config );       /* Config Struktur freigeben */
        
    return NULL;          /* Zum beschreiben der freigegebenen Config Struktur */
}


void printConfig( Config * config, FILE * outFile ) {
        
    assert( config != NULL );
    
    /* Anzahl Wertepaare ausgeben */
    fprintf( outFile, "\nFound %d key,value pairs\n", config->len );
    
    /* Wertepaare ausgeben */
    {
     int i;
     for ( i = 0; i < config->len; i++ ) {
           fprintf( outFile, "%s = \"%s\"\n", config->data[i].key, config->data[i].value );
     }
    }
}

/*EOF*/
