

# static no debug
all: ; cd mpDNSlib ; make all ; cd .. ; cd program ; make all


# static debug
debug: ; cd mpDNSlib ; make debug ; cd .. ; cd program ; make debug


# dynamic no debug -> ! not cygwin !
dynamic: ; cd mpDNSlib ; make dynamic ; cd .. ; cd program ; make dynamic


# clean all
clean: ; cd mpDNSlib ; make clean ; cd .. ; cd program ; make clean



.PHONY: install uninstall


# install mpDNS
# !!! dynamic lib target = /usr/lib !!!
install: ; ./scripts/install.sh


# uninstall mpDNS
uninstall: ; ./scripts/uninstall.sh

