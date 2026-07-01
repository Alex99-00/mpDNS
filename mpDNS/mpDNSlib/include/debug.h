
#ifndef _DEBUG_H_
#define _DEBUG_H_

enum logLevel {
     LOG_NO = 0, /* kein logging */
     LOG_L1 = 1, /* small  */
     LOG_L2 = 2, /* medium */
     LOG_L3 = 3  /* big    */
};

typedef void (*LogFunktion)(char *, int);

void setLogging( int eDebugLevel, LogFunktion logCallback );
void printBuf( FILE * stream, char * buf, int len );
void printLog( char * msg, int level );

#endif

/*EOF*/
