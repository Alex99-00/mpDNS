<?php

/*
   mpDNSadmin.php
    - Hauptscript von "mpDNSadmin".
*/

#------------------------------------------------------------------------------
# Name des Scripts
$filename = 'mpDNSadmin.php';

# Pfad zu den einzelnen Code Dateien
$incPath = './phpInclude';


require( $incPath . '/mpDNShtml.php' );
require( $incPath . '/mpDNSmysql.php' );
require( $incPath . '/mpDNStools.php' );

require( $incPath . '/mpDNScheck.php' );

require( $incPath . '/mpDNSrr.php' );// DNS Types...
require( $incPath . '/mpDNSptr.php' );
require( $incPath . '/mpDNSsoa.php' );
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Gibt SelectBox für Zonenauswahl aus.
# $select = Welche Zone selektiert ist
#
function printZoneSelectBox( $select ) {
   if ( $select == "Alle" ) $select = 0;
   $zoneList = mysqlGetZoneNames();
   array_push($zoneList['Ids'],"0");
   array_push($zoneList['Names'],"Alle");
   reset($zoneList['Ids']); reset($zoneList['Names']);
   echo htmlFormO();
   echo htmlHiddenInput( "cmd", "showSOA" );
   echo htmlSelectByValue( $zoneList['Ids'], $zoneList['Names'], Id, 1, $select );
   echo htmlSubmitButton("Go");
   echo htmlFormC();
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Gibt Linkleiste links neben der Zonenauswahl aus.
#
function printLinkList() {
   global $filename;
   echo "<table align=\"center\" border=\"0\" cellpadding=\"0\" "
       ."cellspacing=\"0\"><tr>"
       ."<td nowrap><a href=\"{$filename}?cmd=newPTR\" class=\"linklist\">[New PTR]</a></td><td>&nbsp;</td>"
       ."<td nowrap><a href=\"{$filename}?cmd=showPTR\" class=\"linklist\">[Show PTR]</a></td><td>&nbsp;</td><td>&nbsp;</td>"
       ."<td nowrap><a href=\"{$filename}?cmd=newSOA\" class=\"linklist\">[New Zone]</a></td>"
       ."</tr></table>";
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Gibt Kopfzeile aller Seiten aus.
# $select = Welche Zone selektiert ist, für printZoneSelectBox
#
function printTopLine( $select ) {
   global $filename;
echo "<table align=\"center\" width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\"><tr><td  align=\"left\">"
."<table align=\"center\" width=\"98%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\"><tr><td  align=\"left\">"

."<a href=\"{$filename}\" class=\"linkpur\">"
."<table align=\"left\" border=\"0\" cellpadding=\"0\" cellspacing=\"2\">"
."<tr><td align=\"center\" nowrap><b class=\"big\">mpDNSadmin</b></td></tr>"
."<tr><td align=\"center\" nowrap><small>Multi Platform DNS Admin</small></td></tr>"
."</table>"
."</a>"

."</td><td align=\"right\">"


."<table align=\"right\" border=\"0\"  cellpadding=\"0\" cellspacing=\"2\">"
."<tr><td align=\"center\" nowrap>"

."<table align=\"right\" border=\"0\" cellpadding=\"0\" cellspacing=\"2\">"
."<tr><td align=\"center\" nowrap>";
printLinkList();
echo "</td></tr></table>"

."</td><td align=\"center\" nowrap>";
printZoneSelectBox( $select );
echo "</td></tr></table>"


."</td></tr></table>"
."</td></tr></table>"
."<hr><br>";

}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
## MAIN
#------------------------------------------------------------------------------

# GET/POST vereinfachen
if ( $HTTP_POST_VARS['cmd'] != '' ) {
     $HTTP_VARS = $HTTP_POST_VARS;
} else {
     $HTTP_VARS = $HTTP_GET_VARS;
}

# mysql Verbindung aufbauen
if ( ! mysqlConnect() ) {
     echo "Can´t connect to MySQL";
     exit;
}


switch ( $HTTP_VARS['cmd'] ) {

   # Resource Records
   #RR-------------------------------------------------------------------------
   case "delRR":
         // returns SOA Id !!!
         $id=deleteRR( $HTTP_VARS['zone'], $HTTP_VARS['Id'] );
         sendHeader("cmd=showSOA&Id=".$id);
   break;
   
   case "newRR":
         htmlHead();
         printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
         newRR( $HTTP_VARS['zone'], "", "", "", "", "" );
         htmlTail();
   break;
   
   case "insertRR":
         // returns SOA Id !!!
         if ( ! ($id=insertRR( $HTTP_VARS['zone'], $HTTP_VARS['Name'], 
                       $HTTP_VARS['Data'], $HTTP_VARS['Type'], 
                       $HTTP_VARS['TTL'] ))) {
              htmlHead();
              printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
              newRR( $HTTP_VARS['zone'], ""/*$HTTP_VARS['Id']*/, $HTTP_VARS['Name'], 
                     $HTTP_VARS['Data'], $HTTP_VARS['Type'], $HTTP_VARS['TTL'] );         
              htmlTail();
         } else {
              sendHeader("cmd=showSOA&Id=".$id);
         }
   break;
   
   case "editRR":
         htmlHead();
         printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
         newRR( $HTTP_VARS['zone'], $HTTP_VARS['Id'], $HTTP_VARS['Name'], 
                $HTTP_VARS['Data'], $HTTP_VARS['Type'], $HTTP_VARS['TTL'] );         
         htmlTail();
   break;
   
   case "updateRR":
         // returns SOA Id !!!
         if ( ! ($id=updateRR( $HTTP_VARS['zone'], $HTTP_VARS['Id'], 
                               $HTTP_VARS['Name'], $HTTP_VARS['Data'], 
                               $HTTP_VARS['Type'], $HTTP_VARS['TTL'] ))) {
              htmlHead();
              printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
              newRR( $HTTP_VARS['zone'], $HTTP_VARS['Id'], $HTTP_VARS['Name'], 
                     $HTTP_VARS['Data'], $HTTP_VARS['Type'], $HTTP_VARS['TTL'] );         
              htmlTail();
         } else {
              sendHeader("cmd=showSOA&Id=".$id);
         }
   break;
   #RR-END---------------------------------------------------------------------
   
   # State of Authority
   #SOA------------------------------------------------------------------------
   case "showSOA":
         htmlHead();
         printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
         showSOA( $HTTP_VARS['Id'] );
         htmlTail();
   break;
   
   case "delSOA":
         deleteSOA( $HTTP_VARS['Id'] );
         sendHeader("");
   break;
   
   case "newSOA":
         htmlHead();
         printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
         newSOA( "", "", "", "", "", "", "", "" );
         htmlTail();
   break;
   
   case "insertSOA":
         if ( ! ($id=insertSOA( $HTTP_VARS['Name'], $HTTP_VARS['Mail'], $HTTP_VARS['Serial'], 
                        $HTTP_VARS['Refresh'], $HTTP_VARS['Retry'], $HTTP_VARS['Expire'],
                        $HTTP_VARS['Minimum'] )) ) {
                 htmlHead();
                 printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
                 newSOA( /*$HTTP_VARS['Id']*/"", $HTTP_VARS['Name'], $HTTP_VARS['Mail'], 
                         $HTTP_VARS['Serial'], $HTTP_VARS['Refresh'], $HTTP_VARS['Retry'], 
                         $HTTP_VARS['Expire'], $HTTP_VARS['Minimum'] );         
                 htmlTail();
         } else {
              sendHeader("cmd=showSOA&Id=".$id);
         }
   break;
   
   case "editSOA":
         htmlHead();
         printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
         newSOA( $HTTP_VARS['Id'], $HTTP_VARS['Name'], $HTTP_VARS['Mail'], 
                 $HTTP_VARS['Serial'], $HTTP_VARS['Refresh'], $HTTP_VARS['Retry'], 
                 $HTTP_VARS['Expire'], $HTTP_VARS['Minimum'] );         
         htmlTail();
   break;
   
   case "updateSOA":
         if ( ! updateSOA( $HTTP_VARS['Id'], $HTTP_VARS['Name'], $HTTP_VARS['Mail'], 
                    $HTTP_VARS['Serial'], $HTTP_VARS['Refresh'], $HTTP_VARS['Retry'], 
                    $HTTP_VARS['Expire'], $HTTP_VARS['Minimum'] ) ) {
              htmlHead();
              printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
              newSOA( $HTTP_VARS['Id'], $HTTP_VARS['Name'], $HTTP_VARS['Mail'], 
                      $HTTP_VARS['Serial'], $HTTP_VARS['Refresh'], $HTTP_VARS['Retry'], 
                      $HTTP_VARS['Expire'], $HTTP_VARS['Minimum'] );         
              htmlTail();
         } else {
              sendHeader("cmd=showSOA&Id=".$HTTP_VARS['Id']);
         }
   break;
   #SOA-END--------------------------------------------------------------------

   # Domainnamen Zeiger
   #PTR------------------------------------------------------------------------
   case "showPTR":
         htmlHead();
         printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
         showPTR();
         htmlTail();
   break;
   
   case "delPTR":
         deletePTR( $HTTP_VARS['Id'] );
         sendHeader("cmd=showPTR");
   break;
   
   case "newPTR":
         htmlHead();
         printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
         newPTR("","","","");
         htmlTail();
   break;
   
   case "insertPTR":
         if ( ! insertPTR( $HTTP_VARS['Address'], $HTTP_VARS['Name'], $HTTP_VARS['TTL'] ) ) {
              htmlHead();
              printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
              newPTR( ""/*$HTTP_VARS['Id']*/, $HTTP_VARS['Address'], $HTTP_VARS['Name'], $HTTP_VARS['TTL'] );
              htmlTail();
         } else {
              sendHeader("cmd=showPTR");
         }
   break;
   
   case "editPTR":
         htmlHead();
         printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
         newPTR( $HTTP_VARS['Id'], $HTTP_VARS['Address'], $HTTP_VARS['Name'], $HTTP_VARS['TTL'] );
         htmlTail();
   break;
   
   case "updatePTR":
         if ( ! updatePTR( $HTTP_VARS['Id'], $HTTP_VARS['Address'], $HTTP_VARS['Name'], $HTTP_VARS['TTL'] ) ) {
              htmlHead();
              printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
              newPTR( $HTTP_VARS['Id'], $HTTP_VARS['Address'], $HTTP_VARS['Name'], $HTTP_VARS['TTL'] );
              htmlTail();
         } else {
              sendHeader("cmd=showPTR");
         }
   break;
   #PTR-END--------------------------------------------------------------------

   # Leere Seite
   default:
           htmlHead();
           printTopLine( ($HTTP_VARS['Id']=='')?"Alle":$HTTP_VARS['Id'] );
           htmlTail();
   break;
   
}

# mysql Verbindung beenden
mysqlDisconnect();

//EOF
?>
