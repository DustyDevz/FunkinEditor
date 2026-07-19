//FunkinC++ Engine
//Copyright (©) 2026 Dusty. All rights reserved.
//This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
//For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.

// this installer is just temp for now
// in the future if i ever have time ill rewrite this from scratch with a better compression and ui in c++
// inno is very good but i like having more control on stuff i make

#ifndef AppVersion
  #define AppVersion "0.0.1"
#endif
#define AppId "C5F6E4C0-23C3-49AD-9E63-5125C281F0CF"

[Setup]
AppId={#AppId}
AppName=FunkinEditor
AppVersion={#AppVersion}
AppPublisher=Dusty
AppCopyright=Copyright (C) 2026 Dusty
DefaultDirName={localappdata}\Programs\FunkinEditor
DefaultGroupName=FunkinEditor
OutputDir=.\installer_output
OutputBaseFilename=FunkinSetup
Compression=lzma2/ultra64
SolidCompression=yes
LZMAUseSeparateProcess=yes
LZMADictionarySize=1048576
LZMANumFastBytes=273
LZMANumBlockThreads=8
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
SetupAppTitle=FunkinEditor - Setup
SetupWindowTitle=FunkinEditor - Setup

[Tasks]
Name: "desktopicon"; Description: "Create a Desktop shortcut"; GroupDescription: "{cm:AdditionalIcons}"; Check: IsInstallOrRepairSelected
Name: "startmenuicon"; Description: "Create a Start Menu shortcut"; GroupDescription: "{cm:AdditionalIcons}"; Check: IsInstallOrRepairSelected

[Files]
Source: "..\assets\images\icon.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\export\Release\FunkinEditor.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\export\Release\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\export\Release\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\FunkinEditor"; Filename: "{app}\FunkinEditor.exe"; IconFilename: "{app}\icon.ico"; Tasks: startmenuicon
Name: "{autodesktop}\FunkinEditor"; Filename: "{app}\FunkinEditor.exe"; Tasks: desktopicon; IconFilename: "{app}\icon.ico"

[Run]
Filename: "{app}\FunkinEditor.exe"; Description: "{cm:LaunchProgram,FunkinEditor}"; Flags: nowait postinstall skipifsilent; Check: IsInstallOrRepairSelected

[UninstallDelete]
Type: filesandordirs; Name: "{app}"

[Code]
var
  OptionPage, VCRedistPage: TWizardPage;
  InstallRadio, RemoveRadio, RepairRadio: TRadioButton;
  CopyrightLabel: TLabel;
  LicenseCheck: TNewCheckBox;

  AllowSilentExit: Boolean;
  VCRedistInstallClicked: Boolean;

  RequiredSpaceLabel, AvailableSpaceLabel, StatusLabel: TLabel;
  WarningLabel: TLabel;
  
function IsMaintenanceMode: Boolean;
var
  FoundHKCU, FoundHKLM: Boolean;
begin
  FoundHKCU := RegKeyExists(HKCU, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#AppId}_is1');
  FoundHKLM := RegKeyExists(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#AppId}_is1');
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

function FormatBytes(Bytes: Int64): String;
var
  GBVal: Extended;
begin
  if Bytes >= (1024 * 1024 * 1024) then
  begin
    GBVal := Bytes / (1024 * 1024 * 1024);
    Result := Format('%.2f GB', [GBVal]);
  end
  else
  begin
    Result := Format('%d MB', [Bytes div (1024 * 1024)]);
  end;
end;
 
function GetFreeSpaceText: String;
var
  FreeBytes, TotalBytes: Int64;
  DriveRoot: String;
begin
  DriveRoot := ExtractFileDrive(ExpandConstant('{localappdata}')) + '\';
  if GetSpaceOnDisk64(DriveRoot, FreeBytes, TotalBytes) then
    Result := FormatBytes(FreeBytes) + ' free on ' + DriveRoot
  else
    Result := 'Unknown';
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

procedure OptionRadioClick(Sender: TObject);
begin
  RequiredSpaceLabel.Visible := InstallRadio.Checked;
  AvailableSpaceLabel.Visible := InstallRadio.Checked;
end;

procedure DownloadButtonClick(Sender: TObject);
var
  ResultCode: Integer;
begin
  ShellExec('open', 'https://aka.ms/vs/17/release/vc_redist.x64.exe', '', '', SW_SHOWNORMAL, ewNoWait, ResultCode);
end;

function ShowRedistNotDetectedDialog: Boolean;
var
  Frm: TForm;
  MsgLabel: TNewStaticText;
  DownloadBtn, OkBtn: TNewButton;
begin
  Frm := TForm.Create(nil);
  Frm.ClientWidth := ScaleX(380);
  Frm.ClientHeight := ScaleY(140);
  Frm.Caption := 'Redistributable Not Detected';
  Frm.Position := poScreenCenter;
  Frm.BorderStyle := bsDialog;
 
  MsgLabel := TNewStaticText.Create(Frm);
  MsgLabel.Parent := Frm;
  MsgLabel.Left := ScaleX(16);
  MsgLabel.Top := ScaleY(16);
  MsgLabel.Width := Frm.ClientWidth - ScaleX(32);
  MsgLabel.Height := ScaleY(70);
  MsgLabel.AutoSize := False;
  MsgLabel.WordWrap := True;
  MsgLabel.Caption :=
    'The Visual C++ Redistributable still isn''t detected.' + #13#10#13#10 +
    'If you haven''t downloaded it yet, click Download. Otherwise, click OK and try Check again once it finishes installing.';
 
  //BtnResult := 0;
 
  DownloadBtn := TNewButton.Create(Frm);
  DownloadBtn.Parent := Frm;
  DownloadBtn.Caption := 'Download';
  DownloadBtn.Width := ScaleX(100);
  DownloadBtn.Height := ScaleY(28);
  DownloadBtn.Left := Frm.ClientWidth - ScaleX(220);
  DownloadBtn.Top := Frm.ClientHeight - ScaleY(44);
  DownloadBtn.ModalResult := mrYes;
 
  OkBtn := TNewButton.Create(Frm);
  OkBtn.Parent := Frm;
  OkBtn.Caption := 'OK';
  OkBtn.Width := ScaleX(100);
  OkBtn.Height := ScaleY(28);
  OkBtn.Left := Frm.ClientWidth - ScaleX(110);
  OkBtn.Top := Frm.ClientHeight - ScaleY(44);
  OkBtn.ModalResult := mrOk;
 
  Frm.ActiveControl := OkBtn;
 
  Result := (Frm.ShowModal = mrYes);
  Frm.Free;
end;

procedure InitializeWizard;
var
  Maintenance: Boolean;
begin
  AllowSilentExit := False;
  VCRedistInstallClicked := False;
  CreateCopyrightLabel(WizardForm);
  
  StatusLabel := TLabel.Create(WizardForm);
  StatusLabel.Parent := WizardForm.InstallingPage;
  StatusLabel.Caption := 'i like cats'; 
  StatusLabel.Left := WizardForm.ProgressGauge.Left;
  StatusLabel.Top := WizardForm.ProgressGauge.Top + WizardForm.ProgressGauge.Height + ScaleY(8);
  StatusLabel.Width := WizardForm.ProgressGauge.Width;
  StatusLabel.Font.Color := clGray;

  WizardForm.LicenseAcceptedRadio.Visible := False;
  WizardForm.LicenseNotAcceptedRadio.Visible := False;
  WizardForm.LicenseMemo.Height := WizardForm.LicenseMemo.Height + ScaleY(20);
 
  LicenseCheck := TNewCheckBox.Create(WizardForm);
  LicenseCheck.Parent := WizardForm.LicensePage;
  LicenseCheck.Caption := 'I have read and accept the license agreement';
  LicenseCheck.Left := WizardForm.LicenseMemo.Left;
  LicenseCheck.Top := WizardForm.LicenseMemo.Top + WizardForm.LicenseMemo.Height + ScaleY(16);
  LicenseCheck.Width := WizardForm.LicenseMemo.Width;
  LicenseCheck.OnClick := @LicenseCheckClick;
  LicenseCheck.Font.Style := [fsBold];
  LicenseCheck.Font.Color := clWhite;

  WizardForm.NextButton.Enabled := False;
  OptionPage := CreateCustomPage(wpLicense, 'Setup Options', 'Select the action you want to perform.');
  Maintenance := IsMaintenanceMode;

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
  RemoveRadio.Enabled := Maintenance;

  RepairRadio := TRadioButton.Create(OptionPage);
  RepairRadio.Parent := OptionPage.Surface;
  RepairRadio.Caption := 'Repair';
  RepairRadio.Top := ScaleY(80);
  RepairRadio.Width := OptionPage.SurfaceWidth;
  RepairRadio.Enabled := Maintenance;

  InstallRadio.OnClick := @OptionRadioClick;
  RemoveRadio.OnClick := @OptionRadioClick;
  RepairRadio.OnClick := @OptionRadioClick;
 
  RequiredSpaceLabel := TLabel.Create(OptionPage);
  RequiredSpaceLabel.Parent := OptionPage.Surface;
  RequiredSpaceLabel.Caption := 'Space required: ' + WizardForm.DiskSpaceLabel.Caption;
  RequiredSpaceLabel.Top := ScaleY(120);
  RequiredSpaceLabel.Width := OptionPage.SurfaceWidth;
  RequiredSpaceLabel.Font.Color := clGray;
 
  AvailableSpaceLabel := TLabel.Create(OptionPage);
  AvailableSpaceLabel.Parent := OptionPage.Surface;
  AvailableSpaceLabel.Caption := 'Space available: ' + GetFreeSpaceText;
  AvailableSpaceLabel.Top := ScaleY(140);
  AvailableSpaceLabel.Width := OptionPage.SurfaceWidth;
  AvailableSpaceLabel.Font.Color := clGray;

  VCRedistPage := CreateCustomPage(OptionPage.ID, 'Prerequisite Required', '');
 
  WarningLabel := TLabel.Create(VCRedistPage);
  WarningLabel.Parent := VCRedistPage.Surface;
  WarningLabel.Caption := 'You need the Microsoft Visual C++ Redistributable.' + #13#10#13#10 +
    'Click Install to open the download page. Once it finishes installing, click Check to continue.';
    WarningLabel.Height := ScaleY(60);
  WarningLabel.AutoSize := False;
  WarningLabel.WordWrap := True;
  WarningLabel.Alignment := taCenter;
  WarningLabel.Width := VCRedistPage.SurfaceWidth;
  WarningLabel.Top := ScaleY(80);
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
 
  if CurPageID = VCRedistPage.ID then
  begin
    if VCRedistInstallClicked then
      WizardForm.NextButton.Caption := 'Check'
    else
      WizardForm.NextButton.Caption := 'Install';
  end
  else
    WizardForm.NextButton.Caption := SetupMessage(msgButtonNext);
end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin
  Result := False;

  if PageID = VCRedistPage.ID then
  begin
    if not IsInstallOrRepairSelected then
      Result := True
    else if not VCRedistNeedsInstall then
      Result := True;
  end;

  if IsMaintenanceMode and (PageID = wpSelectTasks) then
  begin
    if RemoveRadio.Checked or RepairRadio.Checked then
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
 
  if CurPageID = VCRedistPage.ID then
  begin
    if not VCRedistInstallClicked then
    begin
      DownloadButtonClick(nil);
      VCRedistInstallClicked := True;
      WizardForm.NextButton.Caption := 'Check';
      Result := False;
      Exit;
    end
    else
    begin
      if VCRedistNeedsInstall then
      begin
        if ShowRedistNotDetectedDialog then
          DownloadButtonClick(nil);
        Result := False;
        Exit;
      end
      else
      begin
        Result := True;
      end;
    end;
  end;
 
  if CurPageID = OptionPage.ID then
  begin
    if RemoveRadio.Checked then
    begin
      UninstallPath := '';
      if not RegQueryStringValue(HKCU, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#AppId}_is1', 'UninstallString', UninstallPath) then
      begin
        RegQueryStringValue(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{#AppId}_is1', 'UninstallString', UninstallPath);
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
      end;
    end;
  end;
end;