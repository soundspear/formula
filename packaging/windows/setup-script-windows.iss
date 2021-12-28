[Setup]
DisableDirPage=yes
ArchitecturesInstallIn64BitMode=x64
AppName=Soundspear {#AppName}
AppPublisherURL={#WebsiteUrl}
AppSupportURL={#WebsiteUrl}contact/
AppUpdatesURL={#WebsiteUrl}myaccount/
AppVerName={#AppName}
AppVersion={#AppVer}
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

[Icons]
Name: {group}\Uninstall Soundspear {#AppName}; Filename: {uninstallexe}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl" 

[Types]
#ifndef NoVst2
Name: "Full"; Description: "VST/VST3";
#else
Name: "Full"; Description: "VST3";
#endif

[Components]
Name: "VST364"; Description: "64-bit VST3"; Types: Full;
#ifndef NoVst2
Name: "VST64"; Description: "64-bit VST2"; Types: Full;
#endif

[Code]
var
  VST2DirPage: TInputDirWizardPage;
  TypesComboOnChangePrev: TNotifyEvent;
  DownloadPage: TDownloadWizardPage;

function OnDownloadProgress(const Url, FileName: String; const Progress, ProgressMax: Int64): Boolean;
begin
  if Progress = ProgressMax then
    Log(Format('Successfully downloaded file to {tmp}: %s', [FileName]));
  Result := True;
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

  DownloadPage := CreateDownloadPage('Downloading requirements', 'Formula needs to download and install LLVM', @OnDownloadProgress);
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

function NextButtonClick(CurPageID: Integer): Boolean;
var 
  FileName: string;
  ResultCode: Integer;
begin
  if CurPageID = wpLicense then 
  begin
    DownloadPage.Clear;
    DownloadPage.Add('https://github.com/llvm/llvm-project/releases/download/llvmorg-11.1.0/LLVM-11.1.0-win64.exe', 'llvm-11.exe', '');
    DownloadPage.Show;
    try
      try
        DownloadPage.Download;
		FileName := ExpandConstant('{tmp}\llvm-11.exe');
		Result := Exec(FileName, '', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);

		if not Result then
		begin
		  MsgBox('Cannot install required dependencies (LLVM 11)', mbError, MB_OK);
		end;
      except
        SuppressibleMsgBox(AddPeriod(GetExceptionMessage), mbCriticalError, MB_OK, IDOK);
		Result := False;
      end;
    finally
      DownloadPage.Hide;
    end;
  end else
    Result := True;
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