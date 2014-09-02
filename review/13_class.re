
= resource typeのグルーピング - class


これまでの説明では、例として取り上げる題材が小さかったこともあって、manifestを上から下へと、ただそのまま羅列して書いてきました。小さな例ではそれでよいのですが、なにも策を講じなかった場合、manifestが肥大化し、保守性に問題が生じることは容易に想像がつくことでしょう。


そこで、本章ではresource typeを任意の単位にグルーピングするのに使われるclassについて見ていくことにしましょう。

== Puppetにおけるclassとは


「クラス」と聞くと、プログラミング言語に親しんでいる読者は、OOPにおけるクラスのようなものを想像することでしょう。Puppetの言語はプログラミング言語ではなく、これまで何度も述べている通り「システムのあるべき状態」を記述するためのものですから、そのような意味でのクラスではありません。


Puppetにおけるclassは、システムを構成する各種resourceをグルーピングし、より構造化された記述のために使われるものです。そのことにより、manifestの規模が大きくなってきても、保守性を損なうことなくシステム記述が可能となります。


Puppetのclassはプログラミング言語のクラスとは違うと述べましたが、このように、分類・整理のために使われるという意味では、似ているともいえるかもしれません。

== class化の粒度


我々は現在、td-agentのmanifestに取り組んでいるのでした。ここで、システムにおけるtd-agentのあるべき状態を考えてみましょう。たとえば、以下のように考えられます。

 1. システムにtd-agentをインストールする
 1. 設定ファイルを準備する
 1. td-agentが確実に起動するよう保証する



class化の粒度は、上記の項目それぞれに対応させておく程度でよいでしょう。


プログラミング言語におけるクラスの粒度にこれといった決まりがなく、設計者の考え次第であるのと同様に、Puppetにおけるclassの粒度にも特に決まりはありません。だからといって好きにやってよいというものでないのも、プログラミング言語同様です。


このように、あるべき状態を上述のように分割して考え、その項目それぞれをclassに対応させていくと、うまく設計できるでしょう。

== td-agentのclassを定義する


さっそくclassを定義していきましょう。上記では3種類の項目を挙げましたが、その前に、それらの関係性を整理するための元締めとでもいうべきclassを定義することにします。まずは、td-agentというclassを定義します。さっそく書いていきましょう。


今回のmanifest用のディレクトリを用意します。

//cmd{
$ cd puppet/
$ mkdir class
$ cd class/
//}


以下の内容で、@<tt>{td-agent.pp}というファイルを作成してください。

//emlist{
class td-agent {
  include td-agent::install
  include td-agent::config
  include td-agent::service

     Class['td-agent::install']
  -> Class['td-agent::config']
  ~> Class['td-agent::service']
}

class td-agent::install {
  yumrepo { 'treasuredata':
    name     => 'TreasureData',
    descr    => 'TreasureData repo',
    baseurl  => 'http://packages.treasuredata.com/redhat/$basearch',
    enabled  => 1,
    gpgcheck => 0,
  }

  package { 'td-agent':
    ensure  => installed,
    require => Yumrepo['treasuredata'],
  }
}

class td-agent::config {
  file { '/etc/td-agent/td-agent.conf':
    content => template('td-agent.conf'),
  }
}

class td-agent::service {
  service { 'td-agent':
    enable     => true,
    ensure     => running,
    hasrestart => true,
  }
}

include td-agent
//}


また、以下の内容で、同じディレクトリに@<tt>{td-agent.conf}というァイルを作成してください。

//emlist{
<source>
  type forward
</source>

<match debug.**>
  type stdout
</match>
//}


見慣れない記法がいくつか登場しています。順番に見ていきましょう。

== class定義の方法


最初に、td-agentのclassを定義している箇所について説明しましょう。classを定義するには、以下のように@<tt>{class}キーワードを使います。

//emlist{
class td-agent {
  # ここに内容を書く
}

class td-agent::install {
  # ここに内容を書く
}
//}


簡単ですね。また、@<tt>{td-agent::install}のように、class名に@<tt>{::}を用いて階層化して表現できます。


Puppetのclassには「継承」の機能もあるのですが、本書では扱いません。以下の理由によります。

 * すくなくとも、本書の範囲では必要ない
 * より一般的にいって、継承よりも後述の@<tt>{include}による「合成」の方がclassの拡張として適切



継承よりも合成を、というのは、プログラミング言語でもよく言及される、よりよいプラクティスでもあります。

== classを@<tt>{include}する


次に目につくのは@<tt>{include}です。これは、先に挙げたtd-agentの状態記述に必要な3つの項目のそれぞれに対応するclassを読み込むための記述です。

//emlist{
include td-agent::install
include td-agent::config
include td-agent::service
//}


後述の依存関係の記述のために、@<tt>{td-agent}というトップレベルのclass内であらかじめ全て読み込んでおきます。

== class間の依存関係


先に挙げたtd-agentの状態記述に必要な3つの項目には、上から順番に関係があります。その順番にシステム状態が遷移していかなければ、td-agentが正しく起動する状態にはなり得ないわけです。

 1. システムにtd-agentをインストールする
 1. 設定ファイルを準備する
 1. td-agentが確実に起動するよう保証する



