<?php

/*
   mpDNScheck.php
    - Syntax Checks aller Eingabemöglichkeiten.
*/

#------------------------------------------------------------------------------
# INT Typen
$INT16_MIN = 0;
$INT16_MAX = 0xffff;

$INT32_MIN = 0;
$INT32_MAX = 0x80000000;

$UINT32_MIN = 0;
$UINT32_MAX = 0xffffffff;

#--- Grenzen nach rfc1035 ---

# S.10, 2.3.4
$MAX_LABEL = 63;
$MAX_DOMAINNAME = 255;

# TTL Werte (normal; MINIMUM ist z.B. unsigned) - S.10, 2.3.4
$TTL_MIN= $INT32_MIN;
$TTL_MAX= $INT32_MAX;

# SOA S.19, 3.3.13
$SERIAL_MIN= $UINT32_MIN;
$SERIAL_MAX= $UINT32_MAX;

$REFRESH_MIN = $INT32_MIN;
$REFRESH_MAX = $INT32_MAX;

$RETRY_MIN = $INT32_MIN;
$RETRY_MAX = $INT32_MAX;

$EXPIRE_MIN = $INT32_MIN;
$EXPIRE_MAX = $INT32_MAX;

$MINIMUM_MIN = $UINT32_MIN;
$MINIMUM_MAX = $UINT32_MAX;

#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Testet die Syntax eines Domainnamen
# rfc1035, S.7/8, 2.3.1
# $domainName = Zu testender Name
# return TRUE|FALSE
#
function checkDomainName( $domainName ) {
   global $MAX_LABEL, $MAX_DOMAINNAME;
   
   if ( strlen($domainName) > $MAX_DOMAINNAME )
       return FALSE;
       
   $labels = split('\.', $domainName);
   
   reset($labels);
   while ( list($key,$val) = each($labels) ) {
           //echo $key.":".$val."<br>";
           if ( strlen($val) > $MAX_LABEL )
               return FALSE;
   }
   
   $regs = array();
   $ret = ereg( "^(([a-zA-Z]+[a-zA-Z0-9-]*[a-zA-Z]+)(.[a-zA-Z]+[a-zA-Z0-9-]*[a-zA-Z]+)*)$", $domainName, $regs );
   /*printHtmlArray($regs);*/
   return $ret;
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Syntax einer Netzwerk IP testen(Teil einer IP). 192.168 oder 192.168.10
# oder 192.168.10.1
# $IP = Zu testende IP
# return = TRUE|FALSE
#
function checkNetIP( $IP ) {
   $ipAr = split( '\.', $IP );
   
   if ( (count($ipAr) > 4) or (count($ipAr) <= 0) )
        return FALSE;
   
   reset($ipAr);
   while ( list($key,$val) = each($ipAr) ) {
   
           if ( ($val < 0) or ($val > 255) )
                return FALSE;
   
   }
   
   return TRUE;
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Syntax einer IP im vollen Format testen. 192.168.10.1
# $IP = Zu testende IP
# return TRUE|FALSE
#
function checkIP( $IP ) {
   $ipAr = split( '\.', $IP );
   
   if ( count($ipAr) != 4 )
        return FALSE;
   
   reset($ipAr);
   while ( list($key,$val) = each($ipAr) ) {
   
           if ( ($val < 0) or ($val > 255) )
                return FALSE;
   
   }
   
   return TRUE;
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Syntax einer PTR Angabe testen.
# $Address = Zu testende IP
# $Name = Zu testender Name
# $TTL = Zu testender Time To Live Wert
# return TRUE|FALSE
#
function checkPTR( $Address, $Name, $TTL ) {
   return ( checkNetIP( $Address ) and 
            checkDomainName($Name) and 
            checkTTL($TTL) );
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Syntax eines MX Eintrags testen.
# $mx = Zu testender Data Wert des Eintrags ( 10 nathan.ax )
# return TRUE|FALSE
#
function checkMX( $mx ) {
   global $INT16_MIN, $INT16_MAX;
   
   $mxAr = split( ' ', $mx );
   
   if ( count( $mxAr ) != 2 )
        return FALSE;
   
   if ( ( $mxAr[0] < $INT16_MIN ) or ( $mxAr[0] > $INT16_MAX ) )
        return FALSE;
   
   return checkDomainName( $mxAr[1] );
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Wertebereich einer TTL Angabe testen.
# $ttl = Zu testender TTL Wert
# return TRUE|FALSE
#
function checkTTL( $ttl ) {
   global $TTL_MIN, $TTL_MAX;
   return ( ( $ttl >= $TTL_MIN ) and ( $ttl <= $TTL_MAX) );
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Syntax eines SOA Eintrags testen.
# $Name, $Mail, $Serial, $Refresh, $Retry, $Expire, $Minimum = Zu testende
# SOA Werte.
# return TRUE|FALSE
#
function checkSOA( $Name, $Mail, $Serial, $Refresh, $Retry, $Expire, $Minimum ) {
   global $SERIAL_MAX, $REFRESH_MAX, $RETRY_MAX, $EXPIRE_MAX, $MINIMUM_MAX;
   global $SERIAL_MIN, $REFRESH_MIN, $RETRY_MIN, $EXPIRE_MIN, $MINIMUM_MIN;

   if ( ! checkDomainName( $Name ) )
        return FALSE;
   if ( ! checkDomainName( $Mail ) )
        return FALSE;

   if ( ( $Serial < $SERIAL_MIN ) or ( $Serial > $SERIAL_MAX ) )
       return FALSE;

   if ( ( $Refresh < $REFRESH_MIN ) or ( $Refresh > $REFRESH_MAX ) )
       return FALSE;

   if ( ( $Retry < $RETRY_MIN ) or ( $Retry > $RETRY_MAX ) )
       return FALSE;

   if ( ( $Expire < $EXPIRE_MIN ) or ( $Expire > $EXPIRE_MAX ) )
       return FALSE;

   if ( ( $Minimum < $MINIMUM_MIN ) or ( $Minimum > $MINIMUM_MAX ) )
       return FALSE;
   
   return TRUE;
}
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Resource Record testen.
# $Name = Domainname des Eintrags
# $Data = Daten/Wert des Eintrags
# $Type = Typ des Eintrags
# $TTL  = Time To Live Wert des EIntrags
# return TRUE|FALSE
#
function checkRR( $Name, $Data, $Type, $TTL ) {
   global $DNS_TYPE_NAME;
   
   if ( ! checkDomainName( $Name ) )
        return FALSE;
        
   if ( ! checkTTL( $TTL ) )
        return FALSE;
        
   switch ( $DNS_TYPE_NAME[$Type] ) {
      case "A":
               return checkIP( $Data );
      break;
      
      case "NS":
               return checkDomainName( $Data );
      break;
      
      case "CNAME":
               return checkDomainName( $Data );
      break;
      
      case "MX":
               return checkMX( $Data );
      break;
      
      default: return FALSE;
   }//switch

   return FALSE;
}
#------------------------------------------------------------------------------


//EOF
?>
