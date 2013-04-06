# 入門Puppet

antipopがPuppet勉強のためにがんばって調べながら書く『入門Puppet(仮)の』epub/mobi本執筆用リポジトリです。このリポジトリが見える方々は、レビューや助言、執筆参加よろしくおねがいいたします(^o^)/

naoyaさんのChef本の構成をパクって章立てします。

## 目次

  1.  はじめに
  2.  Puppetとは何か？
  3.  Vagrantで試験環境を用意する
  4.  Hello, Puppet!
  5.  nginxのmanifestを書く
  6.  パッケージをインストールしたい - package
  7.  yumリポジトリを登録したい - yumrepo
  8.  サービスを起動したい service
  9.  ファイルやディレクトリを扱いたい - file
  10. ユーザやグループを作成したい - user/group
  11. コマンドを実行したい - exec
  12. td-agentのmanifestを書く
  12. クラスを定義する
  13. モジュール
  14. サードパーティのモジュールを使う
  15. 独自のResouceを定義する
  16. Puppet言語のシンタクス
  17. サーバを役割ごとにグルーピングして管理したい
  18. リモートホストに対してPuppetを実行する
  19. ベストプラクティス
  20. どこまでをPuppetでやるべきか
  21. おわりに

## 事前準備

  1. [calibre](http://calibre-ebook.com/)をインストール
  2. 確認用にKindleアプリ([Mac版](http://www.amazon.com/gp/feature.html?ie=UTF8&docId=1000464931))を入れておくとよいでしょう
    * HTML, epubも出力するので、そちらで確認もできます。
  3. `bundle install`を実行

## 書き方

  1. `ja/*.md`に書く(連番で)

## 書籍ファイルの作り方

  1. `rake`と、引数なしで実行
  2. `open puppet-book.拡張子`して確認
    * HTML, epub, mobi版があります。

用意されているrakeタスクを見るには、`rake -T`を実行してください。

## Thanks to

naoyaさんはいいひとだな〜。

  * https://twitter.com/naoya_ito/status/312557007326572544
  * https://twitter.com/naoya_ito/status/312558894041923584
  * https://twitter.com/naoya_ito/status/312559368774221824
