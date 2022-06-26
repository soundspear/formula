[Setup]
DisableDirPage=yes
ArchitecturesInstallIn64BitMode=x64
AppName=Soundspear {#AppName}
AppPublisherURL={#WebsiteUrl}
AppSupportURL={#WebsiteUrl}contact/
AppUpdatesURL={#WebsiteUrl}myaccount/
AppVerName={#AppName}
AppVersion={#AppVer}
ChangesEnvironment=yes
Compression=lzma2/ultra64
DefaultDirName={commonpf}\Soundspear\{#AppName}
DefaultGroupName={#AppName}
DisableReadyPage=true
DisableWelcomePage=yes
LanguageDetectionMethod=uilanguage
LicenseFile=../../LICENSE.txt
OutputBaseFilename={#OutputFileName}
SetupIconFile=setup.ico
ShowLanguageDialog=no
VersionInfoCompany=Soundspear
VersionInfoCopyright=Soundspear
VersionInfoDescription=Soundspear {#AppName}
VersionInfoProductName={#AppName}
VersionInfoProductVersion={#AppVer}
VersionInfoVersion={#AppVer}
WizardImageFile=setup-image.bmp
WizardImageStretch=true
WizardSmallImageFile=setup-image-small.bmp

[Files]
#ifndef NoVst2
Source: "{#Vst2File64}"; DestDir: {code:GetVST2Dir|0}; Components: VST64; Flags: ignoreversion
#endif
Source: "{#Vst3File64}"; DestDir: "{commoncf64}\VST3\"; Components: VST364; Flags: ignoreversion
Source: "{#StandaloneFile64}"; DestDir: "{app}"; Components: Standalone; Flags: ignoreversion   
Source: "tcc\*"; DestDir: "{commonpf64}\tcc\"; Flags: ignoreversion recursesubdirs

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Icons]
Name: {group}\Uninstall Soundspear {#AppName}; Filename: {uninstallexe}  
Name: "{autoprograms}\{#AppName}"; Filename: "{app}\Formula.exe"
Name: "{autodesktop}\{#AppName}"; Filename: "{app}\Formula.exe"; Tasks: desktopicon

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl" 

[Components]                                  
#ifndef NoVst2
Name: "VST64"; Description: "VST2";
#endif
Name: "VST364"; Description: "VST3";
Name: "Standalone"; Description: "Standalone application";

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; \
    ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{commonpf64}\tcc"; \
    Check: NeedsAddPath('{commonpf64}\tcc')

[Code]
var
  VST2DirPage: TInputDirWizardPage;
  TypesComboOnChangePrev: TNotifyEvent;

function NeedsAddPath(Param: string): boolean;
var
  OrigPath: string;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', OrigPath)
  then begin
    Result := True;
    exit;
  end;
  Result := Pos(';' + Param + ';', ';' + OrigPath + ';') = 0;
end;

procedure ComponentsListCheckChanges;
begin
  WizardForm.NextButton.Enabled := (WizardSelectedComponents(False) <> '');
end;

procedure ComponentsListClickCheck(Sender: TObject);
begin
  ComponentsListCheckChanges;
end;

procedure TypesComboOnChange(Sender: TObject);
begin
  TypesComboOnChangePrev(Sender);
  ComponentsListCheckChanges;
end;

procedure InitializeWizard;
begin

  WizardForm.ComponentsList.OnClickCheck := @ComponentsListClickCheck;
  TypesComboOnChangePrev := WizardForm.TypesCombo.OnChange;
  WizardForm.TypesCombo.OnChange := @TypesComboOnChange;

  VST2DirPage := CreateInputDirPage(wpSelectComponents,
  'Confirm VST2 Plugin Directory', '',
  'Select the folder in which setup should install the VST2 Plugin, then click Next.',
  False, '');

  VST2DirPage.Add('64-bit folder');
  VST2DirPage.Values[0] := GetPreviousData('VST64', ExpandConstant('{reg:HKLM\SOFTWARE\VST,VSTPluginsPath|{pf}\Steinberg\VSTPlugins}'));
end;

procedure CurPageChanged(CurPageID: Integer);
begin
  if CurPageID = VST2DirPage.ID then
  begin
    VST2DirPage.Buttons[0].Enabled := WizardIsComponentSelected('VST64');
    VST2DirPage.PromptLabels[0].Enabled := VST2DirPage.Buttons[0].Enabled;
    VST2DirPage.Edits[0].Enabled := VST2DirPage.Buttons[0].Enabled;
  end;

  if CurPageID = wpSelectComponents then
  begin
    ComponentsListCheckChanges;
  end;
end;


function ShouldSkipPage(PageID: Integer): Boolean;
begin
  if PageID = VST2DirPage.ID then
  begin
  #ifdef NoVst2
	Result := True
  #else
    If (not WizardIsComponentSelected('VST64')) then
      begin
        Result := True
      end;
  #endif
  end;
end;

function GetVST2Dir(Param: string): string;
begin
    Result := VST2DirPage.Values[StrToInt(Param)];
end;

procedure RegisterPreviousData(PreviousDataKey: Integer);
begin
  SetPreviousData(PreviousDataKey, 'VST64', VST2DirPage.Values[0]);
end;