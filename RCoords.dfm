object RwyInput: TRwyInput
  Left = 0
  Top = 0
  Caption = 'Runway Coodrinates Entry'
  ClientHeight = 299
  ClientWidth = 635
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 72
    Top = 24
    Width = 4
    Height = 16
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 48
    Top = 104
    Width = 97
    Height = 16
    Caption = 'Low End Latitude'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 336
    Top = 104
    Width = 107
    Height = 16
    Caption = 'Low End Longitude'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label4: TLabel
    Left = 336
    Top = 144
    Width = 109
    Height = 16
    Caption = 'High End Longitude'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label5: TLabel
    Left = 48
    Top = 144
    Width = 99
    Height = 16
    Caption = 'High End Latitude'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label8: TLabel
    Left = 47
    Top = 184
    Width = 33
    Height = 16
    Caption = 'Width'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label6: TLabel
    Left = 168
    Top = 184
    Width = 75
    Height = 16
    Caption = 'Airport name'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label7: TLabel
    Left = 423
    Top = 184
    Width = 70
    Height = 16
    Caption = '(No Spaces)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label9: TLabel
    Left = 516
    Top = 184
    Width = 29
    Height = 16
    Caption = 'ICAO'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Memo1: TMemo
    Left = 138
    Top = 23
    Width = 351
    Height = 59
    Lines.Strings = (
      
        'Enter the runway endpoints including any displaced threshold are' +
        'a.'
      
        'The low end is the one where you land to the East in other words' +
        ' '
      'from 000'#176' to 179'#176'.'
      
        'For best results use the same measuerments as apt.dat or WED use' +
        's.'
      '')
    TabOrder = 0
  end
  object LElat: TEdit
    Left = 168
    Top = 103
    Width = 89
    Height = 21
    TabOrder = 1
  end
  object LElon: TEdit
    Left = 456
    Top = 103
    Width = 89
    Height = 21
    TabOrder = 2
  end
  object HElat: TEdit
    Left = 168
    Top = 143
    Width = 89
    Height = 21
    TabOrder = 3
  end
  object HElon: TEdit
    Left = 456
    Top = 143
    Width = 89
    Height = 21
    TabOrder = 4
  end
  object HElatns: TComboBox
    Left = 273
    Top = 143
    Width = 33
    Height = 21
    ItemIndex = 0
    TabOrder = 5
    Text = 'N'
    Items.Strings = (
      'N'
      'S')
  end
  object LElatns: TComboBox
    Left = 273
    Top = 103
    Width = 33
    Height = 21
    ItemIndex = 0
    TabOrder = 6
    Text = 'N'
    Items.Strings = (
      'N'
      'S')
  end
  object LElonew: TComboBox
    Left = 567
    Top = 103
    Width = 33
    Height = 21
    ItemIndex = 0
    TabOrder = 7
    Text = 'E'
    Items.Strings = (
      'E'
      'W')
  end
  object HElonew: TComboBox
    Left = 567
    Top = 143
    Width = 33
    Height = 21
    ItemIndex = 0
    TabOrder = 8
    Text = 'E'
    Items.Strings = (
      'E'
      'W')
  end
  object SubmitBtn: TButton
    Left = 271
    Top = 224
    Width = 75
    Height = 25
    Caption = 'Submit'
    TabOrder = 9
    OnClick = SubmitBtnClick
  end
  object Rwidth: TEdit
    Left = 103
    Top = 183
    Width = 41
    Height = 21
    TabOrder = 10
  end
  object AptName: TEdit
    Left = 256
    Top = 183
    Width = 161
    Height = 21
    TabOrder = 11
  end
  object icao: TEdit
    Left = 559
    Top = 183
    Width = 41
    Height = 21
    TabOrder = 12
  end
end
