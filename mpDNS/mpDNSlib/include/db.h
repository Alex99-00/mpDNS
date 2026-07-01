
#ifndef _DB_H_
#define _DB_H_

#include <typen.h>
#include <dns.h>

int iniDB( char * host, char * user, char * passwd, char * db, DNS_USI port );
void deIniDB(void);

int querySOA( char * zone, DNSrr *** rr );
int queryPTR( char * address, DNSrr *** rr );
int queryEntry( char * zone, char * name, unsigned short int type, DNSrr *** rr );
int queryZoneNames( char *** zones );

#endif

/*EOF*/
