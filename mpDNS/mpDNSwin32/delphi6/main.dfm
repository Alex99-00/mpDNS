object Form1: TForm1
  Left = 192
  Top = 116
  Width = 618
  Height = 374
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  DesignSize = (
    610
    347)
  PixelsPerInch = 120
  TextHeight = 16
  object StartButton: TButton
    Left = 261
    Top = 312
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Start'
    TabOrder = 0
    OnClick = StartButtonClick
  end
  object MemProtokoll: TMemo
    Left = 8
    Top = 8
    Width = 329
    Height = 297
    Anchors = [akLeft, akTop, akRight, akBottom]
    ScrollBars = ssBoth
    TabOrder = 1
  end
  object GroupBox1: TGroupBox
    Left = 344
    Top = 8
    Width = 257
    Height = 193
    Anchors = [akTop, akRight]
    Caption = 'MySQL'
    TabOrder = 2
    object Label2: TLabel
      Left = 144
      Top = 32
      Width = 12
      Height = 16
      Caption = 'IP'
    end
    object Label3: TLabel
      Left = 144
      Top = 64
      Width = 86
      Height = 16
      Caption = 'Benutzername'
    end
    object Label4: TLabel
      Left = 144
      Top = 96
      Width = 55
      Height = 16
      Caption = 'Passwort'
    end
    object Label5: TLabel
      Left = 144
      Top = 128
      Width = 100
      Height = 16
      Caption = 'Datenbankname'
    end
    object Label6: TLabel
      Left = 144
      Top = 160
      Width = 24
      Height = 16
      Caption = 'Port'
    end
    object edMySQLip: TEdit
      Left = 16
      Top = 24
      Width = 121
      Height = 24
      TabOrder = 0
      Text = '127.0.0.1'
    end
    object edMySQLname: TEdit
      Left = 16
      Top = 56
      Width = 121
      Height = 24
      TabOrder = 1
      Text = 'alex'
    end
    object edMySQLpass: TEdit
      Left = 16
      Top = 88
      Width = 121
      Height = 24
      TabOrder = 2
      Text = 'baja'
    end
    object edMySQLdb: TEdit
      Left = 16
      Top = 120
      Width = 121
      Height = 24
      TabOrder = 3
      Text = 'mpDNS1'
    end
    object edMySQLport: TEdit
      Left = 16
      Top = 152
      Width = 121
      Height = 24
      TabOrder = 4
      Text = '3306'
    end
  end
  object GroupBox2: TGroupBox
    Left = 344
    Top = 208
    Width = 257
    Height = 129
    Anchors = [akTop, akRight]
    Caption = 'Server'
    TabOrder = 3
    object Label1: TLabel
      Left = 144
      Top = 32
      Width = 12
      Height = 16
      Caption = 'IP'
    end
    object Label11: TLabel
      Left = 144
      Top = 64
      Width = 24
      Height = 16
      Caption = 'Port'
    end
    object Label8: TLabel
      Left = 144
      Top = 97
      Width = 59
      Height = 16
      Caption = 'Log Level'
    end
    object edServerIP: TEdit
      Left = 16
      Top = 24
      Width = 121
      Height = 24
      TabOrder = 0
      Text = '192.168.10.5'
    end
    object edServerPort: TEdit
      Left = 16
      Top = 56
      Width = 121
      Height = 24
      TabOrder = 1
      Text = '53'
    end
    object edServerLL: TEdit
      Left = 16
      Top = 89
      Width = 121
      Height = 24
      TabOrder = 2
      Text = '3'
    end
  end
  object StopButton: TButton
    Left = 176
    Top = 312
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Stop'
    Enabled = False
    TabOrder = 4
    OnClick = StopButtonClick
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 456
    Top = 16
  end
end
