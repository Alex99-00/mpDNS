<?php

/*
   mpDNStools.php
    - Tools.
*/

#------------------------------------------------------------------------------
# tools
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# PHP Array, in <pre> Tag eingeschlossen, ausgeben.
# $ar = Array das angezeigt werden soll
#
function printHtmlArray( $ar ) {
   echo "<br><pre>";
   print_r( $ar );
   echo "</pre><br>";
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Senden von HTTP Headern.
# $params = CGI Parameter ohne Dateinamen und ?
#
function sendHeader( $params ) {
   global $filename;
   Header("Location: {$filename}?" . $params );
}
#------------------------------------------------------------------------------

#EOF
?>
