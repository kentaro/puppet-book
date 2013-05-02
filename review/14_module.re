
= manifestに関連するファイルをまとめる - module


前章では、manifestの内容を構造化する方法としてのclassについて学びました。本章では、内容に加えて、ファイルとしてもmanifestを分割し、さらにわかりやすく構造化する方法としてのmoduleについて見ていきます。

== Puppetにおけるmoduleとは


moduleとは、classの含まれる様々なファイルや、それらのclassが必要とするテンプレートなどをひとまとめに管理するための仕組みです。classがmanifestの内容を分類・整理するためのものであった一方で、moduleは上記のファイル群を分類・整理します。


moduleは、再利用が可能な単位でまとめるのが一般的です。そのような、特定のシステムに限らず、広く一般的に使えるmoduleが、有志により@<href>{https://forge.puppetlabs.com/,Puppet Forge}で公開されています。本章を読み、moduleの作成方法を学んだら、興味をひいたmoduleを眺めてみてください。今後moduleを作成する際に、おおいに参考になるでしょう。

== td-agentのmoduleを作成する


moduleの詳細に入る前に、実際に内容を見る方が理解がはやいでしょうから、まずはさっそく作成していきましょう。


今回のtd-agentの例では、td-agentを利用するに際して、最低限これだけは必要になるだろうという範囲で、ひとつのmoduleとして構成します。前回作成した@<tt>{td-agent.pp}の内容は、ほぼそれに相当するといえるでしょう。


今回作成するmodule用のディレクトリを用意します。

//emlist{
$ cd puppet/
$ mkdir modules
$ cd modules/
//}


まずは、module名と同名のディレクトリを作成しましょう。

//emlist{
$ mkdir td-agent
//}


次に、manifestとテンプレートを格納するディレクトリを作成します。それぞれ、@<tt>{manifests}と@<tt>{templates}という名前にする必要があります。

//emlist{
$ mkdir td-agent/manifests
$ mkdir td-agent/templates
//}


前回作成した@<tt>{td-agent.pp}には、4つのclassが定義されていました。これをそれぞれ、下記のようにファイルに分割していきます。4つのclassのうち@<tt>{td-agent}だけは特別で、@<tt>{init.pp}というファイル名にしてください。


td-agent/manifests/init.pp:

//emlist{
class td-agent {
  include td-agent::install
  include td-agent::config
  include td-agent::service

     Class['td-agent::install']
  -> Class['td-agent::config']
  ~> Class['td-agent::service']
}
//}


td-agent/manifests/install.pp:

//emlist{
class td-agent::install {
  yumrepo { 'treasuredata':
    name     => 'treasuredata',
    descr    => 'treasuredata repo',
    baseurl  => 'http://packages.treasure-data.com/redhat/$basearch/',
    enabled  => 1,
    gpgcheck => 0,
  }

  package { 'td-agent':
    ensure  => installed,
    require => Yumrepo['treasuredata'],
  }
}
//}


td-agent/manifests/config.pp:

//emlist{
class td-agent::config {
  file { '/etc/td-agent/td-agent.conf':
    content => template("td-agent/td-agent.conf"),
  }


  file { '/etc/td-agent/conf.d':
    ensure => directory,
  }
}
//}


td-agent/manifests/service.pp:

//emlist{
class td-agent::service {
  service { 'td-agent':
    enable     => true,
    ensure     => running,
    hasrestart => true,
  }
}
//}


また、@<tt>{td-agent.conf}は@<tt>{templates}ディレクトリ以下に配置します。内容は前回とは異なり、以下のようにします。サーバの用途ごとに異なる設定を@<tt>{conf.d/}以下に配置することを前提に、用途を問わず共通化しておくべき設定を提供します。

//emlist{
# conf.d以下に用途ごとに設定を分けて置く
include conf.d/*.conf

# デバッグ用ログ
<match debug.**>
  type stdout
</match>

# fluentdの内部イベントログ
<match fluent.**>
  type file
  path /var/log/td-agent/fluent.log
</match>

# どのタグにもマッチしなかったログ
<match **>
  type file
  path /var/log/td-agent/no_match.log
</match>

# drb経由で接続してデバッグするための設定
<source>
  type debug_agent
  port 24230
</source>
//}


以上の作業を終えたら、以下の通りのディレクトリ/ファイル構成になっているはずです(@<tt>{tree}コマンドは、MacOSXの場合、homebrewによってインストールできます)。

//emlist{
$ tree puppet/modules/puppet/modules/
└── td-agent
    ├── manifests
    │   ├── config.pp
    │   ├── init.pp
    │   ├── install.pp
    │   └── service.pp
    └── templates
        └── td-agent.conf
//}


これで、td-agentのmoduleが作成できました。

== moduleのレイアウト


moduleは、前述の通り、ファイル群を分類・整理するための仕組みです。そのため、ディレクトリ名やファイル名に、決まったルールがあります。


まずはディレクトリ名のルールについて説明します。

 * moduleのディレクトリ名をmoduleと同名にする
 * manifestを配置するディレクトリ名は@<tt>{manifests}にする
 * templateを配置するディレクトリ名は@<tt>{templates}にする



他にもmoduleに含められる種類のディレクトリはあるのですが、本書の範囲を超えますので、説明しません。興味のある読者は、以下のドキュメントを参照してください。


@<href>{http://docs.puppetlabs.com/puppet/latest/reference/modules_fundamentals.html,http://docs.puppetlabs.com/puppet/latest/reference/modules_fundamentals.html}


次に、manifestについて見ていきましょう。


上記で、@<tt>{td-agent.pp}に含まれている4つのclassをファイルに分割しましたが、@<tt>{td-agent}だけは@<tt>{init.pp}というファイル名にした他は、それぞれclass名の@<tt>{::}以降の部分をファイル名とし、@<tt>{manifests}ディレクトリ以下に配置しました。

 * moduleと同名のclassについては@<tt>{init.pp}というファイル名にする
 * その他のclassについては@<tt>{モジュール名::}以降をファイル名と対応させる
 * 各ファイルにはただひとつだけclassを含めることができる



このようにclass名とファイル名を対応させておくことで、Puppetがclass名からファイル名を決定し、自動的にロードすることができるようになっています。@<tt>{init.pp}では、@<tt>{include td-agent::install}などとしていますが、このルールにより、@<tt>{td-agent/manifests/install.pp}が自動的にロードできるのです。

== moduleを適用する


さて、作成したtd-agentのmoduleを適用してみましょう。今回は、あらたなコマンドライン引数@<tt>{--modulepath}と@<tt>{--execute}を使います。


@<tt>{--modulepath}により、moduleの格納されているパスを指定します。そのパス以下に含まれるディレクトリの名前が、module名として認識されます。今回の例では、@<tt>{puppet/modules/}ディレクトリ以下にある@<tt>{td-agent}ディレクトリが、module名として認識されます。


@<tt>{--execute}オプションに文字列をわたすと、manifestファイルに書かれたもの同様に実行することができます。今回は、これまでのような通常のmanifestを書くことなく、moduleのみ作成したので、@<tt>{--execute}オプションによってclassの@<tt>{include}を行います。

//emlist{
[vagrant@puppet-book ~]$ cd /vagrant/puppet/modules/
[vagrant@puppet-book modules]$ sudo puppet apply --modulepath=. --execute 'include td-agent'
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/descr: descr changed '' to 'treasuredata repo'
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/baseurl: baseurl changed '' to 'http://packages.treasure-data.com/redhat/$basearch/'
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/enabled: enabled changed '' to '1'
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/gpgcheck: gpgcheck changed '' to '0'
Notice: /Stage[main]/Td-agent::Install/Package[td-agent]/ensure: created
Notice: /Stage[main]/Td-agent::Config/File[/etc/td-agent/td-agent.conf]/content: content changed '{md5}c61a851e347734f4500a9f7f373eed7f' to '{md5}183b11dffec86747a3b31ddda02384ab'
Notice: /Stage[main]/Td-agent::Service/Service[td-agent]/ensure: ensure changed 'stopped' to 'running'
Notice: /Stage[main]/Td-agent::Service/Service[td-agent]: Triggered 'refresh' from 1 events
Notice: Finished catalog run in 338.01 seconds
//}


前回同様に、td-agentのインストールから起動まで、ちゃんと適用が成功したようですね。

== まとめ


本章では以下のことを学びました。

 * ファイル群を分類・整理するための仕組みとしてのmodule
 * moduleのレイアウトと、@<tt>{include}によるファイルロードの仕組み
 * @<tt>{--modulepath}と@<tt>{--execute}オプションを使ったmoduleの適用



前章で学んだclassも、本章で学んだmoduleも、これから本格的なmanifestを書いていくためには必須の仕組みですので、よく復習して、内容を完全に把握するようにしてください。


ルールがあることを最初は複雑に感じるかもしれませんが、その分、便利なものですので、慣れさえしたら簡単です。
