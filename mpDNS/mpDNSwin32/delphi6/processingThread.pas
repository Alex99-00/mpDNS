

unit processingThread;

interface

uses
  Classes, windows, winsock, libmpDNS;


type TProcessingData = record
     buf:PChar;
     maxBuf:Cardinal;
     addr:PSockaddrIn;
end;
type TQueue = array of TProcessingData;

// Verarbeitsungs Thread.
type
  TProcessingThread = class(TThread)
  private
    { Private declarations }
    pData:TQueue;
    procedure ini;
    procedure deini;
    procedure freeQueue;
  protected
    procedure Execute; override;
  public
    procedure putProcessing( buf:PChar; maxBuf:Cardinal; addr:PSockaddrIn );
  end;

implementation
{ TProcessingThread }
uses socket;

// Bei Beendigung die Queue freigeben.
procedure TProcessingThread.freeQueue;
var i,len:Integer;
begin
    len:=length(pData);
    for i:=0 to len-1 do begin
        libmpDNS.FREE(pData[i].buf);
        libmpDNS.FREE(pData[i].addr);
    end;
    setLength( pData, 0 );
end;

procedure TProcessingThread.ini;
begin
    //
end;

// Aufr‰umen bei Beendigung.
procedure TProcessingThread.deini;
begin
    Self.freeQueue;
end;

// Thread.
procedure TProcessingThread.Execute;
var len:Integer;
begin
    Self.ini;
    while ( not Self.Terminated ) do begin
            len:=length(pData);
            if ( len > 0 ) then begin
                 // Verarbeiten.
                 processDnsQuery( pData[len-1].buf, pData[len-1].maxBuf );
                 // Senden.
                 writeUdp( ssocket, pData[len-1].buf, pData[len-1].maxBuf,
                           pData[len-1].addr^ );
                 // Aufr‰umen.
                 libmpDNS.FREE(pData[len-1].buf);
                 libmpDNS.FREE(pData[len-1].addr);
                 setLength( pData, len-1 );
                 sleep(100);
            end else begin
                 sleep(10);
            end;
    end;
    Self.deini;
end;

// Zugrif von auﬂen.
procedure TProcessingThread.putProcessing( buf:PChar; maxBuf:Cardinal;
          addr:PSockaddrIn );
var len:Integer;
begin
    len:=length(pData);
    setLength( pData, len+1 );
    pData[len].buf   := buf;
    pData[len].maxBuf:= maxBuf;
    pData[len].addr  := addr;
end;

end.
