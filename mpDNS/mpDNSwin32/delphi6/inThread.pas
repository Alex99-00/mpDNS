

unit inThread;

interface

uses
  Classes, windows, winsock, ProcessingThread, libmpDNS;

// Eingangsthread.
type
  TInputThread = class(TThread)
  private
    { Private declarations }
    addr:PSockAddrIn;
    buf:PChar;
    maxBuf:Cardinal;
    pThread:TProcessingThread; // Verarbeitungs Thread.
    procedure ini;
    procedure deini;
  protected
    procedure Execute; override;
  public
    port:Word;                // Port des Servers.
    host:String;              // IP des Servers.
  end;

implementation
{ TInputThread }
uses socket;

// Initialisierung des Threads.
procedure TInputThread.ini;
begin
    pThread:=TProcessingThread.Create( False );
    pThread.FreeOnTerminate:=False;
    ssocket := getUdpServerSocket( Self.port, PChar(Self.host) );
end;

// Aufräumen.
procedure TInputThread.deini;
begin
    closeUdpSocket( ssocket );
    pThread.Terminate;
    pThread.WaitFor;
    pThread.Free;
end;

// Thread
procedure TInputThread.Execute;
var rb:Integer;
begin
    Self.ini;

    maxBuf:=512;
    buf:=MALLOC( maxBuf );
    addr:=MALLOC( sizeof(TSockaddrIn) );
    while ( not Self.Terminated ) do begin

            rb:= readUdp( ssocket, buf, maxBuf, addr^, 1000 );
            if ( rb <= 0 ) then begin
                 sleep(10);
                 continue; // Noch nichts da
            end;
            // In die Queue stellen.
            maxBuf:=Cardinal(rb);
            buf:=REALLOC(buf, maxBuf);
            pThread.putProcessing( buf, maxBuf, addr );

            maxBuf:=512;
            buf:=MALLOC( maxBuf );
            addr:=MALLOC( sizeof(TSockaddrIn) );
    end;
    libmpDNS.FREE(buf);
    libmpDNS.FREE(addr);
    Self.deini;
    
end;

end.
