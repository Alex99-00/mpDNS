<?php

/*
   mpDNShtml.php
    - HTML Vorlagen.
*/

#------------------------------------------------------------------------------
# html
#------------------------------------------------------------------------------
$bodyBgColor      = '#eeeeee';;#'#ccffcc';
$tabBorderBgColor = '#3333cc';
$tabHeadRowBgColor= '#ffffff';
$tabRowBgColor    = '#ffffff';

#------------------------------------------------------------------------------
# Ausgabe des HTML Kopfes bis einschl. dem <body> Tag.
#
function htmlHead() {
   global $bodyBgColor, $tabHeadRowBgColor, $tabRowBgColor;
   echo "<html><head>"
       ."<title>mpDNSadmin - Multi Platform DNS Admin</title>"
       ."<style type=\"text/css\">"
       ." <!-- "
       ."body { font-size: 100%; "
              ."font-family: Arial, Helvetica, Verdana; color: #000000; "
              ."background-color: {$bodyBgColor}; }"
       ."b.big { font-size: 120%; }"
       ."b.middle { font-size: 120%; }"
       ."table.grey { background-color: #eeeeee; }"
       ."table.grey1 { background-color: #dddddd; }"
       ."table.grey2 { background-color: #cccccc; }"       
       ."table.bg { background-color: #3333cc; color: #ffcccc; }"
       ."a.linklist { font-size: 80%; font-weight: bold; "
                    ."text-decoration: none; color: #000000; }"
       ."a.linkpur { text-decoration: none; }"
       ."tr.head { background-color: {$tabHeadRowBgColor}; }"
       ."tr.data { background-color: {$tabRowBgColor}; font-family: Courier, \"Courier New\", monospace }"
       ." --> "
       ."</style>"
       ."</head><body>\n";
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Ausgabe der Fußzeile jeder Seite.
#
function htmlTail() {
   echo "\n</body></html>\n";
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Ausgabe einer Überschrift.
# $text = Text der als Überschrift ausgegeben werden soll.
#
function htmlHeadline( $text ) {
   echo "<table align=\"center\" border=\"0\" width=\"90%\" class=\"bg\"><tr>"
       ."<td valign=\"middle\" align=\"center\">"
       ."<b class=\"middle\">{$text}</b>"
       ."</td></tr></table><br>";
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Tabellenlayout Nr. 1, bis <tr> Tag.
#
function htmlTabLayout1O() {
   global $tabBorderBgColor;
   echo "<table align=\"center\" bgcolor=\"{$tabBorderBgColor}\"" 
       ." cellpadding=\"0\" cellspacing=\"1\" width=\"90%\" border=\"0\">"
       ."<tr><td>"
       ."<table align=\"center\" bgcolor=\"{$tabBorderBgColor}\"" 
       ." cellpadding=\"5\" cellspacing=\"1\" width=\"100%\" border=\"0\">";       
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Tabellenlayout Nr. 2, bis <tr> Tag.
#
function htmlTabLayout2O() {
   global $tabBorderBgColor;
   echo "<table align=\"center\" cellpadding=\"0\" "
       ."cellspacing=\"1\" width=\"90%\">\n";
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Abschluß des Tabellenlayouts Nr. 1.
#
function htmlTabLayout1C() {
   echo "</table></td></tr></table>\n";
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Abschluß des Tabellenlayouts Nr. 2.
#
function htmlTabLayout2C() {
   echo "</table>\n";
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Gibt einen String mit einer aufgebauten Select Box zurück.
# $values   = Werte für Einträge
# $textar   = Anzuzeigender Text für jeden Werteeintrag
# $name     = Namens Attribut für den Select Tag, also cgi name
# $size     = Sichtbare Einträge
# $selected = Selektierter Eintrag
# return    = html Tag
#
function htmlSelect( $values, $name, $size, $selected ) {
   $ret = "<select name=\"$name\" size=\"$size\">\n";
   
   if ( count($values) <= 0 ) { $values = $textar; }
   
   reset( $values );
   while ( list($key,$val) = each($values) ) {
           $ret .= "<option value=\"{$key}\"";
           $ret .= ($key == $selected) ? " selected" : "";
           $ret .= "> {$val}</option>\n";
           $i++;
   }
   
   $ret .= "</select>\n";
   
   return $ret;
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Gibt einen String mit einer aufgebauten Select Box zurück.
# $values   = Werte für Einträge
# $textar   = Anzuzeigender Text für jeden Werteeintrag
# $name     = Namens Attribut für den Select Tag, also cgi name
# $size     = Sichtbare Einträge
# $selected = Selektierter Eintrag
# return    = html Tag
#
function htmlSelectByValue( $values, $textar, $name, $size, $selected ) {
   $ret = "<select name=\"$name\" size=\"$size\">\n";
   
   if ( count($values) <= 0 ) { $values = $textar; }
   
   $i=0;
   while ( $i < count($textar) ) {
           $ret .= "<option value=\"{$values[$i]}\"";
           $ret .= ($values[$i] == $selected) ? " selected" : "";
           $ret .= "> {$textar[$i]}</option>\n";
           $i++;
   }
   
   $ret .= "</select>\n";
   
   return $ret;
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Gibt einen HTML <form> Tag zurück, mit methode=POST.
#
function htmlFormO() {
   global $filename;
   return "<form action=\"{$filename}\" method=\"POST\">\n";
}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Abschluß eines <form> Tags.
#
function htmlFormC() {
   return "\n</form>\n";
}
#------------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Hidden input Tag für Formulare.
# $name  = Name des cgi-parameters
# $value = Wert des cgi-parameters
# return = html Tag
#
function htmlHiddenInput($name, $value) {
   return "<input type=\"hidden\" value=\"$value\" name=\"$name\">\n";
}
#-----------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Gibt einen Submit Button zurück.
# $value = Beschriftung des Buttons
# return = html Tag
#
function htmlSubmitButton($value) {
   return "<input type=\"submit\" value=\"{$value}\">\n";
}
#------------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Text input Zeile für Formulare.
# $name     = Name des cgi-parameters
# $value    = Wert des cgi-parameters
# $size     = Sichtbare Länge
# $maxlen   = Maximale eingabe Länge
# $readonly = Nur Lesen -> !=0:nein, 1:ja
# return    = html Tag
#
function htmlInputText($name, $value, $size, $maxlen, $readonly) {

   if ( $readonly == 1) {
        return "<input type=\"text\" name=\"$name\" value=\"$value\" "
              ."size=\"$size\" maxlength=\"$maxlen\" readonly>\n";
   } else {
        return "<input type=\"text\" name=\"$name\" value=\"$value\" "
              ."size=\"$size\" maxlength=\"$maxlen\">\n";

   }

}
#-----------------------------------------------------------------------------

#EOF
?>
