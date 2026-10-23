[日本語](README.ja.md)

# OBS Focus Filter

An OBS Studio filter for Windows that shows a window capture only while the captured window is focused. While another window is in the foreground, the capture is transparent and the sources below it show through.

## Features

- Works on Window Capture, and on Game Capture in "Capture specific window" mode. The filter follows the window selected in the source, so there is no window to select in the filter.
- For apps with several windows, can show the capture while any window of the same app is focused.
- Optional fade in and out.

## Installation

Requires 64-bit Windows 10 or later and 64-bit OBS Studio. The releases are built against OBS Studio 31.1.1.

Download one of the following from [Releases](https://github.com/ycookiey/obs-focus-filter/releases):

- Installer (`obs-focus-filter-<version>-windows-x64-Installer.exe`): installs into `%ProgramData%\obs-studio\plugins\obs-focus-filter`.
- zip (`obs-focus-filter-<version>-windows-x64.zip`): extract the `obs-focus-filter` folder into `%ProgramData%\obs-studio\plugins\`.

Portable OBS does not load plugins from `%ProgramData%`. For portable OBS, use the zip: copy `obs-focus-filter\bin\64bit\obs-focus-filter.dll` from the zip into `obs-plugins\64bit\` in the OBS folder, and the contents of `obs-focus-filter\data\` into `data\obs-plugins\obs-focus-filter\`.

## Usage

1. Right-click a Window Capture source and choose Filters.
2. Under Effect Filters, click + and choose Show Only While Focused.

| Setting | Values | Default |
|---|---|---|
| Show while | "The captured window is focused": shows while the captured window, or a dialog the captured window owns, is in the foreground.<br>"Any window of the same app is focused": shows while any window of the same process is in the foreground. | "The captured window is focused" |
| Fade duration | 0 to 2000 ms. At 0 the capture switches without a fade. | 0 |

## Known limitations

- The filter checks the foreground window every 0.1 seconds, so the capture can appear or disappear up to about 0.1 seconds after the focus changes.
- While the captured window does not exist, for example after the app is closed, the capture stays hidden.
- On a source with no window selected, such as Display Capture, the filter does nothing.
- Game Capture in "Capture any fullscreen application" or "Capture foreground window with hotkey" mode keeps the window last selected in "Capture specific window" mode in its settings, and the filter follows that window.

## Uninstallation

- Installer: uninstall OBS Focus Filter from Installed apps in Windows Settings.
- zip: delete `%ProgramData%\obs-studio\plugins\obs-focus-filter`.
- Portable OBS: delete `obs-plugins\64bit\obs-focus-filter.dll` and `data\obs-plugins\obs-focus-filter\` in the OBS folder.

## Building

Requires CMake 3.28 or later, and Visual Studio 2026 with the Windows SDK 10.0.26100, which the `windows-x64` preset selects. The first configure downloads the OBS Studio sources and prebuilt dependencies (about 320 MB) into `.deps` and builds libobs.

```powershell
cmake --preset windows-x64
cmake --build --preset windows-x64
cmake --install build_x64 --prefix release --config RelWithDebInfo
```

The last command puts the plugin in `release\obs-focus-filter\`, in the same layout as the zip.

## License

GNU General Public License v2.0 or later. See [LICENSE](LICENSE).
