#/bin/sh

echo
rm -v -f /etc/mpDNS.cfg

echo
rm -v -f /usr/sbin/mpDNS

if [ -e /usr/lib/libmpDNS.so ]
then
   echo
   rm -v -f /usr/lib/libmpDNS.so
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

echo
rm -v -f $INITDPATH/named
echo
