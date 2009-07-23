; 

[Setup]
AppName=Syntext @Serna@ @AppVersion@
AppVerName=Syntext @Serna@ @AppVersion@
AppVersion=@AppVersion@
UsePreviousAppDir=no
UsePreviousGroup=no
DefaultDirName={pf}\Syntext\@Serna@ @VersionId@
DefaultGroupName=Syntext\@Serna@ @VersionId@
PrivilegesRequired=admin

AppPublisher=Syntext (c)
AppPublisherURL=http://www.syntext.com
AppSupportURL=http://www.syntext.com
AppUpdatesURL=http://www.syntext.com

LicenseFile=@LicenseFile@

DisableStartupPrompt=yes
UninstallDisplayIcon={app}\bin\serna.exe
ChangesAssociations=yes
SolidCompression=yes

[Tasks]
Name: desktopicon; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; MinVersion: 4,4
Name: associate; Description: "Associate files with @Serna@ application"; GroupDescription: "Other tasks:"; Flags: exclusive
Name: associate\xml; Description: ".&xml"; GroupDescription: "Other tasks:"; Flags: checkedonce
Name: associate\dita; Description: ".&dita"; GroupDescription: "Other tasks:"; Flags: checkedonce
Name: associate\ditamap; Description: ".&ditamap"; GroupDescription: "Other tasks:"; Flags: checkedonce

[Icons]
Name: "{group}\@Serna@ @VersionId@"; Filename: "{app}\bin\serna.exe"

Name: "{userdesktop}\Syntext @Serna@ @VersionId@"; Filename: "{app}\bin\serna.exe"; MinVersion: 4,4; Tasks: desktopicon

Name: "{group}\Uninstall @Serna@ @AppVersion@"; Filename: "{uninstallexe}"

[Registry]
Root: HKLM; Subkey: "Software\Syntext"; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "Software\Syntext\@Serna@\@VersionId@"; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "Software\Syntext\@Serna@\@VersionId@"; ValueType: string; ValueName: "serna_data_dir"; ValueData: "{app}"; Flags: uninsdeletekey;
Root: HKCR; Subkey: ".xml"; ValueType: string; ValueName: ""; ValueData: "@Serna@"; Flags: uninsdeletevalue; Tasks: associate\xml;
Root: HKCR; Subkey: ".dita"; ValueType: string; ValueName: ""; ValueData: "@Serna@"; Flags: uninsdeletevalue; Tasks: associate\dita;
Root: HKCR; Subkey: ".ditamap"; ValueType: string; ValueName: ""; ValueData: "Serna"; Flags: uninsdeletevalue; Tasks: associate\ditamap;
Root: HKCR; Subkey: "@Serna@"; ValueType: string; ValueName: ""; ValueData: "Serna XML File"; Flags: uninsdeletekey; Tasks: associate;
Root: HKCR; Subkey: "@Serna@\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\bin\serna.exe,0"; Tasks: associate;
Root: HKCR; Subkey: "@Serna@\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\serna.exe"" ""%1"""; Tasks: associate;
Root: HKCR; Subkey: "Applications\serna.exe"; ValueType: string; ValueName: "FriendlyAppName"; ValueData: "@Serna@"; Flags: uninsdeletekey;
Root: HKCR; Subkey: "Applications\serna.exe\shell"; ValueType: string; ValueName: "FriendlyCache"; ValueData: "@Serna@";
Root: HKCR; Subkey: "Applications\serna.exe\shell\open"; ValueType: string; ValueName: "FriendlyAppName"; ValueData: "Syntext @Serna@ @AppVersion@";

[Run]
Filename: "{app}\bin\serna.exe"; Description: "Launch @Serna@"; Flags: nowait postinstall skipifsilent

[Code]

function InitializeSetup(): Boolean;
begin
  if RegValueExists(HKLM, 'Software\Syntext\@Serna@\@VersionId@', 'serna_data_dir') then begin
    Result := MsgBox('Looks like Syntext @Serna@ @VersionId@ is already installed. It is recommended to uninstall previous version.' #13#13 'Exit setup?', mbConfirmation, MB_YESNO) = idNo;
  end else
    Result := True;
  end;
end.
