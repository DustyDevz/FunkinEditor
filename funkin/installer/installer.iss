#ifndef MyAppVersion
  #define MyAppVersion "0.0.1"
#endif
#define MyAppId "C5F6E4C0-23C3-49AD-9E63-5125C281F0CF"

[Setup]
AppId={#MyAppId}
AppName=FunkinEditor
AppVersion={#MyAppVersion}
AppPublisher=Dusty
AppCopyright=Copyright (C) 2026 Dusty
DefaultDirName={localappdata}\Programs\FunkinEditor
DefaultGroupName=FunkinEditor
OutputDir=.\installer_output
OutputBaseFilename=FunkinSetup
Compression=lzma2/max
SolidCompression=yes
LZMANumBlockThreads=8
LZMABlockSize=65536
ChangesAssociations=yes
DisableProgramGroupPage=yes
DisableDirPage=yes
PrivilegesRequired=lowest
SetupIconFile=..\assets\images\icon.ico
WizardStyle=modern dynamic includetitlebar
LicenseFile=..\..\LICENSE
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
UninstallDisplayIcon={app}\icon.ico

[Messages]
LicenseLabel3=

[Tasks]
Name: "desktopicon"; Description: "Create a Desktop shortcut"; GroupDescription: "{cm:AdditionalIcons}"; Check: IsInstallOrRepairSelected
Name: "startmenuicon"; Description: "Create a Start Menu shortcut"; GroupDescription: "{cm:AdditionalIcons}"; Check: IsInstallOrRepairSelected

[Files]
Source: "..\assets\images\icon.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\export\Release\FunkinEditor.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\export\Release\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\export\Release\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall; Check: IsInstallOrRepairSelected

[Icons]
Name: "{group}\FunkinEditor"; Filename: "{app}\FunkinEditor.exe"; IconFilename: "{app}\icon.ico"; Tasks: startmenuicon
Name: "{autodesktop}\FunkinEditor"; Filename: "{app}\FunkinEditor.exe"; Tasks: desktopicon; IconFilename: "{app}\icon.ico"

[Run]
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Installing Visual C++ Redistributable..."; Check: IsInstallOrRepairSelected and VCRedistNeedsInstall
Filename: "{app}\FunkinEditor.exe"; Description: "{cm:LaunchProgram,FunkinEditor}"; Flags: nowait postinstall skipifsilent; Check: IsInstallOrRepairSelected

[UninstallDelete]
Type: filesandordirs; Name: "{app}"

[Code]
var
  OptionPage: TWizardPage;
  InstallRadio, RemoveRadio, RepairRadio: TRadioButton;
  CopyrightLabel: TLabel;
  LicenseCheck: TNewCheckBox;
  AllowSilentExit: Boolean;

function IsMaintenanceMode: Boolean;
var
  FoundHKCU, FoundHKLM: Boolean;
begin
  FoundHKCU := RegKeyExists(HKCU, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppId}_is1');
  FoundHKLM := RegKeyExists(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppId}_is1');
  Result := FoundHKCU or FoundHKLM;
end;

function IsInstallOrRepairSelected: Boolean;
begin
  Result := True;
  if IsMaintenanceMode then
  begin
    Result := InstallRadio.Checked or RepairRadio.Checked;
  end;
end;

function VCRedistNeedsInstall: Boolean;
var
  Installed: Cardinal;
begin
  Result := True;
  if RegQueryDWordValue(HKLM64, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\X64', 'Installed', Installed) then
  begin
    Result := Installed <> 1;
  end;
end;

procedure CreateCopyrightLabel(ParentForm: TSetupForm);
begin
  CopyrightLabel := TLabel.Create(ParentForm);
  CopyrightLabel.Parent := ParentForm;
  CopyrightLabel.Caption := 'Copyright (©) 2026 Dusty';
  CopyrightLabel.Left := ScaleX(22);
  CopyrightLabel.Top := ParentForm.ClientHeight - ScaleY(40);
  CopyrightLabel.Font.Color := clGray;
end;

procedure LicenseCheckClick(Sender: TObject);
begin
  WizardForm.LicenseAcceptedRadio.Checked := LicenseCheck.Checked;
  WizardForm.LicenseNotAcceptedRadio.Checked := not LicenseCheck.Checked;
  WizardForm.NextButton.Enabled := LicenseCheck.Checked;
end;

procedure InitializeWizard;
begin
  AllowSilentExit := False;
  CreateCopyrightLabel(WizardForm);

  WizardForm.LicenseAcceptedRadio.Visible := False;
  WizardForm.LicenseNotAcceptedRadio.Visible := False;

  WizardForm.LicenseMemo.Top := WizardForm.LicenseMemo.Top + ScaleY(15);
  WizardForm.LicenseMemo.Height := WizardForm.LicenseMemo.Height + ScaleY(10);

  LicenseCheck := TNewCheckBox.Create(WizardForm);
  LicenseCheck.Parent := WizardForm.LicensePage;
  LicenseCheck.Caption := 'I have read and accept the license agreement';
  LicenseCheck.Left := WizardForm.LicenseMemo.Left;
  LicenseCheck.Top := WizardForm.LicenseMemo.Top + WizardForm.LicenseMemo.Height + ScaleY(12);
  LicenseCheck.Width := WizardForm.LicenseMemo.Width;
  LicenseCheck.OnClick := @LicenseCheckClick;

  WizardForm.NextButton.Enabled := False;

  OptionPage := CreateCustomPage(wpLicense, 'Setup Options', 'Select the action you want to perform.');

  InstallRadio := TRadioButton.Create(OptionPage);
  InstallRadio.Parent := OptionPage.Surface;
  InstallRadio.Caption := 'Install';
  InstallRadio.Top := ScaleY(20);
  InstallRadio.Width := OptionPage.SurfaceWidth;
  InstallRadio.Checked := True;

  RemoveRadio := TRadioButton.Create(OptionPage);
  RemoveRadio.Parent := OptionPage.Surface;
  RemoveRadio.Caption := 'Remove';
  RemoveRadio.Top := ScaleY(50);
  RemoveRadio.Width := OptionPage.SurfaceWidth;

  RepairRadio := TRadioButton.Create(OptionPage);
  RepairRadio.Parent := OptionPage.Surface;
  RepairRadio.Caption := 'Repair';
  RepairRadio.Top := ScaleY(80);
  RepairRadio.Width := OptionPage.SurfaceWidth;
end;

procedure InitializeUninstallProgressForm;
begin
  CreateCopyrightLabel(UninstallProgressForm);
end;

procedure CurPageChanged(CurPageID: Integer);
begin
  if CurPageID = wpLicense then
  begin
    WizardForm.LicenseAcceptedRadio.Checked := LicenseCheck.Checked;
    WizardForm.LicenseNotAcceptedRadio.Checked := not LicenseCheck.Checked;
    WizardForm.NextButton.Enabled := LicenseCheck.Checked;
  end;
end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin
  Result := False;
  if not IsMaintenanceMode then
  begin
    if PageID = OptionPage.ID then
      Result := True;
  end;

  if IsMaintenanceMode and (PageID = wpSelectTasks) then
  begin
    if RemoveRadio.Checked then
      Result := True;
  end;
  
  if IsMaintenanceMode and (PageID = wpSelectTasks) then
  begin
    if RepairRadio.Checked then
      Result := True;
  end;
end;

procedure CancelButtonClick(CurPageID: Integer; var Cancel, Confirm: Boolean);
begin
  if AllowSilentExit then
  begin
    Cancel := True;
    Confirm := False;
  end;
end;

function NextButtonClick(CurPageID: Integer): Boolean;
var
  ResultCode: Integer;
  UninstallPath: String;
begin
  Result := True;
  if CurPageID = OptionPage.ID then
  begin
    if RemoveRadio.Checked then
    begin
      UninstallPath := '';
      if not RegQueryStringValue(HKCU, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppId}_is1', 'UninstallString', UninstallPath) then
      begin
        RegQueryStringValue(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppId}_is1', 'UninstallString', UninstallPath);
      end;

      if UninstallPath <> '' then
      begin
        UninstallPath := RemoveQuotes(UninstallPath);
        if Exec(UninstallPath, '/SILENT', '', SW_SHOW, ewWaitUntilTerminated, ResultCode) then
        begin
          if ResultCode <> 0 then
            MsgBox('The uninstaller reported an error (code ' + IntToStr(ResultCode) + '). ' +
                   'Some files may not have been removed.', mbError, MB_OK);
          
          Result := False;
          AllowSilentExit := True;
          PostMessage(WizardForm.Handle, $0010, 0, 0); 
        end
        else
        begin
          MsgBox('Failed to launch the uninstaller. Please try running it manually.', mbCriticalError, MB_OK);
          Result := False;
        end;
      end
      else
      begin
        MsgBox('Could not find the uninstaller path in the registry.', mbCriticalError, MB_OK);
        Result := False;
      end;
    end;
  end;
end;