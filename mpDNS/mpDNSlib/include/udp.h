
#ifndef _UDP_H_
#define _UDP_H_

#if (defined(_WIN32))
#include <windows.h>
#else
#include <sys/socket.h>   /* Socket Funktionen */
#include <netinet/in.h>   /* Konstanten */
#endif

int readUdp( int socket, char * buf, size_t maxBuf, struct sockaddr_in * addr, int usec );

int writeUdp( int socket, char * buf, size_t writeBytes, struct sockaddr_in * addr );

int genSockaddressStruct( unsigned short int port, char *name, struct sockaddr_in * addr );

int getUdpServerSocket( unsigned short int port, char *name );

int getUdpClientSocket();

void closeUdpSocket( int socket );

#endif

/*EOF*/
