<?php

/*
   mpDNSsoa.php
    - Verwaltung der SOA Tabelle.
*/

#------------------------------------------------------------------------------
# Zeigt ein oder mehrere SOA Einträge und deren Untereinträge an.
# Wenn Id leer dann alle SOA Einträge.
#
# Id = Id des anzuzeigenden Eintrags der SOA Tabelle.
#
function showSOA( $Id ) {
   global $filename, $DNS_TYPE_NAME;
         
   if ( $Id > 0 ) { /* Alle ? */
        $soas = mysqlGetSOA( $Id );
   } else {
        $soas = mysqlGetSOAs();
   }
   
   if ( count($soas) <= 0 ) { /* Noch nichts vorhanden */
        htmlHeadline("SOA");
        htmlTabLayout2o();
        echo "<tr><td align=\"center\">No SOA(s) found<td></tr>\n";
        htmlTabLayout2c();     
        return;
   }
   
   reset($soas);
   while ( list($idx,$soa) = each($soas) ) { /* Alle SOAs durchgehen */
           
           htmlHeadline("Zone&nbsp;&nbsp;\"".$soa['Name']."\"");
           
           htmlTabLayout2o(); /* Tabellenbeschriftung */
           echo "<tr><td><b>SOA:</b></td></tr>\n";
           htmlTabLayout2c();
   
           htmlTabLayout1o();

           echo "<tr class=\"head\">"; /* Tabellenkopf */
           echo "<th>Name</th><th>Mail</th><th>Serial</th><th>Refresh</th>
                 <th>Retry</th><th>Expire</th><th>Minimum</th>
                 <th>Action</th><th>Action</th>\n";
           echo "</tr>\n";
                   
           echo "<tr class=\"data\">"
           
           ."<td align=\"center\" nowrap>"
           .$soa['Name']
           ."</td>"
                              
           ."<td align=\"center\" nowrap>"
           .$soa['Mail']
           ."</td>"
               
           ."<td align=\"center\" nowrap>"
           .$soa['Serial']
           ."</td>"
                   
           ."<td align=\"center\" nowrap>"
           .$soa['Refresh']
           ."</td>"
               
           ."<td align=\"center\" nowrap>"
           .$soa['Retry']
           ."</td>"
                   
           ."<td align=\"center\" nowrap>"
           .$soa['Expire']
           ."</td>"
                   
           ."<td align=\"center\" nowrap>"
           .$soa['Minimum']
           ."</td>"
               
           ."<td align=\"center\" nowrap>"
           ."<a href=\"{$filename}?cmd=delSOA&Id={$soa['Id']}\" class=\"linklist\">Delete</a>"
           ."</td>"
               
           ."<td align=\"center\" nowrap>"
           ."<a href=\"{$filename}?cmd=editSOA&Id={$soa['Id']}&Name={$soa['Name']}"
           ."&Mail={$soa['Mail']}&Serial={$soa['Serial']}"
           ."&Refresh={$soa['Refresh']}&Retry={$soa['Retry']}&Expire={$soa['Expire']}"
           ."&Minimum={$soa['Minimum']}\" class=\"linklist\">Edit</a>"
           ."</td>";
           
           echo "</tr>\n";
                   
           htmlTabLayout1c();
           echo "<br>";
           
           #--Entrys--
           
           htmlTabLayout2o(); /* Tabellenbeschriftung */
           echo "<tr><td><b>Entrys:</b></td></tr>\n";
           htmlTabLayout2c();
           
           htmlTabLayout1o();

           echo "<tr class=\"head\">"; /* Tabellenkopf */
           echo "<th>Name</th><th>Data</th><th>Type</th><th>TTL</th>
                 <th>Action</th><th>Action</th>\n";
           echo "</tr>\n";
           
           $rrs = mysqlGetRRs( $soa['Name'] );
         
           while ( list($idx2,$rr) = each($rrs) ) {
              echo "<tr class=\"data\">"
              
              ."<td align=\"center\" nowrap>"
              .$rr['Name']
              ."</td>"
           
              ."<td align=\"center\" nowrap>"
              .$rr['Data']
              ."</td>"
           
              ."<td align=\"center\">"
              .$DNS_TYPE_NAME[$rr['Type']]
              ."</td>"

              ."<td align=\"center\" nowrap>"
              .$rr['TTL']
              ."</td>";

              echo "<td align=\"center\"><a href=\"{$filename}?cmd=delRR&zone={$soa['Name']}&Id={$rr['Id']}\" class=\"linklist\">Delete</a></td>\n";
              echo "<td align=\"center\"><a href=\"{$filename}?cmd=editRR&zone={$soa['Name']}&Id={$rr['Id']}&Name={$rr['Name']}&Data={$rr['Data']}&Type={$rr['Type']}&TTL={$rr['TTL']}\" class=\"linklist\">Edit</a></td>\n";
              echo "</tr>\n";
           }
         
                   
           htmlTabLayout1c();
           echo "<br><center><a href=\"{$filename}?cmd=newRR&zone={$soa['Name']}\" class=\"linklist\">[New Zone Entry]</a></center>";
           echo "<br>";
                         
   }//while
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Eingabemaske für einen SOA (eine Zone). Neu oder bearbeiten.
# Wenn Id leer dann neu, sonst bearbeitung eines SOA Eintrags.
# Id = Id des zu bearbeitenden Eintrags der SOA Tabelle.
# Name, Mail, Serial, Refresh, Retry, Expire, Minimum = SOA Daten.
#
function newSOA( $Id, $Name, $Mail, $Serial, $Refresh, $Retry, $Expire, $Minimum ) {
   htmlHeadline("New SOA/Zone");

   if ( $Serial == '' )
        $Serial = date("YmdH");
        
   if ( $Refresh == '' )
        $Refresh = 3600;
        
   if ( $Retry == '' )
        $Retry = 3600;
        
   if ( $Expire == '' )
        $Expire = 3600;
        
   if ( $Minimum == '' )
        $Minimum = 3600;
   
   htmlTabLayout2o();
   echo ""
   ."<tr><td>"
   .htmlFormO()
   .(($Id=='')?htmlHiddenInput("cmd","insertSOA"):htmlHiddenInput("cmd","updateSOA"))
   .htmlHiddenInput("Id",$Id)
   ."<table align=\"center\">"   
   
   ."<tr><td>Name</td><td>&nbsp;</td><td>".htmlInputText("Name", $Name, 64, 255, (($ed)?1:0))."</td></tr>"
   ."<tr><td>Mail</td><td>&nbsp;</td><td>".htmlInputText("Mail", $Mail, 64, 255, 0)."</td></tr>"
   ."<tr><td>Serial</td><td>&nbsp;</td><td>".htmlInputText("Serial", $Serial, 12, 11, 0)."</td></tr>"
   ."<tr><td>Refresh</td><td>&nbsp;</td><td>".htmlInputText("Refresh", $Refresh, 11, 10, 0)."</td></tr>"
   ."<tr><td>Retry</td><td>&nbsp;</td><td>".htmlInputText("Retry", $Retry, 11, 10, 0)."</td></tr>"
   ."<tr><td>Expire</td><td>&nbsp;</td><td>".htmlInputText("Expire", $Expire, 11, 10, 0)."</td></tr>"
   ."<tr><td>Minimum</td><td>&nbsp;</td><td>".htmlInputText("Minimum", $Minimum, 11, 10, 0)."</td></tr>"
   
   ."<td align=\"left\" colspan=\"3\">".htmlSubmitButton("OK")."</td></tr>"
   ."</table>"
   .htmlFormC()
   ."</td></tr>\n";
   htmlTabLayout2c();
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Update eines SOA Eintrags.
# Gibt FALSE zurück wenn der SOA Eintrag fehlerhafte Werte/Syntax enthält. 
# Id = Id des zu bearbeitenden Eintrags der SOA Tabelle.
# Name, Mail, Serial, Refresh, Retry, Expire, Minimum = SOA Daten.
# return = TRUE|FALSE
#
function updateSOA( $Id, $Name, $Mail, $Serial, $Refresh, $Retry, $Expire, $Minimum ) {
   if ( ! checkSOA($Name, $Mail, $Serial, $Refresh, $Retry, $Expire, $Minimum ) ) 
        return FALSE;

   return mysqlUpdateSOA( $Id, $Name, $Mail, $Serial, $Refresh, $Retry, $Expire, $Minimum );
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Löschen eines SOA Eintrags.
# Id = Id des zu löschenden Eintrags der SOA Tabelle.
# return = TRUE|FALSE
#
function deleteSOA( $Id ) {
   return mysqlDeleteSOA( $Id );
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Einfügen eines neuen SOA Eintrags in die SOA Tabelle.
# Gibt FALSE zurück wenn der SOA Eintrag fehlerhafte Werte/Syntax enthält. 
# Name, Mail, Serial, Refresh, Retry, Expire, Minimum = SOA Daten.
# return = TRUE|FALSE
#
function insertSOA( $Name, $Mail, $Serial, $Refresh, $Retry, $Expire, $Minimum ) {
   if ( ! checkSOA($Name, $Mail, $Serial, $Refresh, $Retry, $Expire, $Minimum ) ) 
        return FALSE;

   return mysqlInsertSOA( $Name, $Mail, $Serial, $Refresh, $Retry, $Expire, $Minimum );
}
#------------------------------------------------------------------------------

//EOF
?>
