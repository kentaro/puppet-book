
= サービスを起動する - service


ウェブサーバやデータベースサーバなどは、インストール後「サービス」として常に起動し、クライアントからのリクエストを受け付けるという使い方が普通でしょう。またその時、OS起動時にそれらサービスも自動的に起動するよう設定するでしょう。


くわえて、設定ファイルが変更されたり、あらたなプラグインをインストールした時など、それと同時にサービスをリスタートする必要があることもあるでしょう。


@<tt>{service}はそのような、サービスに関するシステム状態を記述するためのresource typeです。


@<href>{http://docs.puppetlabs.com/references/latest/type.html#file,http://docs.puppetlabs.com/references/latest/type.html#service}

== Service


第5章で、nginxのサービス起動状態を記述するのに、以下のように書きました。

//emlist{
service { 'nginx':
  enable     => true,
  ensure     => running,
  hasrestart => true,
  require    => File['/etc/nginx/conf.d/my.conf'],
}
//}


このように書くことで、

 * OS起動時に、nginxも自動的に起動する(@<tt>{enable})
 * nginxが常に起動している状態を保つよう保証する(@<tt>{ensure})



という、ふたつの状態を記述したのでした。


RHEL系OSの場合、

 * @<tt>{enable}の状態は@<tt>{chkconfig}コマンドで
 * @<tt>{ensure}の状態は@<tt>{service}コマンドで



実行されます。いずれも、みなさんが手動でサービスの起動状態を変更する時に慣れ親しんだコマンドでしょう。


@<tt>{hasrestart}について補足しておきましょう。サービスをリスタートするための方法が、サービス(この場合はnginx)のinitスクリプトに用意されている場合は、上記のように@<tt>{hasrestart => true}を指定しておきます。さもないと、Puppetはサービスの再起動を、stopとstartで代用してしまいます。


もし、initスクリプト以外のなんらかのコマンドやスクリプトなどでrestartしたい場合は、@<tt>{restart => "..."}として、そのコマンド/スクリプトの文字列を指定します。

== @<tt>{notify}と@<tt>{subscribe}


いちどPuppetで構築したシステムも、時間が経つに連れ、その時々の要件に合わせて変化していきます。たとえば、今回のnginxの例でいうと、必要に応じて設定を変更するような場合です。


nginxの設定に変更を行った場合、その変更を反映させるため、再起動などが必要になるでしょう。第4章で書いた設定ファイルに関する記述は、以下のようになっていました。

//emlist{
file { '/etc/nginx/conf.d/my.conf':
  ensure  => present,
  owner   => 'root',
  group   => 'root',
  mode    => '0644',
  content => template('my.conf'),
  require => Package['nginx'],
  notify  => Service['nginx'],
}
//}


ここで重要なのが@<tt>{notify}の箇所です。もし、この設定ファイル@<tt>{/etc/nginx/conf.d/my.conf}の状態に変更があった場合、@<tt>{Service['nginx']}へrefresh eventが通知されます。そのイベントを受け取った@<tt>{Service['nginx']}は、この場合、@<tt>{/sbin/service nginx restart}を実行して、nginxを再起動します。


今回は@<tt>{notify}を使いましたが、@<tt>{subscribe}を使って、逆方向に関係を記述することもできます。以下のように、通知をする側ではなく通知を受け取りたい側で、@<tt>{subscribe}により対象のresourceを指定します。

//emlist{
service { 'nginx':
  enable     => true,
  ensure     => running,
  hasrestart => true,
  require    => File['/etc/nginx/conf.d/my.conf'],
  subscribe  => File['/etc/nginx/conf.d/my.conf'], #=> ここを追加
}
//}


@<tt>{notify}でも@<tt>{subscribe}でも、結果はいずれも同じです。その時々に応じて、わかりやすい方を適宜使うとよいでしょう。

== まとめ


本章では以下のことを学びました。

 * サービスの起動状態を記述するresource typeである@<tt>{service}
 * relationshipを@<tt>{notify}とは逆方向から指定する@<tt>{subscribe}



ちなみに、refresh relationshipは、@<tt>{service}だけに適用されるものではありません。後述の、任意のコマンドを実行する@<tt>{exec}にも適用されます。
