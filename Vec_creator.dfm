object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Form1'
  ClientHeight = 550
  ClientWidth = 820
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 488
    Top = 48
    Width = 273
    Height = 337
    Lines.Strings = (
      'Memo1')
    TabOrder = 0
  end
  object MainMenu1: TMainMenu
    Left = 24
    Top = 440
    object File1: TMenuItem
      Caption = 'File'
      object Openinputfile1: TMenuItem
        Caption = 'Open text input file'
        Enabled = False
        OnClick = Openinputfile1Click
      end
      object OpenVectordatafile1: TMenuItem
        Caption = 'Open Vector data file'
        Enabled = False
      end
      object Exit1: TMenuItem
        Caption = 'Exit'
        OnClick = Exit1Click
      end
    end
    object Vector1: TMenuItem
      Caption = 'Vector'
      object CreateShapefileset1: TMenuItem
        Caption = 'Create Shapefile set'
        OnClick = CreateShapefileset1Click
      end
      object CreateMapInfofile1: TMenuItem
        Caption = 'Create MapInfo file'
        OnClick = CreateMapInfofile1Click
      end
      object CreateDXFfile1: TMenuItem
        Caption = 'Create DXF file'
        OnClick = CreateDXFfile1Click
      end
    end
    object Runway1: TMenuItem
      Caption = 'Runway'
      object RunwayPolyFromAptDat1: TMenuItem
        Caption = 'Create runway polygon from Apt.Dat'
        OnClick = RunwayPolyFromAptDat1Click
      end
      object MakeRunwayPolygon1: TMenuItem
        Caption = 'Create runway polygon - manual input'
        OnClick = MakeRunwayPolygon1Click
      end
      object RunwayPolyFromFile: TMenuItem
        Caption = 'Create runway polygon from input file '
        Enabled = False
      end
    end
    object Area1: TMenuItem
      Caption = 'Area'
      object CreateAreaPolyFromKMLfile1: TMenuItem
        Caption = 'Create area poly from KML file'
        OnClick = CreateAreaPolyFromKMLfile1Click
      end
      object CreateAreaPolyFromFile1: TMenuItem
        Caption = 'Create area polygon from text file'
      end
      object Creategridforairportarea1: TMenuItem
        Caption = 'Create grid for airport area'
        OnClick = Creategridforairportarea1Click
      end
      object Creategridwalkfile1: TMenuItem
        Caption = 'Create grid walk file'
        OnClick = Creategridwalkfile1Click
      end
      object Extractdatafromwalktextfile1: TMenuItem
        Caption = 'Extract data from walk text file'
        OnClick = Extractdatafromwalktextfile1Click
      end
    end
    object DEMS1: TMenuItem
      Caption = 'DEMS'
      object ExtractDSFDEM1: TMenuItem
        Caption = 'Extract DSF DEM'
        OnClick = ExtractDSFDEM1Click
      end
      object PatchtileHGT1: TMenuItem
        Caption = 'Patch tile HGT file'
        OnClick = PatchtileHGT1Click
      end
      object PatchDSFDEM1: TMenuItem
        Caption = 'Patch DSF DEM'
        OnClick = PatchDSFDEM1Click
      end
      object PatchDSFBIL1: TMenuItem
        Caption = 'Patch DSF BIL'
        OnClick = PatchDSFBIL1Click
      end
    end
  end
  object OpenDialog1: TOpenDialog
    Left = 96
    Top = 448
  end
  object SaveDialog1: TSaveDialog
    Left = 160
    Top = 448
  end
end
