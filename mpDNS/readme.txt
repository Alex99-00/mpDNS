
#------------------------------------------------------------------------------

mpDNS & mpDNSlib v1.0r

#------------------------------------------------------------------------------

Install(SuSE7.3):

 make 
 make install 
 edit /etc/mpDNS.cfg
 copy mpDNSadmin to your Apache Directory
 edit mpDNSadmin/phpInclude/mpDNSmysql.php to fit your MySQL Database
 Start your Browser
 Insert Zones and Entrys
 start the DNS, /etc/init.d/named start
 
