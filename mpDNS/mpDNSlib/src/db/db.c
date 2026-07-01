/*

  Programm/Lib : mpDNSlib
  Datei        : db.c
  Version      : v0.1r
  
  Beschreibung:
  Wickelt die aktuelle Datenbasis ein.
  Hier mit MySQL als DNS Datenbank.
  
*/

#include <mpdnsmysql.h>

/*
  DB initialisieren.
  host   = MySQL Hostname. ! IP nehmen, "ich" bin der DNS !
  user   = Username auf MySQL Server.
  passwd = Passwort auf MySQL Server.
  db     = Datenbank Name.
  port   = Port des MySQL Server.
  return = (-1): Keine Verbindung möglich.
             0 : Verbindung hergestellt.
*/
int iniDB( char * host, char * user, char * passwd, char * db, DNS_USI port ) {
    if ( connectToMysql(host, user, passwd, db, port) < 0 ) {
         return (-1);
    }
    return 0;
}

/*
  MySQL Verbindung abbauen.
*/
void deIniDB(void) {
    disconnectFromMysql();
}

/*
  SOA RR für eine Zone abfragen.
  zone   = Zonenname.
  rr     = Ablageplatz für gefundene RRs.
  return = (-1) : Fehler.
           (>=0): Anzahl gefundener Einträge.
*/
int querySOA( char * zone, DNSrr *** rr ) {
    return queryMysqlSOA( zone, rr );
}

/*
  PTR RR für eine Adresse abfragen.
  address = IP (1.10.168.192 NICHT 192.168.10.1). 
  rr      = Ablageplatz für gefundene RRs.
  return  = (-1) : Fehler.
            (>=0): Anzahl gefundener Einträge.
*/
int queryPTR( char * address, DNSrr *** rr ) {
    return queryMysqlPTR( address, rr );
}

/*
  Entry RR(A, MX, NS, CNAME) für eine Zone abfragen.
  zone   = Zonenname.
  name   = Name des Records(Domainname).
  type   = Typ der zu suchenden RRs.
  rr     = Ablageplatz für gefundene RRs.
  return = (-1) : Fehler.
           (>=0): Anzahl gefundener Einträge.
*/
int queryEntry( char * zone, char * name, unsigned short int type, DNSrr *** rr ) {
    return queryMysqlEntry( zone, name, type, rr );
}

/*
  Alle Zonennamen die zur Verfügung stehen abfragen.
  zones  = Ablageplatz für gefundene Zonennamen.
  return = (-1) : Fehler.
           (>=0): Anzahl gefundener Einträge.
*/
int queryZoneNames( char *** zones ) {
    return queryMysqlZoneNames( zones );
}

/*EOF*/
