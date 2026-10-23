; Installs into %ProgramData%\obs-studio\plugins. Portable OBS does not load
; plugins from there, so portable installs use the zip instead.

#ifndef ProjectRoot
  #error ProjectRoot must be defined (/DProjectRoot=...)
#endif
#ifndef Version
  #define Version "0.0.0"
#endif
#ifndef Config
  #define Config "RelWithDebInfo"
#endif

[Setup]
AppId={{45b52426-e40e-4f2f-9587-efb9af422c60}
AppName=OBS Focus Filter
AppVersion={#Version}
AppPublisher=ycookiey
AppPublisherURL=https://github.com/ycookiey/obs-focus-filter
AppSupportURL=https://github.com/ycookiey/obs-focus-filter/issues
DefaultDirName={commonappdata}\obs-studio\plugins\obs-focus-filter
DisableDirPage=yes
DisableProgramGroupPage=yes
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog commandline
OutputDir={#ProjectRoot}\release
OutputBaseFilename=obs-focus-filter-{#Version}-windows-x64-Installer
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
Compression=lzma
SolidCompression=yes
LicenseFile={#ProjectRoot}\LICENSE

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"

[Files]
Source: "{#ProjectRoot}\release\{#Config}\obs-focus-filter\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs
