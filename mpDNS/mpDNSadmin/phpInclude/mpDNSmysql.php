<?php

/*
   mpDNSmysql.php
    - MySQL Funktionalität.
*/

#------------------------------------------------------------------------------
# MySQL
#------------------------------------------------------------------------------
$mysqlHost = '127.0.0.1';  # Hostname of MySQL server
$mysqlPort = '3306';       # Port to connect to. Empty for default(3306)
$mysqlUser = 'dns';        # Username for MySQL server
$mysqlPass = 'baja';       # Password for MySQL server
$mysqlDb   = 'mpDNS1';     # Database name on MySQL server
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Verbindung zu MySQL herstellen.
# Verbindungsdaten aus Globalen Variablen.
# return = TRUE|FALSE
#
function mysqlConnect() {
   global $mysqlHost, $mysqlPort, $mysqlUser, $mysqlPass, $mysqlDb;
   
   if ( ! (@mysql_connect( $mysqlHost.(($mysqlPort!='')?"":(":".$mysqlPort)), 
                          $mysqlUser, $mysqlPass )) )
        return FALSE;
        
   if ( ! (@mysql_select_db( $mysqlDb )) ) {
        return mysqlCreateDatabase( $mysqlDb );
   }
   
   return TRUE;
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# MySQL Verbindung abbauen.
#
function mysqlDisconnect() {
   return mysql_close();
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
function mysqlCreateDatabase( $Name ) {
   $query = "CREATE DATABASE {$Name}";
   mysql_query( $query ) or mysqlErrorDie( $query );
   
   if ( ! (@mysql_select_db( $Name )) ) {
        return FALSE;
   }
   
   $query = "
CREATE TABLE PTR (
  Id int(11) unsigned NOT NULL auto_increment,
  Address varchar(15) NOT NULL default '',
  Name varchar(255) NOT NULL default '',
  TTL int(10) NOT NULL default '3600',
  PRIMARY KEY  (Id),
  KEY Address (Address)
) TYPE=MyISAM
   ";
   mysql_query( $query ) or mysqlErrorDie( $query );

   $query = "
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
) TYPE=MyISAM
   ";
   mysql_query( $query ) or mysqlErrorDie( $query );
   
   return TRUE;
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Fehlerausgabe. --> !!! Beendigung des Scripts !!!
# $query = Anfrage die den Fehler produziert hat
#
function mysqlErrorDie( $query ) {
   echo "<br>mysql error: ".mysql_error()."<br>query: ".$query."<br>";
   die();
}
#------------------------------------------------------------------------------



#------------------------------------------------------------------------------
# Fragt die Id(In MySQL Tabelle) des SOA Eintrags für $zone ab.
# $zone = Name des SOA EIntrags
# return = Id (In MySQL Tabelle)
#
function mysqlGetIdFromZoneName( $zone ) {
   $query  = "SELECT Id FROM SOA WHERE Name = '{$zone}'";
   $sqlres = mysql_query( $query ) or mysqlErrorDie( $query );
   $row = mysql_fetch_array( $sqlres, MYSQL_ASSOC );
   return $row['Id'];
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Fragt alle in der SOA Tabelle enthaltenen Zonennamen ab.
# return = array( 0 => "abc.ax", x => ..., 5 => "xyz.net", x => ... )
#        = Oder leeres Array
#
function mysqlGetZoneNames() {
   $query  = "SELECT * FROM SOA WHERE 1 ORDER BY Name";
   $sqlres = mysql_query( $query ) or mysqlErrorDie( $query );
   
   $res['Ids'] = array();
   $res['Names'] = array();
   while ( $row = mysql_fetch_array( $sqlres, MYSQL_ASSOC ) ) {
           
#   printHtmlArray( $row );
           array_push( $res['Ids'], $row['Id'] );
           array_push( $res['Names'], $row['Name'] );
   
   }
   
   //natsort( $res );
   
   return $res;
}
#------------------------------------------------------------------------------





#------------------------------------------------------------------------------
# Gibt den SOA Eintrag für eine Zone zurück.
#
# $Id    = Id des Eintrags in SOA Tabelle.
#
# return = array( Name = > "abc.ax", Mail => "root.abc.ax", Serial => "123" 
#                 Refresh => "3600", Retry => "3600", Expire => "3600", 
#                 Minimum => "3600")
#        = Oder leeres Array
#
function mysqlGetSOA( $Id ) {
   $query  = "SELECT * FROM SOA WHERE Id = '{$Id}'";
   $sqlres = mysql_query( $query ) or mysqlErrorDie( $query );
   
   $res = array();
   if ( $row = mysql_fetch_array( $sqlres, MYSQL_ASSOC ) )
        array_push( $res, $row );
   
   reset($res);
   return $res;
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Gibt alle SOA Einträge zurück.
#
# return = array( Name = > "abc.ax", Mail => "root.abc.ax", Serial => "123" 
#                 Refresh => "3600", Retry => "3600", Expire => "3600", 
#                 Minimum => "3600")
#        = Oder leeres Array
#
function mysqlGetSOAs() {
   $query  = "SELECT * FROM SOA WHERE 1 ORDER BY Name";
   $sqlres = mysql_query( $query ) or mysqlErrorDie( $query );
   
   $res = array();
   while ( $row = mysql_fetch_array( $sqlres, MYSQL_ASSOC ) ) {
           array_push( $res, $row );
   }
   
   reset($res);
   return $res;
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Eintragen eines SOA Eintrags in die SOA Tabelle.
# $Name, $Mail, $Serial, $Refresh, $Retry, $Expire, 
# $Minimum = SOA Daten
# return = Id des eingetragenen Eintrags | FALSE
#
function mysqlInsertSOA( $Name, $Mail, $Serial, $Refresh, $Retry, $Expire, 
                         $Minimum ) {

   $zone = explode( ".", $Name );
   $zone = implode( "_", $zone );
   
   $query = "CREATE TABLE {$zone}_RR
             (Id int(11) unsigned DEFAULT '0' NOT NULL auto_increment,
              Name varchar(255) NOT NULL default '',
              Data varchar(255) NOT NULL default '',
              Type smallint(6) NOT NULL default '0',
              TTL int(11) NOT NULL default '0',
              PRIMARY KEY Id (Id),
              KEY (Name),
              KEY(Type) )";
   mysql_query( $query ) or mysqlErrorDie( $query );
         
   $query = "INSERT INTO SOA
             SET
                Name     = '{$Name}',
                Mail     = '{$Mail}',
                Serial   = '{$Serial}',
                Refresh  = '{$Refresh}',
                Retry    = '{$Retry}',
                Expire   = '{$Expire}',
                Minimum  = '{$Minimum}'
            ";
   mysql_query( $query ) or mysqlErrorDie( $query );
   
   if ( mysql_affected_rows() != 1 ) {
        $query = "DELETE TABLE {$zone}_RR";
        mysql_query( $query ) or mysqlErrorDie( $query );
        return FALSE;
   }
   
   return mysql_insert_id();
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Update eines SOA Eintrags in der SOA TAbelle.
# $Id = Id des alten Eintrags
# $Name, $Mail, $Serial, $Refresh, $Retry, $Expire, 
# $Minimum = SOA Daten
# return = TRUE
#
function mysqlUpdateSOA( $Id, $Name, $Mail, $Serial, $Refresh, $Retry, $Expire, 
                         $Minimum ) {

   $query = "UPDATE SOA
             SET
                Name     = '{$Name}',
                Mail     = '{$Mail}',
                Serial   = '{$Serial}',
                Refresh  = '{$Refresh}',
                Retry    = '{$Retry}',
                Expire   = '{$Expire}',
                Minimum  = '{$Minimum}'
             WHERE Id = '{$Id}'";
   mysql_query( $query ) or mysqlErrorDie( $query );
              
   return TRUE;
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Löschen eines SOA Eintrags. Die dazugehörige _RR Tabelle wird auch gelöscht!
# $Id = Id des zu löschenden SOA Eintrags
# return = TRUE
#
function mysqlDeleteSOA( $Id ) {
   $zone = mysqlGetSOA( $Id );
   $zone = $zone[0]['Name'];

   $zone = explode( ".", $zone );
   $zone = implode( "_", $zone );
   
   $query = "DELETE FROM SOA WHERE Id = '{$Id}'";
   mysql_query( $query ) or mysqlErrorDie( $query );
   
   $query = "DROP TABLE {$zone}_RR";
   mysql_query( $query ) or mysqlErrorDie( $query );
   
   return TRUE;
}
#------------------------------------------------------------------------------




#------------------------------------------------------------------------------
# Löschen eines PTR EIntrags aus der PTR Tabelle.
# $Id = Id des zu loschenden Eintrags
# return = TRUE
#
function mysqlDeletePtr( $Id ) {
   $query = "DELETE FROM PTR 
             WHERE Id = '{$Id}'";
   mysql_query( $query ) or mysqlErrorDie( $query );
   
   if ( mysql_affected_rows() != 1 ) 
        return FALSE;
        
   return TRUE;
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Update eines PTR Eintrags aus der PTR Tabelle.
# $Id = Id des alten Eintrags
# $Address, $Name, $TTL = PTR Daten.
# return = TRUE|FALSE
#
function mysqlUpdatePTR( $Id, $Address, $Name, $TTL  ) {
   $query = "UPDATE PTR
             SET
                Address = '{$Address}',
                Name    = '{$Name}',
                TTL     = '{$TTL}'
             WHERE Id = '{$Id}'";
   mysql_query( $query ) or mysqlErrorDie( $query );
   
   if ( mysql_affected_rows() != 1 ) 
        return FALSE;
        
   return TRUE;
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Eintargen eines PTR Eintrags in die PTR Tabelle.
# $Address = IP Adresse des Eintrags (1.10.168.192 NICHT 192.168.10.1).
# $Name = Domain Name der Adresse
# $TTL = Time To Live Wert des EIntrags
# return = TRUE|FALSE
#
function mysqlInsertPTR( $Address, $Name, $TTL ) {
   $query = "INSERT INTO PTR
             SET
                Address = '{$Address}',
                Name    = '{$Name}',
                TTL     = '{$TTL}'
            ";
   mysql_query( $query ) or mysqlErrorDie( $query );
   
   if ( mysql_affected_rows() != 1 ) 
        return FALSE;
        
   return TRUE;
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Gibt alle Einträge aus der PTR Tabelle zurück.
# return = Alle Daten aus PTR Tabelle
#
function mysqlGetPTR() {
   $query  = "SELECT * FROM PTR WHERE 1 ORDER BY Address";
   $sqlres = mysql_query( $query ) or mysqlErrorDie( $query );
   
   $res = array();
   while ( $row = mysql_fetch_array( $sqlres, MYSQL_ASSOC ) ) {
           array_push( $res, $row );
   }
   
   reset($res);
   
   return $res;
}
#------------------------------------------------------------------------------






#------------------------------------------------------------------------------
# Gibt alle Einträge einer Zone zurück.
#
# $zoneName = Zonenangabe im Format "abc.ax" sein.
#
# return = array( array(Name => "abc.ax", Data => "192.168.5.4", Type => "A", 
#                       TTL => "3600"), ..., array(...) )
#        = Oder leeres Array
#
function mysqlGetRRs( $zone ) {
   $temp = explode( ".", $zone );
   array_push( $temp, "RR" );
   $tabname = implode( "_", $temp );
   
   $query  = "SELECT * FROM {$tabname} WHERE 1 ORDER BY Type, Name, Data";
   $sqlres = mysql_query( $query ) or mysqlErrorDie( $query );
   
   $res = array();
   while ( $row = mysql_fetch_array( $sqlres, MYSQL_ASSOC ) ) {
           array_push( $res, $row );
   }
   
   reset($res);
   return $res;
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Eintragen eines RR Eintrags in die entsprechende _RR Tabelle.
# $zone = Zone des Eintrags
# $Name = Domain Name des EIntrags
# $Data = Daten des Eintrags, je nach Typ
#         -> 192.168.10.1(A), 10 nathan.ax(MX), xx.nathan.ax(CNAME), 
#            nathan.ax(NS)
# return = TRUE|FALSE
#
function mysqlInsertRR( $zone, $Name, $Data, $Type, $TTL ) {
   $zone = explode( ".", $zone );
   $zone = implode( "_", $zone );
   
   $query = "INSERT INTO {$zone}_RR
             SET
                Name = '{$Name}',
                Data = '{$Data}',
                Type = '{$Type}',
                TTL  = '{$TTL}'
            ";
   mysql_query( $query ) or mysqlErrorDie( $query );
   
   if ( mysql_affected_rows() != 1 ) 
        return FALSE;
        
   return TRUE;
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Löschen eines RR Eintrags aus einer Zone.
# $zone = Zone in der der RR eingetragen ist
# $Id = Id des zu löschenden RR Eintrags
#
function mysqlDeleteRR( $zone, $Id ) {
   $zone = explode( ".", $zone );
   $zone = implode( "_", $zone );
   
   $query = "DELETE FROM {$zone}_RR
             WHERE Id = '{$Id}'";
   mysql_query( $query ) or mysqlErrorDie( $query );
   
   if ( mysql_affected_rows() != 1 ) 
        return FALSE;
        
   return TRUE;
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Update eines RR Eintrags in der entsprechenden _RR Tabelle.
# $zone = Zone in der der RR eingetragen ist
# $Id = Id des alten Eintrags
# $Name, $Data, $Type, $TTL = RR Daten
#
function mysqlUpdateRR( $zone, $Id, $Name, $Data, $Type, $TTL  ) {
   $zone = explode( ".", $zone );
   $zone = implode( "_", $zone );
   
   $query = "UPDATE {$zone}_RR
             SET
                Name    = '{$Name}',
                Data    = '{$Data}',
                Type    = '{$Type}',
                TTL     = '{$TTL}'
             WHERE Id = '{$Id}'";
   mysql_query( $query ) or mysqlErrorDie( $query );
   
   if ( mysql_affected_rows() != 1 ) 
        return FALSE;
        
   return TRUE;
}
#------------------------------------------------------------------------------

#EOF
?>
