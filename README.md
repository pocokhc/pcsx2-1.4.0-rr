# pcsx2-1.4.0-rr
[pcsx2-1.4.0](https://github.com/PCSX2/pcsx2)にTAS作成のための機能を追加した非公式エミュレータです。  
pcsx2-rr(<https://code.google.com/archive/p/pcsx2-rr/>)を参考にしています。  
ただ、だいぶ中身が変わっており単純なマージはできなかったのでかなりソースコードは変わっています。  
  

# TAS
実際に作ってみたTASはこちらの動画となります。(作成は本ツールv1.0の時のです)  
[【TAS】チュウリップ part0(WIP)【pcsx2-1.4.0-rr】](http://www.nicovideo.jp/watch/sm30385451)  
  

# ダウンロード
windows10のみビルド後の実行ファイルを置いておきます。  
[pcsx2-1.4.0-rr](https://github.com/pocokhc/pcsx2-1.4.0-rr/releases)  
windows10以外のOSは未確認です。一応公式で対応しているOSではビルドして使えるはずです。  
  
動作させるために以下が必要です。(公式から引用)  
You need the Visual C++ 2015 x86 Redistributable for this version to work.  
Download it [HERE](https://www.microsoft.com/en-us/download/details.aspx?id=48145)  
  
実行環境は公式と同じです。  
一応 [windows10](https://www.microsoft.com/ja-jp/software-download/windows10)をVM上で新規にインストールした状態で起動できたことを確認しています。  

# 追加機能の概要  
* KeyMovie機能  
* Pause/Unpase/FrameAdvance機能  
* Lua  
* KeyMovieのEditor機能  
  
各機能の詳細は[wiki](/wiki)をご覧ください。  



# ビルド方法について(windows10)
公式と同じビルド方法です。  
  
用意するもの  
1. Visual Studio Comunnity 2015(<https://www.visualstudio.com/vs/community/>)  
※デフォルト設定だとC++が足りないのでカスタム→プログラミング言語→Visual C++にチェックを入れてインストールしてください。  
2. DirectX Software Development Kit (June 2010)(<https://www.microsoft.com/en-us/download/details.aspx?id=6812>)  
※デフォルト設定のインストールでおｋです。追加で.NET Framework3.5が必要なので追加しますか？と聞かれる場合はインストールしておきます。  
3. ソースコード内の「PCSX2_suite.sln」を開き、ビルドをする。
  
[windows10](https://www.microsoft.com/ja-jp/software-download/windows10)をVM上で新規にインストールした状態でビルドできたことを確認しています。  


# ソースコードの変更箇所
元のコードから変更している箇所にはすべてコメントで  
//--TAS--//  
//--LuaEngine--//  
//--TASKeyEditor--//  
を入れてあります。  


# コメント
ひとまずv3.0で開発は止めてTAS制作に移りたいと思います。  
TASを作成していくうえで気になった点やバグが出れば随時対応はしていく予定です。  
特にLuaはあまりテストしていないので…、バグが出ればLuaの使用を控えてもらえれば(えっ  



------------------------
**Licence**  
公式のライセンス通りです。  
[License GNU LGPL](http://www.gnu.org/licenses/lgpl.html)  

