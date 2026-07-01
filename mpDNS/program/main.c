/*

  Programm/Lib : mpDNS
  Datei        : main.c
  Version      : v0.6r
  
  Beschreibung:
  Hauptmodul des Programms mpDNS.
  
*/

#include <stdio.h>
#include <stdlib.h>
#define __USE_BSD  /* usleep */
#include <unistd.h>
#undef __USE_BSD
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include <sys/types.h> /* umask */
#include <sys/stat.h>  /* umask */

#ifndef NDEBUG
#include <mcheck.h>
#endif


#include <processing.h>
#include <error.h>
#include <dns.h>
#include <configparser.h>


/* Mutex für logFile */
extern pthread_mutex_t mutexLogFile;

/* Start/Stop Variablen der Thraeds */
extern int runInThread, runOutThread, runProcessingThread;
extern int errorThread;

/* Datei für Protokollierung  */
FILE * logFile= NULL;

/* 
  Semaphore für die Beendigung von main(). 
  Wird durch Signalhandler gesetzt. 
*/
sem_t terminateMain;

/* 
  Lokale Configurations Struktur. 
  mpDNSlib enthält einen rudimentären Configurations Datei Parser  .
*/
typedef struct {
        char               * host;
        char               * user;
        char               * passwd;
        char               * dbname;
        unsigned short int   port;
        int                  debugLevel;
        char               * logFile;
        unsigned short int   dnsPort;
        char               * dnsIP;
        char               * forwarder;
}LConfig;

/* Defult pfad/name der Configurationsdatei. */
char * defaultConfigFileName = "/etc/mpDNS.cfg";



/* 
  Callback-Funktion für die mpDNSlib um in die logDatei zu schreiben.
  Kann auch lokal verwendet werden, da durch Mutex geschützt.
  msg    = Zu schreibende Nachricht.
  level  = Nummer des Loglevels.
  return = void.
*/
void dnsLog( char * msg, int level ) {
    static struct tm *tm;
    static time_t akttime;
    static char * p;
    
    if ( logFile == NULL )
         return;
    
    if ( pthread_mutex_lock( &mutexLogFile ) < 0 ) {
         return ;
    }

    akttime = time( NULL );
    tm = localtime( &akttime );
    p = asctime(tm);
    p[strlen(p)-1]='\0'; /* delete "\n" */
    
    fprintf( logFile, "%s: Log level %d: %s\n", p, level, msg ); fflush(logFile);

    if ( pthread_mutex_unlock( &mutexLogFile ) < 0 ) {
         return ;
    }    

}

/*
  Ließt die Config Datei ein(Benutzt die mpDNSlib Funktionalität).
  Wird später im programm weiterverarbeitet.
  filename = pfad/name der einzulesenden Datei.
  return   = Struktur mit gefundenen Werten.
*/
Config * getConfig( char * filename ) {
    Config * c;
    
    c = readConfig( filename, stderr );
    
    if ( c == NULL ) {
         fprintf( stderr, "Kann Config Datei \"%s\" nicht lesen.\n", lastErrorMsg() );
    }
    
    return c;/*NULL on error*/
}

/*
  Baut aus der eingelesenen Config Struktur die lokale Struktur.
  Mit Fehlerprüfung.
  lconfig = Lokale Configurations Struktur.
  config  = Eingelesene Configurations Struktur.
  return  = (-1): Fehler/Nicht alle benötigten Werte vorhanden.
              0 : Alles vorhanden.
*/
int buildLocalConfig( LConfig * lconfig, Config * config ) {
    int i=0, found=0;
    
    lconfig->host       = NULL;
    lconfig->user       = NULL;
    lconfig->passwd     = NULL;
    lconfig->dbname     = NULL;    
    lconfig->port       = 0;
    lconfig->debugLevel = 0;    
    lconfig->logFile    = NULL;
    lconfig->forwarder  = NULL;
    
    while ( i < config->len ) {
    
            if ( strcmp( config->data[i].key, "host" ) == 0 ) {
                 lconfig->host = config->data[i++].value;
                 found++;
                 continue;
            }

            if ( strcmp( config->data[i].key, "user" ) == 0 ) {
                 lconfig->user = config->data[i++].value;
                 found++;
                 continue;
            }

            if ( strcmp( config->data[i].key, "passwd" ) == 0 ) {
                 lconfig->passwd = config->data[i++].value;
                 found++;
                 continue;
            }

            if ( strcmp( config->data[i].key, "port" ) == 0 ) {
                 lconfig->port = (unsigned short int)atoi(config->data[i++].value);
                 found++;
                 continue;
            }
            
            if ( strcmp( config->data[i].key, "dbname" ) == 0 ) {
                 lconfig->dbname = config->data[i++].value;
                 found++;
                 continue;
            }

            if ( strcmp( config->data[i].key, "debugLevel" ) == 0 ) {
                 lconfig->debugLevel = (int)atoi(config->data[i++].value);
                 found++;
                 continue;
            }

            if ( strcmp( config->data[i].key, "logFile" ) == 0 ) {
                 lconfig->logFile = config->data[i++].value;
                 found++;
                 continue;
            }

            if ( strcmp( config->data[i].key, "dnsport" ) == 0 ) {
                 lconfig->dnsPort = (unsigned short int)atoi(config->data[i++].value);
                 found++;
                 continue;
            }

            if ( strcmp( config->data[i].key, "dnsip" ) == 0 ) {
                 lconfig->dnsIP = config->data[i++].value;
                 found++;
                 continue;
            }
            
            if ( strcmp( config->data[i].key, "forwarder" ) == 0 ) {
                 lconfig->forwarder = config->data[i++].value;
                 /*found++;*/ /* ist KEIN muß */
                 continue;
            }

            i++;
    }
    
    if ( found != 9/*alle außer forwarder*/ )
         return (-1);
         
    return 0;
}


