# 入門Puppet

antipopがPuppet勉強のためにがんばって調べながら書く『入門Puppet(仮)の』epub/mobi本執筆用リポジトリです。このリポジトリが見える方々は、レビューや助言、執筆参加よろしくおねがいいたします(^o^)/

naoyaさんのChef本の構成をパクって章立てします。

## 目次

  1.  はじめに
  2.  Puppetとは何か？
  3.  Vagrantで試験環境を用意する
  3.  Hello, Puppet!
  4.  nginx を Puppet で立ち上げたい
  5.  Puppet の試験環境を3分で用意する ─ Vagrant
  6.  レシピを作って実行する流れをおさらいしたい
  7.  サードパーティの Puppet クックブックを使いたい
  8.  代表的なレシピのサンプルを見たい ─ xxx のレシピを読む
  9.  パッケージをインストールする ─ package
  10. サービスを起動したい ─ service と subscribe/notify
  11. テンプレートから設定ファイルを配置したい ─ Puppet Template
  12. ファイルやディレクトリを扱いたい ─ file, directory
  13. ユーザーを作成したい ─ user, group
  14. 任意のシェルスクリプトを実行したい ─ exec
  15. その他の Resource
  16. レシピ落ち穂拾い ─ class, module, ベストプラクティス
  17. Resource を自分で定義したい ─ Defined Types
  18. ノードやプラットフォームによって異なる変数を定義したい
  19. ノードを役割ごとにグルーピングして管理したい
  20. サードパーティのクックブックを Bundler 風に管理したい ─ librarian-puppet
  21. Puppet Server の様子を知りたい - 概要からセットアップまで
  22. どこまでを Puppet でやるべきか
  24. おわりに

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
