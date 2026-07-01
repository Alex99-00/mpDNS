/*

  Tabellen Struktur

#
# Table structure for table 'PTR'
#
CREATE TABLE PTR (
  Id int(11) unsigned NOT NULL auto_increment,
  Address varchar(15) NOT NULL default '',
  Name varchar(255) NOT NULL default '',
  TTL int(10) NOT NULL default '3600',
  PRIMARY KEY  (Id),
  KEY Address (Address)
) TYPE=MyISAM;

#
# Table structure for table 'SOA'
#
CREATE TABLE SOA (
  Id int(11) unsigned NOT NULL auto_increment,
  Name varchar(255) NOT NULL default '',
  Mail varchar(255) NOT NULL default '',
  Serial int(10) unsigned NOT NULL default '0',
  Refresh int(11) NOT NULL default '0',
  Retry int(11) NOT NULL default '0',
  Expire int(11) NOT NULL default '0',
  Minimum int(11) NOT NULL default '0',
  PRIMARY KEY  (Id),
  KEY Name (Name)
) TYPE=MyISAM;

#
# Table structure for table 'nathan_ax_RR'
#
CREATE TABLE nathan_ax_RR (
  Id int(11) unsigned NOT NULL auto_increment,
  Name varchar(255) NOT NULL default '',
  Data varchar(255) NOT NULL default '',
  Type smallint(6) NOT NULL default '0',
  TTL int(11) NOT NULL default '0',
  PRIMARY KEY  (Id),
  KEY Name (Name),
  KEY Type (Type)
) TYPE=MyISAM; 
*/

#ifndef _MYSQL_H_
#define _MYSQL_H_

#if (defined(_WIN32))
#include <windows.h>
#endif

#include <dns.h> /* DNSrr */


int queryMysqlSOA( char * zone, DNSrr *** rr );
int queryMysqlPTR( char * address, DNSrr *** rr );
int queryMysqlEntry( char * zone, char * name, unsigned short int type, DNSrr *** rr );
int queryMysqlZoneNames( char *** zones );

int connectToMysql( char * host, char * user, char * pass, char * db, unsigned int port );
void disconnectFromMysql();

#endif

/*EOF*/