このような依存関係を、これまでは@<tt>{require}や@<tt>{notify}/@<tt>{subscribe}によって記述してきました。今回は、@<tt>{->}(dependency relationship)や@<tt>{~>}(refresh relationship)といった記法を用いて、関係を記述しています。


classを使って構造化を進めていくと、個々のresource type間の関係というよりもむしろ、class間の関係といったものを記述する必要がでてきます。そうした場合に、あらかじめ必要なclassを@<tt>{include}した上で、@<tt>{->}や@<tt>{~>}を用いて関係を記述すると、一箇所でまとめて関係を記述できて便利です。

//emlist{
   Class['td-agent::install']
-> Class['td-agent::config']
~> Class['td-agent::service']
//}


class内部のresource type間の関係はこれまで通り@<tt>{require}や@<tt>{notify}/@<tt>{subscribe}で、class間の関係は一箇所でclassをまとめて@<tt>{include}した上で@<tt>{->}や@<tt>{~>}で記述していくと、見通しのよい記述になるでしょう。

== manifestを適用する


以上のようにclassを定義した上で、@<tt>{td-agent.pp}の最後の行で@<tt>{include td-agent}として大元のclassを読み込み、適用するようにしています。このようにして、td-agentの各種状態に関するclass定義ができたところで、このmanifestを適用してみましょう。


@<tt>{--templatedir=.}を忘れずに引数にわたして、例によって@<tt>{puppet apply}を実行します。

//cmd{
[vagrant@puppet-book ~]$ cd /vagrant/puppet/class/
[vagrant@puppet-book class]$ sudo puppet apply --templatedir=. td-agent.pp
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/descr: descr changed '' to 'treasuredata repo'
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/baseurl: baseurl changed '' to 'http://packages.treasure-data.com/redhat/$basearch/'
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/enabled: enabled changed '' to '1'
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/gpgcheck: gpgcheck changed '' to '0'
Notice: /Stage[main]/Td-agent::Install/Package[td-agent]/ensure: created
Notice: /Stage[main]/Td-agent::Config/File[/etc/td-agent/td-agent.conf]/content: content changed '{md5}c61a851e347734f4500a9f7f373eed7f' to '{md5}344838bf3c7825824ab99ded78dff244'
Notice: /Stage[main]/Td-agent::Service/Service[td-agent]/ensure: ensure changed 'stopped' to 'running'
Notice: /Stage[main]/Td-agent::Service/Service[td-agent]: Triggered 'refresh' from 1 events
Notice: Finished catalog run in 99.17 seconds
//}


td-agentのインストール、設定ファイルの配置、サービス起動まで、うまくいっているようですね。

//cmd{
[vagrant@puppet-book class]$ sudo service td-agent status
td-agent (pid  6651) is running...
//}

== 動作確認をする


ここでもう一歩ふみこんで、本当に意図通りに動いているかどうか、確かめてみましょう。@<tt>{fluent-cat}コマンドを使うと、任意のメッセージをfluentdに対して送信できます。以下の例では、@<tt>{{"hello":"puppet"\}}というJSON文字列を、@<tt>{debug.test}というタグをつけて、fluentdに送信します。

//cmd{
[vagrant@puppet-book class]$ echo '{"hello":"puppet"}' | /usr/lib64/fluent/ruby/bin/fluent-cat debug.test
//}


ログファイルをのぞいてみましょう。

//cmd{
[vagrant@puppet-book class]$ cat /var/log/td-agent/td-agent.log
2013-04-14 06:06:42 +0000 [info]: starting fluentd-0.10.33
2013-04-14 06:06:42 +0000 [info]: reading config file path="/etc/td-agent/td-agent.conf"
2013-04-14 06:06:42 +0000 [info]: using configuration file: <ROOT>
  <source>
    type forward
  </source>
  <match debug.**>
    type stdout
  </match>
</ROOT>
2013-04-14 06:06:42 +0000 [info]: adding source type="tcp"
2013-04-14 06:06:42 +0000 [info]: adding match pattern="debug.**" type="stdout"
2013-04-14 06:06:42 +0000 [info]: listening fluent socket on 0.0.0.0:24224
2013-04-14 06:06:47 +0000 debug.test: {"hello":"puppet"}
//}


さきほどのmanifest適用時にfluentdが起動したログとあわせて、一番下に、メッセージを受信し、ログに記録している様子が見られます。manifestがきちんと適用され、fluentdが意図通りに動いている様子も確認できました。

== まとめ


本章では以下のことを学びました。

 * classを使って、システムを構成する各種resourceをグルーピングし構造化する
 * @<tt>{include}でclassをロードできる
 * class間のrelationshipの記述方法



これまでmanifestを書く際にしてきたように、システム状態を適切に分割し、それらの関係を考えるところまでは同じです。今回は、分割した各パーツをそれぞれclassにマッピングするというところだけが異なる点でした。
