<?php

/*
   mpDNSptr.php
    - Verwaltung der PTR Tabelle.
*/


#------------------------------------------------------------------------------
# Zeigt Seite mit allen PTR Einträgen an.
#
function showPTR() {
   global $filename;
   
   htmlHeadline("PTR");   /* Kopfzeile ausgeben */
   
   $ptrs = mysqlGetPTR(); /* Alle Einträge abfragen */
   
   if ( count($ptrs) <= 0 ) {
        htmlTabLayout2o();
        echo "<tr><td align=\"center\">No PTR records found<td></tr>\n";
        htmlTabLayout2c();
        return;
   }
   
   htmlTabLayout2o(); /* Tabellenbeschriftung */
   echo "<tr><td><b>Entrys:</b></td></tr>\n";
   htmlTabLayout2c();
   
   
   htmlTabLayout1o();

   echo "<tr class=\"head\">"; /* Tabellenkopf */
   echo "<th>Address</th><th>Name</th><th>TTL</th><th>Action</th><th>Action</th>\n";
   echo "</tr>\n";
   
   reset( $ptrs );
   while ( list($key,$val) = each($ptrs) ) { /* Alle Einträge ausgeben */
           echo "<tr class=\"data\">"
           
               ."<td align=\"center\" nowrap>"
               .$val['Address'] . ".IN-ADDR.ARPA"
               ."</td>"
                              
               ."<td align=\"center\" nowrap>"
               .$val['Name']
               ."</td>"
               
               ."<td align=\"center\" nowrap>"
               .$val['TTL']
               ."</td>"
               
               ."<td align=\"center\" nowrap>"
               ."<a href=\"{$filename}?cmd=delPTR&Id={$val['Id']}\" class=\"linklist\">Delete</a>"
               ."</td>"
               
               ."<td align=\"center\" nowrap>"
               ."<a href=\"{$filename}?cmd=editPTR&Id={$val['Id']}&Address={$val['Address']}&Name={$val['Name']}&TTL={$val['TTL']}\" class=\"linklist\">Edit</a>"
               ."</td>";
           
           echo "</tr>\n";
   }
   htmlTabLayout1c();
   
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Eingabemaske für neuen/alten PTR Eintrag.
# Ist Id leer neueintrag, sonst Eintrag bearbeiten.
# $Id = Id des zu ändernden Eintrags.
# $Address = IP Adresse, Format: 4.10.168.192
# $Name = Domainname
# $TTL = Time To Live Wert des Eintrags
#
function newPTR( $Id, $Address, $Name, $TTL ) {
   htmlHeadline("New PTR"); /* Kopfzeile */
   
   htmlTabLayout2o();
   
   echo "<tr><td>"
   .htmlFormO()
   .(($Id=='')?htmlHiddenInput("cmd","insertPTR"):
     htmlHiddenInput("cmd","updatePTR")
     .htmlHiddenInput("Id",$Id))
   ."<table align=\"center\">"
   ."<tr><th>Address</th><th>Name</th><th>TTL</th></tr>"
   ."<tr>"
   ."<td>".htmlInputText("Address", $Address, 16, 15, 0)."</td>"
   ."<td>".htmlInputText("Name", $Name, 48, 255, 0)."</td>"
   ."<td>".htmlInputText("TTL", $TTL, 11, 10, 0)."</td>"
   ."<td align=\"right\">".htmlSubmitButton("OK")."</td>"
   ."</tr>"
   ."</table>"
   .htmlFormC()
   ."</td></tr>\n";
   
   htmlTabLayout2c();
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Einfügen eines Eintrags in die Datenbank.
# $Address = IP Adresse, Format: 4.10.168.192
# $Name = Domainname
# $TTL = Time To Live Wert des Eintrags
#
function insertPTR( $Address, $Name, $TTL ) {
   if ( ! checkPTR( $Address, $Name, $TTL ) )
        return FALSE;
        
   return mysqlInsertPTR( $Address, $Name, $TTL );
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Ändern eines bestehenden Datenbankeintrags.
# $Id = Id des zu ändernden Eintrags.
# $Address = IP Adresse, Format: 4.10.168.192
# $Name = Domainname
# $TTL = Time To Live Wert des Eintrags
#
function updatePTR( $Id, $Address, $Name, $TTL ) {
   if ( ! checkPTR( $Address, $Name, $TTL ) )
        return FALSE;

   return mysqlUpdatePTR( $Id, $Address, $Name, $TTL );
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Löschen eines Datenbankeintrags.
# $Id = Id des zu löschenden Eintrags.
#
function deletePTR( $Id ) {
   return mysqlDeletePtr( $Id );
}
#------------------------------------------------------------------------------

//EOF
?>
