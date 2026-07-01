

program mpDNS;

uses
  Forms,
  main in 'main.pas' {Form1},
  inThread in 'inThread.pas',
  processingThread in 'processingThread.pas',
  libmpDNS in 'libmpDNS.pas',
  socket in 'socket.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
