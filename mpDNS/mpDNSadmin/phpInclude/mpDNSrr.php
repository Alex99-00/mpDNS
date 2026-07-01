<?php

/*
   mpDNSrr.php
    - Verwaltung der RR´s (Resourcen Records)    
*/


# DNS Typen/Namen umsetzung

# Name nach Nummer
$DNS_TYPE_NUM = array(
'A'     => 1,
'NS'    => 2,
'CNAME' => 5,
'SOA'   => 6,
'PTR'   => 12,
'MX'    => 15
);

# Nummer nach Name
$DNS_TYPE_NAME = array(
1  => 'A',
2  => 'NS',
5  => 'CNAME',
6  => 'SOA',
12 => 'PTR',
15 => 'MX'
);

#------------------------------------------------------------------------------
function newRR( $zone, $Id, $Name, $Data, $Type, $TTL ) {
   global $DNS_TYPE_NAME;
   
   # SOA und PTR haben seperate Eingabemasken   
   $dnsTypes = array();
   while ( list($key,$val) = each($DNS_TYPE_NAME) ) {
           if ( ($val != SOA) && ($val != PTR) )
                $dnsType[$key] = $val;
   }
   
   # $Id leer ist neueintrag, sonst Update mit übergebenen Werten.
   if ( $Id == '' ) {
        $cmd = "insertRR";
   } else {
        $cmd = "updateRR";
   }
        
   htmlHeadline( $zone );
      
   htmlTabLayout2o();
   echo ""
   ."<tr><td>"
   .htmlFormO()
   .htmlHiddenInput("cmd",$cmd)
   .htmlHiddenInput("zone",$zone)
   .htmlHiddenInput("Id",$Id)   
   ."<table align=\"center\">"   
   
   ."<tr><td>Name</td><td>&nbsp;</td><td>".htmlInputText("Name", $Name, 64, 255, 0)."</td></tr>"
   ."<tr><td>Data</td><td>&nbsp;</td><td>".htmlInputText("Data", $Data, 64, 255, 0)."</td></tr>"
   ."<tr><td>Type</td><td>&nbsp;</td><td>".htmlSelect( $dnsType, "Type", 1, $Type)."</td></tr>"
   ."<tr><td>TTL</td><td>&nbsp;</td><td>".htmlInputText("TTL", $TTL, 11, 10, 0)."</td></tr>"
   ."<td align=\"left\" colspan=\"3\">".htmlSubmitButton("OK")."</td></tr>"
   ."</table>"
   .htmlFormC()
   ."</td></tr>\n";
   htmlTabLayout2c();

}
#------------------------------------------------------------------------------



#------------------------------------------------------------------------------
# Eintargen eines RR Eintrags in die entsprechende _RR Tabelle.
# Gibt FALSE zurück wenn der RR Eintrag fehlerhafte Werte/Syntax enthält.
# $zone = Zone des Eintrags
# $Name = Domain Name des EIntrags
# $Data = Daten des Eintrags, je nach Typ
#         -> 192.168.10.1(A), 10 nathan.ax(MX), xx.nathan.ax(CNAME), 
#            nathan.ax(NS)
# return = SOA Id | FALSE
#
function insertRR( $zone, $Name, $Data, $Type, $TTL ) {

   if ( ! checkRR( $Name, $Data, $Type, $TTL ) ) {
        return FALSE;
   }
   
   mysqlInsertRR( $zone, $Name, $Data, $Type, $TTL );
   return mysqlGetIdFromZoneName( $zone );
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Löschen eines RR Eintrags aus der entsprechenden Tabelle.
# $zone = Zone des Eintrags.
# $Id = Id des Eintrags
# return = SOA Id
#
function deleteRR( $zone, $Id ) {
   mysqlDeleteRR( $zone, $Id );
   return mysqlGetIdFromZoneName( $zone );
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Update eines RR Eintrags in der entsprechenden _RR Tabelle.
# Gibt FALSE zurück wenn der RR Eintrag fehlerhafte Werte/Syntax enthält.
# $zone = Zone in der der RR eingetragen ist
# $Id = Id des alten Eintrags
# $Name, $Data, $Type, $TTL = RR Daten
# return = SOA Id
#
function updateRR( $zone, $Id, $Name, $Data, $Type, $TTL ) {

   if ( ! checkRR( $Name, $Data, $Type, $TTL ) ) {
        return FALSE;
   }

   mysqlUpdateRR( $zone, $Id, $Name, $Data, $Type, $TTL );
   return mysqlGetIdFromZoneName( $zone );
}
#------------------------------------------------------------------------------


//EOF
?>
