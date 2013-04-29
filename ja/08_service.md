## 第8章 サービスを起動する - service

ウェブサーバやデータベースサーバなどは、インストール後「サービス」として常に起動し、クライアントからのリクエストを受け付けるという使い方が普通でしょう。またその時、OS起動時にそれらサービスも自動的に起動するよう設定するでしょう。

くわえて、設定ファイルが変更されたり、あらたなプラグインをインストールした時など、それと同時にサービスをリスタートする必要があることもあるでしょう。

`service`はそのような、サービスに関するシステム状態を記述するためのresource typeです。

[http://docs.puppetlabs.com/references/latest/type.html#service](http://docs.puppetlabs.com/references/latest/type.html#file)

### Service

第5章で、nginxのサービス起動状態を記述するのに、以下のように書きました。

```
service { 'nginx':
  enable     => true,
  ensure     => running,
  hasrestart => true,
  require    => File['/etc/nginx/conf.d/my.conf'],
}
```

このように書くことで、

  * OS起動時に、nginxも自動的に起動する(`enable`)
  * nginxが常に起動している状態を保つよう保証する(`ensure`)

という、ふたつの状態を記述したのでした。

RHEL系OSの場合、

  * `enable`の状態は`chkconfig`コマンドで
  * `ensure`の状態は`service`コマンドで

実行されます。いずれも、みなさんが手動でサービスの起動状態を変更する時に慣れ親しんだコマンドでしょう。

`hasrestart`について補足しておきましょう。サービスをリスタートするための方法が、サービス(この場合はnginx)のinitスクリプトに用意されている場合は、上記のように`hasrestart => true`を指定しておきます。さもないと、Puppetはサービスの再起動を、stopとstartで代用してしまいます。

もし、initスクリプト以外のなんらかのコマンドやスクリプトなどでrestartしたい場合は、`restart => "..."`として、そのコマンド/スクリプトの文字列を指定します。

### `notify`と`subscribe`

いちどPuppetで構築したシステムも、時間が経つに連れ、その時々の要件に合わせて変化していきます。たとえば、今回のnginxの例でいうと、必要に応じて設定を変更するような場合です。

nginxの設定に変更を行った場合、その変更を反映させるため、再起動などが必要になるでしょう。第4章で書いた設定ファイルに関する記述は、以下のようになっていました。

```
file { '/etc/nginx/conf.d/my.conf':
  ensure  => present,
  owner   => 'root',
  group   => 'root',
  mode    => '0644',
  content => template('my.conf'),
  require => Package['nginx'],
  notify  => Service['nginx'],
}
```

ここで重要なのが`notify`の箇所です。もし、この設定ファイル`/etc/nginx/conf.d/my.conf`の状態に変更があった場合、`Service['nginx']`へrefresh eventが通知されます。そのイベントを受け取った`Service['nginx']`は、この場合、`/sbin/service nginx restart`を実行して、nginxを再起動します。

今回は`notify`を使いましたが、`subscribe`を使って、逆方向に関係を記述することもできます。以下のように、通知をする側ではなく通知を受け取りたい側で、`subscribe`により対象のresourceを指定します。

```
service { 'nginx':
  enable     => true,
  ensure     => running,
  hasrestart => true,
  require    => File['/etc/nginx/conf.d/my.conf'],
  subscribe  => File['/etc/nginx/conf.d/my.conf'], #=> ここを追加
}
```

`notify`でも`subscribe`でも、結果はいずれも同じです。その時々に応じて、わかりやすい方を適宜使うとよいでしょう。

### まとめ

本章では以下のことを学びました。

  * サービスの起動状態を記述するresource typeである`service`
  * relationshipを`notify`とは逆方向から指定する`subscribe`

ちなみに、refresh relationshipは、`service`だけに適用されるものではありません。後述の、任意のコマンドを実行する`exec`にも適用されます。
