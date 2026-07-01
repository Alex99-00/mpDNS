

unit libmpDNS;

interface
uses windows, winsock;

const libmpDNSName = 'libmpDNS.dll';

{typedef void (*LogFunktion)(char *, int);
int iniDNSlib( char * host, char * user, char * pass, char * db, USI port,
               int eDebugLevel, LogFunktion logCallback );}
type TlogCallback = procedure( msg:PChar; level:Integer ) cdecl;
function iniDNSlib( host:PChar;
                    user:PChar;
                    pass:PChar;
                    db:PChar;
                    port:Word;
                    debugLevel:Integer;
                    logCallback:TlogCallback
                  ):Integer cdecl; external libmpDNSName;


{void deIniDNSlib(void);}
procedure deIniDNSlib() cdecl; external libmpDNSName;


{int processDNSquery( char ** msg, size_t * msgLen );}
//type TPCharPointer = ^PChar;
//function processDNSquery( msg:TPCharPointer; msgLen:Cardinal ):
//                          Integer cdecl; external libmpDNSName;
function processDNSquery( var msg:PChar; var msgLen:Cardinal ):
                          Integer cdecl; external libmpDNSName;


{int getUdpServerSocket( unsigned short int port, char *name );}
function getUdpServerSocket( port:Word; name:PChar ):
                             Integer cdecl; external libmpDNSName;


{int getUdpClientSocket();}
function getUdpClientSocket():Integer cdecl; external libmpDNSName;


{void closeUdpSocket( int socket );}
procedure closeUdpSocket( socket:Integer ) cdecl; external libmpDNSName;


{int readUdp( int socket, char * buf, size_t maxBuf,
              struct sockaddr_in * addr );}
function readUdp( socket:Integer; buf:PChar; maxBuf:Cardinal;
                  var addr:TSockAddrIn; usec:Integer ):
                  Integer cdecl; external libmpDNSName;


{int writeUdp( int socket, char * buf, size_t writeBytes,
               struct sockaddr_in * addr );}
function writeUdp(socket:Integer; buf:PChar; writeBytes:Cardinal;
                  var addr:TSockAddrIn):
                  Integer cdecl; external libmpDNSName;


{int genSockaddressStruct( unsigned short int port, char *name,
                           struct sockaddr_in * addr );}
function genSockaddressStruct( port:Word; name:PChar;
                               var addr:TSockAddrIn ):
                               Integer cdecl; external libmpDNSName;


{char * lastErrorMsg(void)}
function lastErrorMsg():PChar; cdecl; external libmpDNSName;


{void * MALLOC (size_t size);}
function MALLOC( size:Cardinal ):Pointer; cdecl; external libmpDNSName;


{void * FREE (void *ptr);}
function FREE( ptr:Pointer ):Pointer; cdecl; external libmpDNSName;


{void * REALLOC (void *ptr, size_t size);}
function REALLOC( ptr:Pointer; size:Cardinal ):
                  Pointer; cdecl; external libmpDNSName;

implementation

end.

