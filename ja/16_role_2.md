## 第16章 サーバの役割を定義する Part.2

前章では、td-agentクラスタのうち「nginxのログを収集・送信する」役割(ロール)のサーバについて、manifestを記述しました。本章では「各サーバから送信されてくるログを集約するサーバ」について、manifestを書いていきましょう。

### 集約サーバの役割

今回採った構成では、nginxと同じサーバで起動するtd-agentがそのログを読み取り、本章でmanifestを書いていく中間td-agentに対してログを送信するのでした。そのようにして集約したログに対して、中間td-agentで、

  * 必要な加工をほどこす
  * ログファイルに書き出す
  * MongoDBやS3など、別のストレージに送信する

といったことを行うことで、ログの収集・集約の仕組みを簡単に作れてしまうのが、td-agentの素晴しいところです。

本章ではその一端を体験してみるために、集約したログをファイルに書き出してみるということをしてみましょう。

### ディレクトリ構成のおさらい

まずは前回作成したディレクトリを確認してみましょう。以下のようになっているはずです。

```
$ cd puppet/cluster/
$ tree
.
├── Vagrantfile
├── manifests
│   └── app.pp
├── modules
│   ├── nginx
│   │   ├── manifests
│   │   │   ├── config.pp
│   │   │   ├── init.pp
│   │   │   ├── install.pp
│   │   │   └── service.pp
│   │   └── templates
│   │       ├── index.html
│   │       └── my.conf
│   └── td-agent
│       ├── manifests
│       │   ├── config.pp
│       │   ├── init.pp
│       │   ├── install.pp
│       │   └── service.pp
│       └── templates
│           └── td-agent.conf
└── roles
    ├── app
    │   ├── manifests
    │   │   ├── init.pp
    │   │   ├── nginx.pp
    │   │   ├── td-agent
    │   │   │   └── config.pp
    │   │   └── td-agent.pp
    │   └── templates
    │       └── td-agent
    │           └── app.conf
    └── log
        ├── manifests
        └── templates
```

今回は、`roles/log`以下にファイルを配置していきます。さっそく、大元のclassを書いていきましょう。以下の内容で、`roles/log/manifests/init.pp`というファイルを作成してください。

```
class log {
  include ::iptables
  include log::td-agent
}
```

今回は`log`という役割を、その具体的な中身として集約td-agentが動くものとして定義していきます。そのため、これから`log::td-agent`というclassを作成します。

### iptablesについての補足

その前に、`include ::iptables`という箇所について説明しておきましょう。

提供されているVagrantのOSイメージによっては、iptablesにより、外部からの接続が制限されている場合があります。本書の検証環境では、80番ポートや集約td-agentで利用する24224番ポートなどは、外部からの接続が禁止された状態になっていました。

今回は、manifestを書き、td-agentの動作を検証することが目的ですので、いったんiptablesを無効にしておきましょう。「iptablesが無効になっている」という状態を定義するため、以下の通りmodulesを作成します。

```
$ mkdir modules/iptables
$ mkdir modules/iptables/manifests
```

以下の内容で、それぞれファイルを作成します。

modules/iptables/manifests/init.pp:

```
class iptables {
  include iptables::service
}
```

modules/iptables/manifests/service.pp:

```
class iptables::service {
  service { 'iptables':
    enable => false,
    ensure => stopped,
  }
}
```

これを、前述の通り`incelude ::iptables`として読み込んでおき、iptablesを無効にしておきます。今回は、あくまでも検証なのでこのような方法を採りましたが、本番環境でこのようなことをすることのないよう、念のため申し添えておきます。

### `log::td-agent`

さて、次は`log::td-agent`です。以下の内容で、`td-agent.pp`というファイルを作成してください。

```
class log::td-agent {
  include ::td-agent
  include log::td-agent::config

     Class['::td-agent::install']
  -> Class['log::td-agent::config']
  ~> Class['::td-agent::service']
}
```

ここでなにをやっているかについてはもう、説明するまでもないでしょう。大元のmoduleの設定に、集約td-agent専用の設定ファイルを追加して、サービスをrefreshするという状態を定義しています。

次に、以下の内容で`log::td-agent::config`を作成します。

```
class log::td-agent::config {
  file { '/etc/td-agent/conf.d/log.conf':
    content => template('log/td-agent/log.conf'),
  }

  file { '/var/log/td-agent/app':
      ensure => directory,
      owner  => 'td-agent',
      group  => 'td-agent',
  }
}
```