/*
  Signalhandler. Zur sauberen Beendigung des Programms.
  sigNr  = Nummer des Signals.
  return = void.
*/
void exitProgram(int sigNr) {
    struct tm *tm;
    time_t akttime;
    char * p;

    /* Kein dnsLog im Signal Handler verwenden, weil mutex:lock/unlock so nicht sicher ist...deadlock */
    akttime = time( NULL );
    tm = localtime( &akttime );
    p = asctime(tm);
    p[strlen(p)-1]='\0'; /* delete "\n" */
    
    fprintf( logFile, "%s: Log level %d: %s %d\n", p, LOG_L1, "exitProgram", sigNr ); fflush(logFile);

    stopThreads();
    fprintf( logFile, "%s: Log level %d: %s\n", p, LOG_L1, "exitProgram stopThreads ok" ); fflush(logFile);

    waitForThreads();
    fprintf( logFile, "%s: Log level %d: %s\n", p, LOG_L1, "exitProgram waitForThreads ok" ); fflush(logFile);

    deIniDNSlib();
    fprintf( logFile, "%s: Log level %d: %s\n", p, LOG_L1, "exitProgram deIniDNSlib ok" ); fflush(logFile);
    
    if ( logFile != NULL )
        fclose( logFile );
        
    /* main() darf nun zuende laufen. */
    sem_post( &terminateMain );
}

/*
  Installiert Signalhandler für SIGINT, SIGTERM, SIGABRT, SIGHUP.
  Alle beenden das Programm über exitProgram().
  return = (-1): Ein Signalhandler konnte nicht eingerichtet werden.
             0 : Alle Signalhandler eingerichtet.
*/
int setSignalHandler(void) {

    if ( signal(SIGINT, exitProgram) == SIG_ERR ) {
         fprintf( stderr, "Signal Handler SIGINT konnten nicht eingerichtet werden\n" );
         return (-1);
    }

    if ( signal(SIGTERM, exitProgram) == SIG_ERR ) {
         fprintf( stderr, "Signal Handler SIGTERM konnten nicht eingerichtet werden\n" );
         return (-1);
    }

    if ( signal(SIGABRT, exitProgram) == SIG_ERR ) {
         fprintf( stderr, "Signal Handler SIGABRT konnten nicht eingerichtet werden\n" );
         return (-1);
    }
    
    if ( signal(SIGHUP, exitProgram) == SIG_ERR ) {
         fprintf( stderr, "Signal Handler SIGHUP konnten nicht eingerichtet werden\n" );
         return (-1);
    }
    
    return 0;

}

/*
  Prozess demonisieren.
  return (-1): Fehler bei fork() Aufruf.
           0 : Demonisierung erfolgreich.
*/
int makeDeamon(void) {

    switch ( fork() ) {
       case (-1):
              fprintf( stderr, "Demonisierung über fork() fehlgeschlagen.\n" );
              perror("");
              return (-1);
       break;
       
       case 0:
             setsid();   /* Neue Session */
             chdir("/"); /* Wechsel ins root Verzeichnis */
             umask(0);   /* Dateikreirungsmaske auf 0 setzen */
             /* Terminal Verbindungen schließen */
             fclose(stdout);
             fclose(stderr);
             fclose(stdin);      
             return 0;
       break;
       
       default:
              exit(0); /* Elternprozess beenden !!! */
       break;
    
    }
    
    return 0;
}

