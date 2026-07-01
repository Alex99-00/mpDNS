#/bin/sh


if [ ! -s /etc/mpDNS.cfg ]
then 
     echo
     install -m 644 -v ./install/mpDNS.cfg /etc/mpDNS.cfg
     echo
else 
     echo
     echo "File /etc/mpDNS.cfg already exists"
     echo "Please copy ./install/mpDNS.cfg manualy to /etc/mpDNS.cfg"
     echo
fi

if [ -e ./mpDNSlib/lib/libmpDNS.so ]
then
   if [ ! -s /usr/lib/libmpDNS.so ]
   then 
        echo
        install -m 755 -v ./mpDNSlib/lib/libmpDNS.so /usr/lib/libmpDNS.so
        echo
   else 
        echo
        echo "File /usr/lib/libmpDNS.so already exists"
        echo "Please copy ./mpDNSlib/lib/libmpDNS.so manualy to /usr/lib/libmpDNS.so"
        echo
   fi
fi

if [ ! -s /usr/sbin/mpDNS ]
then
     echo
     install -m 755 -v ./install/mpDNS /usr/sbin/mpDNS
     echo
else 
     echo
     echo "File /usr/sbin/mpDNS already exists"
     echo "Please copy ./install/mpDNS manualy to /usr/sbin/mpDNS"
     echo
fi


INITDPATH=

if [ -d /etc/init.d ]
then
    INITDPATH=/etc/init.d
else
    if [ -d /sbin/init.d ]
    then
        INITDPATH=/sbin/init.d
    fi
fi


if [ ! -s $INITDPATH/named ]
then
     echo
     install -m 755 -v ./install/named $INITDPATH/named
     echo
else 
     echo
     echo "File $INITDPATH/named already exists"
     echo "Please copy ./install/named manualy to $INITDPATH/named"
     echo
fi

