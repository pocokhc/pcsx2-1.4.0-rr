# pcsx2-1.4.0-rr
[pcsx2-1.4.0](https://github.com/PCSX2/pcsx2)にTAS作成のための機能を追加した非公式エミュレータです。  
pcsx2-rr(<https://code.google.com/archive/p/pcsx2-rr/>)を参考にしています。  
ただ、だいぶ中身が変わっており単純なマージはできなかったのでかなりソースコードは変わっています。  
  

# TAS
実際に作ってみたTASはこちらの動画となります。  
[【TAS】チュウリップ part0(WIP)【pcsx2-1.4.0-rr】](http://www.nicovideo.jp/watch/sm30385451)  
  

# ダウンロード
windows10のみビルド後の実行ファイルを置いておきます。  
それ以外のOSは未確認です。一応公式で対応しているOSではビルドして使えるはずです。  
[pcsx2-1.4.0-rr](https://github.com/pocokhc/pcsx2-1.4.0-rr/releases)  
  
動作させるために以下が必要です。(公式から引用)  
You need the Visual C++ 2015 x86 Redistributable for this version to work.  
Download it [HERE](https://www.microsoft.com/en-us/download/details.aspx?id=48145)  
  
実行環境は公式と同じです。  
一応 [windows10](https://www.microsoft.com/ja-jp/software-download/windows10)をVM上で新規にインストールした状態で起動できたことを確認しています。  

# 追加機能
* メニューバー
	* Movie

||機能|
|---|---|
|NewRecord|Recordモードでファイルを開きます。ファイルの中身は初期化されます。|
|Play|Replayモードでファイルを開きます。|
|Stop|ファイルを閉じます。ファイルクローズをするので終了したら呼んでください。|
|Convert(p2m->p2m2)|旧pcsx2-rrのKeyMovieファイルを再生できるように変換します。まだうまく同期がとれていません|
|Convert(v1.0~v1.2->v2.0)|KeyMovieフォーマットを変更したのでそれに合わせて古いバージョンのKeyMovieフォーマットが再生できるように変換します。|
|Open KeyEditor Window...|KeyEditorのウィンドウを開きます。|
※Record,PlayともにStop状態で始まります。Pキーを押して再生してください。  
※[KeyEditor](https://github.com/pocokhc/pcsx2-1.4.0-rr/wiki)に関してはwikiへ  

* ショートカットキー

|ボタン|機能|
|---|---|
|P|pause/unpauseの切り替え|
|space|frame advance|
|R|キームービーの再生モードと録画モードの切り替え|

# ショートカットキーに関して
キー配置は下記ファイルをいじってください。  
./inis_1.4.0/PCSX2-rr_keys.ini  
ただし、iniファイル内の説明にある通り、  
F5,F7,PAGE_UP,DELETE,HOME,INSERT キーは使用しない方がいいです。  


# キームービー作成想定手順
1. ゲームを始める前にRecord/Playを実行します。  
2. Boot CDVDからゲームを起動します。  
※(full)と(fast)ですが、fastで起動したキーファイルはfast専用になりそう（未検証）  
　同じ起動方法で起動する方がいいと思います。  
3. 停止していますので、Pキーで動かします。  
4. 作成を再開したい場合は、Playで始めて作成を再開した場所までいき、そこでRキーを押します。  
5. 以下キーを使って作成を進めます。

 |ボタン|機能|
 |---|---|
 |P    |pause/unpause|
 |R    |キームービーのrecord/replay 切り替え|
 |space|frame advance|
 |F1       |QS|
 |F2       |toggle save slot(+1)|
 |SHIFT+F2 |toggle save slot(-1)|
 |F3       |QL|
※その他ショートカットキーはpscx2公式ページを[参照](http://wiki.pcsx2.net/index.php/Hotkeys)  
※saveの注意点  
saveした後すぐにsaveを行うとうまくいかないときがあります。consoleでsaveできたことを確認したほうがいいです。  
  
##tips
キーファイルはframeが基準となり読み込み/上書きをします。  
ですので、Rキーでreplayモードに切り替えて一定フレーム進ませた後、Rキーでrecordモードにして記録するということが出来ます。  
  

# 録画想定手順
1. ゲームを始める前にPlayで始めます。
2. ゲームを始めたら止まっていると思いますので、F12キーで録画を開始します。  
※録画ファイルの保存場所は権限のある場所にしてください(windows10だとCドライブ以下は権限がないのでホームディレクトリ推奨)  
※権限がない等で保存されない場合でも警告等ありませんのでファイルができているか確認したほうがいいです。  
※音ファイルは別にpcsx2のexeファイルと同じディレクトリに「recording.wav」で作成されます。  
※音ファイルと映像ファイルを同期させるために録画中はPauseやQL,QSはしない方がいい気がします。  
3. Pキーでunpauseし、録画する。  
  

# ビルド方法(windows10)
  
公式と同じビルド方法です。  
  
用意するもの  
* Visual Studio Comunnity 2015(<https://www.visualstudio.com/vs/community/>)  
※デフォルト設定だとC++が足りないので、  
  カスタム→プログラミング言語→Visual C++にチェックを入れてインストールしてください。  
* DirectX Software Development Kit (June 2010)(<https://www.microsoft.com/en-us/download/details.aspx?id=6812>)  
※デフォルト設定のインストールでおｋです。追加で.NET Framework3.5が必要なので追加しますか？と聞かれる場合はインストールしておきます。  
  
あとは、ソースコード内の「PCSX2_suite.sln」が開けると思うので開いてビルドでいけるはずです。  
  
[windows10](https://www.microsoft.com/ja-jp/software-download/windows10)をVM上で新規にインストールした状態でビルドできたことを確認しています。  


# ソースコードの変更箇所
元のコードから変更している箇所にはすべてコメントで  
//--TAS--//  
を入れてあります。これで検索をかければわかると思います。  



# その他(使う側)
* キーファイルですが、pcsx2のp2mファイルとは互換性がありません。  
変換機能を付けましたが、同期ずれが激しく多分使い物になりません…  

* ショートカットキーのキーコンフィグがしたい！  
inis_1.4.0/PCSX2-rr_keys.ini  
を編集してください。  

* ゲーム画面でキーが反応しない時  
条件は不明ですが、一定時間何もしていないとなる時があります。  
その場合は、メインメニューにフォーカスをあててからゲーム画面にフォーカスしなおすとなおります。  
  
* ゲーム画面だけフリーズした時  
ゲーム起動中でpause中にコントローラの設定などメインフレームを動かすと固まる場合があります。  
その時はシステムのメニューから中断→再開とするとなおります。  
  
* pscx2事態がフリーズする  
滅多にないと思いますが、もしあったら再起動です…  

* desyncは解決された？  
多分されたと信じたいです。  
キーファイルは完全に内部のframe単位で管理するようにしているので、Emu側でどれだけframe管理ができているか次第です。  
現状はdesyncは確認していません。  
  

* windowタイトルのframe数の更新が遅い  
一定周期毎に更新されるようです。  
KeyEditorのタイトルがリアルタイムで更新されますのでそちらをご確認ください。  
  

# その他(作成側)
* スペース押しっぱなしで普通に再生できない？  
現状出来ないです。実現の仕方がよくわかっていません。  

* Lua機能の追加に関して  
時間がかかりそうなので今のところ見送りです。  
参考になるものがあればいいのですが…  

* GUIに関して  
wxWidgetsというものを使っていました。  
wxWidgetsの知識が全然ないのであまり作りこめていません。  
作れればキー情報を録画中に操作できるような機能とかいいかもしれませんね。  

* (PCSX2の)最新版でつくれる？  
元の構造は極力いじっていない(つもり)ので、add TAS codeの箇所を適用すればPCSX2の最新版にも適用可能だと思います。  

# その他
githubは初心者であまり分かっていません。何か不備や問題があれば連絡ください。  
あと、正月で突貫で作ったのであまりいいコードではないと思います。これも何かあれば連絡ください。  
あと英語はほとんどできなく、PCSX2の情報を調べるのに苦労しています…  
連絡用：pocopococpp198@gmail.com  


------------------------
**Licence**  
公式のライセンス通りです。  
[License GNU LGPL](http://www.gnu.org/licenses/lgpl.html)  

