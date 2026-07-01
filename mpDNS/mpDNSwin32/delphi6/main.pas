

unit main;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, inThread, ExtCtrls, ComCtrls;

type
  TForm1 = class(TForm)
    StartButton: TButton;
    MemProtokoll: TMemo;
    Timer1: TTimer;
    GroupBox1: TGroupBox;
    edMySQLip: TEdit;
    edMySQLname: TEdit;
    edMySQLpass: TEdit;
    edMySQLdb: TEdit;
    edMySQLport: TEdit;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    GroupBox2: TGroupBox;
    Label1: TLabel;
    Label11: TLabel;
    edServerIP: TEdit;
    edServerPort: TEdit;
    edServerLL: TEdit;
    Label8: TLabel;
    StopButton: TButton;
    procedure StartButtonClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure StopButtonClick(Sender: TObject);
  private
    { Private declarations }
    isIni:Boolean;
    inThread:TInputThread; // Erzeugt outThread
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

procedure logCallback( msg:PChar; level:Integer ); cdecl;

implementation
{$R *.dfm}
uses libmpDNS;

// Callbackfür DLL.
procedure logCallback( msg:PChar; level:Integer ); cdecl;
begin
    Form1.MemProtokoll.Lines.Add( 'Level(' + IntToStr(level) + '): ' + msg );
end;

// Server starten.
procedure TForm1.StartButtonClick(Sender: TObject);
begin
    if (not isIni) then begin
        if (iniDNSlib(PChar(edMySQLip.text), PChar(edMySQLname.text),
                     PChar(edMySQLpass.Text), PChar(edMySQLdb.Text),
                     Word(StrToInt(edMySQLport.text)),
                     StrToInt(edServerLL.text),
                     logCallback) < 0 ) then begin
            MemProtokoll.lines.add( 'iniDNSlib error: ' + lastErrorMsg() );
        end else begin
            isIni:=True;
        end;
    end;
    inThread:=TInputThread.Create(True);
    inThread.port:=StrToInt(edServerport.Text);
    inThread.host:=edServerIP.Text;
    inThread.FreeOnTerminate:=False;// Mache ich
    inThread.Resume;
    StopButton.Enabled:=True;
    StartButton.Enabled:=False;
end;

// Initialisieren.
procedure TForm1.FormCreate(Sender: TObject);
begin
    isIni:=False;
    inThread:=nil;
end;

// Aufräumen.
procedure TForm1.FormDestroy(Sender: TObject);
begin
    if ( inThread <> nil ) then begin
         inThread.Terminate;
         inThread.WaitFor;
         inThread.Free;
    end;

    if ( isIni ) then
         deIniDNSlib();
end;

// Memo ab und zu löschen, da nur eine begrentzte Anzahl von
// Zeilen hineingehen.
procedure TForm1.Timer1Timer(Sender: TObject);
begin
    if ( MemProtokoll.Lines.Count > 500 ) then begin
         MemProtokoll.lines.Clear;
    end;
end;

// Server anhalten.
procedure TForm1.StopButtonClick(Sender: TObject);
begin
    if ( inThread <> nil ) then begin
         inThread.Terminate;
         inThread.WaitFor;
         inThread.Free;
         inThread:= nil;
    end;

    if ( isIni ) then begin
         deIniDNSlib();
         isIni:=False;
    end;
    StartButton.Enabled:=True;
    StopButton.Enabled:=False;
end;

end.
