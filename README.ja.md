[English](README.md)

# OBS Focus Filter

ウィンドウキャプチャを、キャプチャ対象のウィンドウにフォーカスがある間だけ表示するOBS Studioのフィルタ。Windows用。ほかのウィンドウが前面にある間はキャプチャが透明になり、下に重ねたソースが映る。

## 特徴

- ウィンドウキャプチャと、モードが「特定のウィンドウをキャプチャ」のゲームキャプチャに使える。フィルタはソースで選んだウィンドウにフォーカスがあるかで表示を切り替えるため、フィルタにはウィンドウを選ぶ設定が無い
- 複数のウィンドウを開くアプリ向けに、同じアプリのどのウィンドウにフォーカスがあっても表示する設定がある
- フェードイン・フェードアウトを付けられる

## インストール

64ビットのWindows 10以降と、64ビットのOBS Studioが必要。ReleasesのファイルはOBS Studio 31.1.1向けにビルドしている。

[Releases](https://github.com/ycookiey/obs-focus-filter/releases)から、次のどちらかをダウンロードする。

- インストーラー: `obs-focus-filter-<version>-windows-x64-Installer.exe`を実行すると、`%ProgramData%\obs-studio\plugins\obs-focus-filter`にインストールされる
- zip: `obs-focus-filter-<version>-windows-x64.zip`の中の`obs-focus-filter`フォルダを、`%ProgramData%\obs-studio\plugins\`に展開する

ポータブル版のOBSは`%ProgramData%`のプラグインを読み込まない。ポータブル版のOBSにはzipを使い、zipの中の`obs-focus-filter\bin\64bit\obs-focus-filter.dll`をOBSのフォルダの`obs-plugins\64bit\`に、`obs-focus-filter\data\`の中身を`data\obs-plugins\obs-focus-filter\`にコピーする。

## 使い方

1. ウィンドウキャプチャのソースを右クリックし、「フィルタ」を選択する
2. 「エフェクトフィルタ」の「+」を押し、「フォーカス中のみ表示」を選択する

| 設定 | 値 | 既定値 |
|---|---|---|
| 表示する条件 | 「キャプチャ対象のウィンドウがフォーカス中」: キャプチャ対象のウィンドウか、キャプチャ対象のウィンドウから開いたダイアログが前面にある間だけ表示する<br>「同じアプリのウィンドウがフォーカス中」: 同じプロセスのウィンドウが前面にある間だけ表示する | 「キャプチャ対象のウィンドウがフォーカス中」 |
| フェード時間 | 0〜2000ms。0のときはフェードせずに切り替わる | 0 |

## 既知の制限

- フィルタは前面のウィンドウを0.1秒ごとに確認するため、フォーカスが移ってから表示か非表示に切り替わるまで、最大で約0.1秒かかる
- アプリを閉じた後など、キャプチャ対象のウィンドウが存在しない間は、キャプチャを表示しない
- 画面キャプチャなど、ウィンドウを選んでいないソースでは、フィルタは何もしない
- ゲームキャプチャは、「特定のウィンドウをキャプチャ」のモードで最後に選んだウィンドウを設定に残す。モードを「フルスクリーンアプリケーションをキャプチャ」か「ホットキーで前面のウィンドウをキャプチャ」に変えても、設定に残ったウィンドウは消えない。フィルタは、設定に残ったウィンドウにフォーカスがあるかで表示を切り替える

## アンインストール

- インストーラーで入れた場合: Windowsの設定の「インストールされているアプリ」で、OBS Focus Filterをアンインストールする
- zipで入れた場合: `%ProgramData%\obs-studio\plugins\obs-focus-filter`を削除する
- ポータブル版のOBSに入れた場合: OBSのフォルダの`obs-plugins\64bit\obs-focus-filter.dll`と`data\obs-plugins\obs-focus-filter\`を削除する

## ビルド

CMake 3.28以降と、Visual Studio 2026、Windows SDK 10.0.26100が必要。`windows-x64`のプリセットがVisual Studio 2026とWindows SDK 10.0.26100を指定している。最初の`cmake --preset`の実行で、OBS Studioのソースとビルド済みの依存ライブラリ、合わせて約320MBを`.deps`にダウンロードし、libobsをビルドする。

```powershell
cmake --preset windows-x64
cmake --build --preset windows-x64
cmake --install build_x64 --prefix release --config RelWithDebInfo
```

最後のコマンドで、zipと同じ構成のプラグインが`release\obs-focus-filter\`に出力される。

## ライセンス

GNU General Public License v2.0以降。詳細は[LICENSE](LICENSE)を参照。