今回、`app`ロールのサーバからtd-agentを通じて送信されてくるnginxのアクセスログを、`log`ロールの`/var/log/td-agent/app`以下にファイルとして集約することにします。そのため、あらかじめログを配置するディレクトリを作成しています。

次に設定ファイルです。以下の内容で、`roles/log/templates/td-agent/log.conf`というファイルを作成してください。

```
<source>
  type forward
  port 24224
</source>

<match forward.**>
  type file
  path /var/log/td-agent/app/access
  time_slice_format %Y%m%d
</match>
```

この設定により集約td-agentは、

  1. 24224番ポートで接続を待ち受ける
  2. 送信されてきたログを一定期間バッファリングする(上記の設定だと、`time_slice_format`が`%Y%m%d`なので、1日ごとにログファイルに書き出す)
  3. `path`、`time_slice_format`で指定されたファイル名、フラッシュ時に自動採番される数字により構成されるファイル名に、ログを書き出します。

out_fileプラグインの仕様については、以下のブログエントリが詳しいのでご参照ください。

[http://blog.tnmt.info/2012/10/19/about-fluentd-out-file-plugin/](http://blog.tnmt.info/2012/10/19/about-fluentd-out-file-plugin/)

### `log`ロールを`include`する

`app`ロールと同様に、`log`ロールのためのエントリーポイントとなるファイルを作成しましょう。

以下の内容で、`manifests/log.pp`というファイルを作成してください。

```
include log
```

最終的なディレクトリ構成は、以下のようになります。

```
$ tree ../cluster
../cluster/
├── Vagrantfile
├── manifests
│   ├── app.pp
│   └── log.pp
├── modules
│   ├── iptables
│   │   └── manifests
│   │       ├── init.pp
│   │       └── service.pp
│   ├── nginx
│   │   ├── manifests
│   │   │   ├── config.pp
│   │   │   ├── init.pp
│   │   │   ├── install.pp
│   │   │   └── service.pp
│   │   └── templates
│   │       ├── index.html
│   │       └── my.conf
│   └── td-agent
│       ├── manifests
│       │   ├── config.pp
│       │   ├── init.pp
│       │   ├── install.pp
│       │   └── service.pp
│       └── templates
│           └── td-agent.conf
└── roles
    ├── app
    │   ├── manifests
    │   │   ├── init.pp
    │   │   ├── nginx.pp
    │   │   ├── td-agent
    │   │   │   └── config.pp
    │   │   └── td-agent.pp
    │   └── templates
    │       └── td-agent
    │           └── app.conf
    └── log
        ├── manifests
        │   ├── init.pp
        │   ├── td-agent
        │   │   └── config.pp
        │   └── td-agent.pp
        └── templates
            └── td-agent
                └── log.conf
```

### manifestを適用する

まず、`log`ロールの仮想ホストにログインします(仮想ホストが2台あってまぎらわしいので、間違えないよう気をつけてください)。

```
$ vagrant ssh log
```

`puppet apply`でmanifestを適用しましょう。`--modulepath=modules:roles`と、`roles`ディレクトリも指定します。

```
[vagrant@log ~]$ cd /vagrant
[vagrant@log vagrant]$ sudo puppet apply --modulepath=modules:roles manifests/log.pp
Notice: /Stage[main]/Iptables::Service/Service[iptables]/ensure: ensure changed 'running' to 'stopped'
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/descr: descr changed '' to 'treasuredata repo'
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/baseurl: baseurl changed '' to 'http://packages.treasure-data.com/redhat/$basearch/'
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/enabled: enabled changed '' to '1'
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/gpgcheck: gpgcheck changed '' to '0'
Notice: /Stage[main]/Td-agent::Install/Package[td-agent]/ensure: created
Notice: /Stage[main]/Td-agent::Config/File[/etc/td-agent/conf.d]/ensure: created
Notice: /Stage[main]/Log::Td-agent::Config/File[/var/log/td-agent/app]/ensure: created
Notice: /Stage[main]/Log::Td-agent::Config/File[/etc/td-agent/conf.d/log.conf]/ensure: defined content as '{md5}b5f1e6a793038fe449196c52aa544168'
Notice: /Stage[main]/Td-agent::Config/File[/etc/td-agent/td-agent.conf]/content: content changed '{md5}c61a851e347734f4500a9f7f373eed7f' to '{md5}f3d4e2ffaec6ef9b67bd01171844fa60'
Notice: /Stage[main]/Td-agent::Service/Service[td-agent]/ensure: ensure changed 'stopped' to 'running'
Notice: /Stage[main]/Td-agent::Service/Service[td-agent]: Triggered 'refresh' from 1 events
Notice: Finished catalog run in 152.16 seconds
```

td-agentが起動しているか確かめてみましょう。

```
[vagrant@log vagrant]$ sudo service td-agent status
td-agent (pid  6162) is running...
```

ちゃんと動いているようですね。

### 仮想ホスト同士の連携を動作確認する

なにを「動作確認」するべきなのでしょうか。あらためて整理してみましょう。`app`で`http://app.puppet-book.local/`にアクセスすると:

  1. `app`ロールの`/var/log/nginx/app.access.log`に、アクセスログが記録される
  2. 1のログが、`log`ロールのtd-agentに送信され、`/var/log/td-agent/app`以下に記録される

というものでした。実際にみていきましょう。

まず、`app`ロールの仮想ホストで、`http://app.puppet-book.local/`にアクセスしてみます。

```
[vagrant@app vagrant]$ curl http://app.puppet-book.local/
Hello, Puppet!
```

ログを見てみましょう。

```
[vagrant@app vagrant]$ cat /var/log/nginx/app.access.log
time:20/Apr/2013:10:09:55 +0000 host:127.0.0.1  method:GET      path:/  version:HTTP/1.1        status:200    size:15 referer:-       ua:curl/7.19.7 (x86_64-redhat-linux-gnu) libcurl/7.19.7 NSS/3.13.6.0 zlib/1.2.3 libidn/1.18 libssh2/1.4.2     restime:0.000   ustime:-
```

ちゃんと記録されているようですね。

次に、今度は`log`ホストにログインして、ログファイルのディレクトリをのぞいてみましょう。

```
[vagrant@log vagrant]$ ls -la /var/log/td-agent/app
total 12
drwxr-xr-x 2 td-agent td-agent 4096 Apr 20 10:09 .
drwxr-xr-x 3 td-agent td-agent 4096 Apr 20 10:05 ..
-rw-rw-rw- 1 td-agent td-agent  331 Apr 20 10:09 access.20130420.b4dac809922779494
```

なにやらファイルができています。

```
[vagrant@log vagrant]$ cat /var/log/td-agent/app/access.20130420.b4dac809922779494
2013-04-20T10:09:55+00:00       forward.app.access      {"time":"20/Apr/2013:10:09:55 +0000","host":"127.0.0.1","method":"GET","path":"/","version":"HTTP/1.1","status":"200","size":"15","referer":"-","ua":"curl/7.19.7 (x86_64-redhat-linux-gnu) libcurl/7.19.7 NSS/3.13.6.0 zlib/1.2.3 libidn/1.18 libssh2/1.4.2","restime":"0.000","ustime":"-"}
```

`app`の仮想ホストに記録されていたログが、きちんと転送されてきているようですね。

ファイル名が`access.20130420.b4dac809922779494`となっており、末尾になにやらハッシュ値のようなものがついています。これは現在バッファリング中の状態であることを示します。

先に説明した通り、out_fileプラグインは`time_slice_format`で指定した期間の単位でログファイルを作成するので、このファイルは翌日(正確には、この場合4/21の0:00を過ぎた後に)、`access.20130420_0`というファイルに書き出されます。

以上で、Puppetでtd-agentのクラスタ構成を記述し、動作確認することができました。

### まとめ

本章では以下のことを学びました。

  * 集約td-agentの役割、manifestの書き方
  * iptablesについて
  * out_fileプラグインについて

また、前章までに書いてきたmoduleやロール定義と合わせて、Puppetによりtd-agentのクラスタ構成を定義し、nginxのログを取得・集約できることを確認しました。

現実のシステムは、様々な役割を持った複数台のサーバにより構成されます。今回はごく単純なtd-agentによるクラスタ構成を試してみましたが、現実のシステム構築も本質的には、今回と同じようなことを必要な分だけくりかえしていくことに他なりません。

本書をここまで読んだあなたは、Puppetを使ってシステムを構築する知識を既に習得済みです。自信を持って、現実のシステムに適用していってください。