/*
  Ausgabe der Hilfe auf stdout.
  return = void.
*/
void printHelp(void) {
    printf("\n");
    printf(" mpDNS [-h|--help] [filename]\n\n");
    printf(" -h|--help\tPrint this help\n");
    printf(" filename\tPath/Filename to configuration file, or empty for default(\"/etc/mpDNS.cfg\")\n");
    printf("\n");
}

/*
  Einstiegspunkt des Programms.
  argc = Anzahl Parameter.
  argv = Parameter.
*/
int main(int argc, char **argv) {
    Config * config;
    LConfig lconfig;
    char * configFileName; /* pfad/name der Configurationsdatei */

/* Wenn "Debug-Build" dann mtrace() zum auffinden von Speicherleks benutzen. */
#ifndef NDEBUG    
    mtrace(); /* set MALLOC_TRACE env. var. to a "path/file"
                 bash: MALLOC_TRACE=/home/xx/c/MALLOC_TRACE/mem.trace
                 check with "mtrace path/filename" */
#endif

    /* Nur Programmname und ein Parameter(pfad/name der "ConfigDatei") erlauben. */
    if ( argc > 2 ) {
         fprintf( stderr, "Zu viele Parameter.\n" );
         printHelp();
         return 1;
    }
    
    /* Hilfe gewünscht. */
    if ( (argc > 1) &&
         ((strcmp(argv[1],"-h") == 0) || (strcmp(argv[1],"--help") == 0)) ) {
         printHelp();
         return 1;
    }

    /* Signalhandler einrichten. */
    if ( setSignalHandler() < 0 ) {
         fprintf( stderr, "Signal Handler konnten nicht eingerichtet werden\n" );
         return 1;
    }

    /* Default pfad/name oder angegeben. */
    if ( argc == 2 ) {
         configFileName=argv[1];
    } else {
         configFileName=defaultConfigFileName;
    }
    
    /* Einlesen/Parsen der Datei. */
    if ( (config = getConfig( configFileName )) == NULL ) {
         return 1;
    }    

    /* Aus eingelesenen daten die lokale Config bauen. */
    if ( buildLocalConfig( &lconfig, config ) < 0 ) {
         fprintf( stderr, "Configurationsdatei enthält nicht alle benötigten Werte\n" );
         freeConfig( config );
         return 1;
    }

    /* Alles erfüllt um zu starten. */
    
    /* Protokollierungs datei öffnen/anlegen */
    if ( (logFile = fopen( lconfig.logFile, "ab+" )) == NULL ) {
         fprintf( stderr, "Log File \"%s\" konnte nicht geöffnet werden\n", 
                  (lconfig.logFile==NULL)?"":lconfig.logFile );
    }

    /* Prozess Demonisieren. */
    if ( makeDeamon() != 0 ) {
         fclose(logFile);
         freeConfig( config );
         return 1;
    }

    
    /* mpDNSlib initialisieren. */
    if ( iniDNSlib( lconfig.host, lconfig.user, lconfig.passwd, lconfig.dbname, 
                    lconfig.port, lconfig.debugLevel, dnsLog, lconfig.forwarder) < 0 ) {
         fprintf(stderr, "%s\n", lastErrorMsg() );
         freeConfig( config );
         return 1;
    }

    /* 
      Semaphore initialisieren. 
      main() wartet nach dem starten der Threads darauf, das der Signalhandler diese
      auf eins setzt.
    */
    if ( sem_init( &terminateMain, 0/* nur dieser prozess*/, 0/*nicht null um main zu beenden*/ ) != 0 ) {
         dnsLog("Fehler bei Semaphoren Initialisierung", LOG_L1);
         deIniDNSlib();
         freeConfig( config );
         return 1;
    }
    
    /* Threads erzeugen. */
    if ( createThreads( lconfig.dnsPort, lconfig.dnsIP ) < 0 ) {
         dnsLog("Fehler bei Thread Initialisierung", LOG_L1);
         deIniDNSlib();
         freeConfig( config );
         return 1;
    }
    
    /* Configuration nicht mehr benötigt, freigeben. */
    freeConfig( config );
    
    /* Threads starten. */
    runThreads( );
    
    /* 
      Warten bis main() sich beenden darf. 
      Signalhandler setzt die Semaphoren variable auf (!= 0)
      -> use (SIGTERM || SIGINT || SIGABRT || SIGHUP)
    */
    
    {
     int ret;
     while ( ( ret = sem_trywait( &terminateMain )) == (-1) ) {
             sleep(1);
             if ( (errorThread == 1) ) {
                   exitProgram(SIGINT);
                   return 1;
             }
     }
    }
    /* sem_wait( &terminateMain ); */

    return 0; /* ...nice... */
}

/*EOF*/
