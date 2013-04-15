## 第15章 サーバの役割を定義する Part.1

前章では、td-agentを使う上でこれだけは最低限必要だろうという状態を、ひとつのmoduleとして記述しました。本章では、そのmoduleを使ってより具体的な状態、すなわちサーバの「役割」を定義するmanifestを書いていきましょう。

なぜそのような、一見するとまわりくどく思えるようなことをするのでしょうか。それは、moduleの再利用性を担保するためです。その意味においてPuppetのmanifest記述は、一般のプログラミングと同じように、適切な設計を要します。さっそく具体例を見ていきましょう。

### td-agentクラスタの構成

td-agentは前述の通り、様々なログを収集・集約するためのツールです。td-agent間で通信してログをリレーできる機能を用い、クラスタを構成して利用するのが一般的です。

単純なクラスタ構成の場合、

  * ログを収集して集約サーバに送信する1〜数台のサーバ
  * 各サーバから送信されてきたログを集約するサーバ

という構成になるでしょう。つまりここでは、サーバの役割として

  1. なんらかのサービスがログを記録し、そのログを収集・送信する
  2. ログを集約する

のふたつがあるということになります。前章で作成したmoduleを使って、このふたつの具体的な役割を記述します。

### ディレクトリを準備する

まず、今回のmanifestを配置するディレクトリを用意します。

```
$ cd puppet/
$ mkdir cluster
$ cd cluster/
```

前回作成したtd-agentのmoduleをコピーしてください(実際にmoduleを再利用する際はコピーする必要はありませんが、説明の都合上、前回のディレクトリとわけたいのでそうしてもらっています)。また、通常、moduleは複数あり得るので、`modules`と複数形の名前にしておきましょう。

```
$ cp -R ../module modules
```

上記したふたつの役割を記述するmanifestを配置するために、`roles`というディレクトリを作成しましょう。

```
$  mkdir roles
```

前述の「なんらかのサービスがログを記録し、そのログを収集・送信する」という役割のために`app`、「ログを集約する」という役割のために`log`というディレクトリを作成しましょう。同時に、それぞれのディレクトリの下に`manifests`と`templates`も作成します。

```
$ mkdir roles/{app,log}
$ mkdir roles/app/{manifests,templates}
$ mkdir roles/log/{manifests,templates}
```

以上の作業により、最終的に以下のようなディレクトリ/ファイル構成ができあがっているはずです。

```
$ tree ../cluster
../cluster
├── modules
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
    │   └── templates
    └── log
        ├── manifests
        └── templates

11 directories, 5 files
```

### あらたにVagrantfileを準備する

これまでとは異なり、今回はクラスタ構成を採るため仮想ホストを複数台必要としますが、ここでもVagrantが活躍します。

以下の内容で、`puppet/cluster`(上で作成した`cluster`ディレクトリ)の直下に、あらたに`Vagrantfile`を作成してください。

```
Vagrant.configure("2") do |config|
  config.vm.box     = "centos-6.4"
  config.vm.box_url = "http://developer.nrel.gov/downloads/vagrant-boxes/CentOS-6.4-x86_64-v20130309.box"

  config.vm.define :app do |app_config|
    app_config.vm.hostname = "app.puppet-book.local"
    app_config.vm.network :private_network, ip: "192.168.1.10"
    app_config.vm.network :forwarded_port, guest: 80, host: 8000
  end

  config.vm.define :log do |log_config|
    log_config.vm.hostname = "log.puppet-book.local"
    log_config.vm.network :private_network, ip: "192.168.1.11"
    log_config.vm.network :forwarded_port, guest: 24224, host: 24224
  end
end
```

今回は、仮想ホスト間の通信が必要となるため、スタティックなプライベートIPアドレスを割り当てています。

いつものように`vagrant up`するだけで、今度は2台の仮想ホストが起動します(便利ですね!)。

```
$ vagrant up
Bringing machine 'app' up with 'virtualbox' provider...
[app] Importing base box 'centos-6.4'...
[app] Matching MAC address for NAT networking...
[app] Setting the name of the VM...
[app] Clearing any previously set forwarded ports...
[app] Fixed port collision for 22 => 2222. Now on port 2205.
[app] Creating shared folders metadata...
[app] Clearing any previously set network interfaces...
[app] Preparing network interfaces based on configuration...
[app] Forwarding ports...
[app] -- 22 => 2205 (adapter 1)
[app] -- 80 => 8000 (adapter 1)
[app] Booting VM...
[app] Waiting for VM to boot. This can take a few minutes.
[app] VM booted and ready for use!
[app] Setting hostname...
[app] Configuring and enabling network interfaces...
[app] Mounting shared folders...
[app] -- /vagrant
Bringing machine 'log' up with 'virtualbox' provider...
[log] Importing base box 'centos-6.4'...
[log] Matching MAC address for NAT networking...
[log] Setting the name of the VM...
[log] Clearing any previously set forwarded ports...
[log] Fixed port collision for 22 => 2205. Now on port 2206.
[log] Creating shared folders metadata...
[log] Clearing any previously set network interfaces...
[log] Preparing network interfaces based on configuration...
[log] Forwarding ports...
[log] -- 22 => 2206 (adapter 1)
[log] -- 24224 => 24224 (adapter 1)
[log] Booting VM...
[log] Waiting for VM to boot. This can take a few minutes.
[log] VM booted and ready for use!
[log] Setting hostname...
[log] Configuring and enabling network interfaces...
[log] Mounting shared folders...
[log] -- /vagrant
```

`Vagrantfile`で指定した通り、`app`と`log`という名前で、2台の仮想ホストが起動している様子が確認できます。

ちなみに、`app`と`log`の仮想ホストをそれぞれ別々に`vagrant`コマンドから扱いたい場合、

```
$ vagrant up app
```

というように、その名前を引数にわたすことができます。

### nginxモジュール

さて「なんらかのサービスがログを記録し、そのログを収集・送信する」役割のサーバを定義していくわけですが、あまり人工的な例でもつまらないので、より実践的な題材と取り上げます。その「なんらかのサービス」の具体例として、nginxのログを収集してみることにしましょう。

そのために、まずは第5章で作成したnginxのmanifestをmodule化します。

```
$ mkdir modules/nginx
$ mkdir modules/nginx/manifests
```

内容はほとんど第5章と同じですので、駆け足でいきます。`http://localhost/`へのアクセスを[LTSVフォーマット](http://ltsv.org/)でログに記録するだけの、簡単な設定です。

以下の内容でそれぞれファイルを作成してください。

modules/nginx/manifests/init.pp:

```
class nginx {
  include nginx::install
  include nginx::config
  include nginx::service

     Class['nginx::install']
  -> Class['nginx::config']
  ~> Class['nginx::service']
}
```

modules/nginx/manifests/install.pp:

```
class nginx::install {
  yumrepo { 'nginx':
    descr    => 'nginx yum repository',
    baseurl  => 'http://nginx.org/packages/centos/$releasever/$basearch/',
    enabled  => 1,
    gpgcheck => 0
  }

  package { 'nginx':
    require => Yumrepo['nginx'],
  }

  file { '/var/log/nginx':
    ensure  => directory,
    owner   => 'nginx',
    group   => 'nginx',
    mode    => '0755',
    require => Package['nginx'],
  }
}
```

modules/nginx/manifests/config.pp:

```
class nginx::config {
  file { '/etc/nginx/conf.d/my.conf':
    content => template('nginx/my.conf'),
  }

  file { '/usr/share/nginx/html/index.html':
    content => template('nginx/index.html'),
  }
}
```

modules/nginx/manifests/service.pp:

```
class nginx::service {
  service { 'nginx':
    enable     => true,
    ensure     => running,
    hasrestart => true,
  }
}
```

modules/nginx/templates/my.conf:

```
server {
  listen       80;
  server_name  localhost;

  log_format ltsv "time:$time_local\thost:$remote_addr\tmethod:$request_method\tpath:$request_uri\tversion:$server_protocol\tstatus:$status\tsize:$body_bytes_sent\treferer:$http_referer\tua:$http_user_agent\trestime:$request_time\tustime:$upstream_response_time";
  access_log /var/log/nginx/app.access.log ltsv;

  location / {
    root  /usr/share/nginx/html;
    index index.html;
  }
}
```

modules/nginx/templates/index.html:

```
Hello, Puppet!
```

### ログを収集・送信する

さて、今度はログを収集・送信するtd-agentのmanifestを書いていきます。前章で作成したmoduleで大枠はできているので、今回は

  * nginxのログを取得する
  * それを集約サーバに送信する

というふたつの設定をほどこすだけです。

その前に、まずは`app`という役割をmanifestとして定義します。といっても特に変わったことをするわけではなく、`app`というclassを定義するだけです。

以下の内容で、`roles/app/manifests/init.pp`というファイルを作成してください。

```
class app {
  include app::nginx
  include app::td-agent

     Class['app::nginx']
  -> Class['app::td-agent']
}
```

役割`app`では、nginxとtd-agentを起動させるのでした。言葉を変えていうと、そのふたつのサービスからなる役割のことを`app`と呼ぶことにしたのでした。このように、moduleを組合せることで、役割を定義していきます。

ここでいったん整理してみると、システム状態を記述するmanifestの全体は、

  1. システムの状態を表す最小単位としての各種resource type
  2. resouce typeをグルーピングするclass
  3. classなどの集まりとしてのmodule
  4. moduleの組合せとしての役割

という階層関係を持ちます。

さて、上で名前だけ出てきた`app::nginx`の中身を作成しましょう。以下の内容で、`roles/app/manifests/nginx.pp`というファイルを作成してください。

```
class app::nginx {
  include ::nginx
}
```

今回、nginxはmoduleの設定をそのまま用いるので、ここでは単に、先に定義したmoduleに含まれるclassを`include`するだけです。ちなみに、`::nginx`と`::`と先頭につけているのは、名前空間のトップレベルから名前解決を行うという意味です。Rubyを知っている読者なら、Rubyにおけるmoduleの名前解決と同様の仕組みだといえば、わかりやすいでしょう。詳細については、以下のドキュメントを参照してください。

[http://docs.puppetlabs.com/puppet/latest/reference/lang_namespaces.html](http://docs.puppetlabs.com/puppet/latest/reference/lang_namespaces.html)

このように、いまいるコンテキストとは違う階層にある別のclassを`include`する際は、このように`::`を先頭につけて、明示的にトップレベルから指定すると、思わぬハマりどころを避けられるので、お勧めです。

次は、td-agentです。以下の内容で、`roles/app/manifests/td-agent.pp`というファイルを作成してください。

```
class app::td-agent {
  include ::td-agent
  include app::td-agent::config

     Class['::td-agent']
  -> Class['app::td-agent::config']
  ~> Class['::td-agent::service']
}
```

今回、`app`のために設定を追加するので、`app::td-agent::config`というclassを作成します。また、moduleに含まれる各classとの関係も、このようにここで定義しておきます。

`app::td-agent::config`の内容は以下の通りです。

roles/app/manifests/td-agent/config.pp:

```
class app::td-agent::config {
  file { '/etc/td-agent/conf.d/app.conf':
    content => template('app/td-agent/app.conf'),
  }
}
```

テンプレートも用意しましょう。

roles/app/templates/td-agent/app.conf:

```
<source>
  type     tail
  path     /var/log/nginx/app.access.log
  tag      forward.app.access
  format   ltsv
</source>

<match forward.**>
  type forward

  <server>
    host 192.168.1.11
    port 24224
  </server>

  buffer_type file
  buffer_path /var/log/td-agent/buffer/forward

  # すぐに結果を確認できるよう、一時的に短かくしておく
  flush_interval 1s
</match>
```

大元のtd-agentのmoduleでは、`/etc/td-agent/conf.d/*.conf`にあるファイルをロードするよう設定されているので、ここでは`roles/app/templates/td-agent/app.conf`として、`app`の役割に特化したファイルを配置するだけで設定が完了します。

### manifestを適用する

まずは`app`用のmanifestを先に適用してみましょう。今回は、`vagrant ssh`に`app`という引数をわたしてログインします。また、`Vagrantfile`の場所がこれまでとは違うので、マウントされているディレクトリも異なります。注意してください。

```
$ vagrant ssh app
Welcome to your Vagrant-built virtual machine.
[vagrant@app ~]$ cd /vagrant
[vagrant@app vagrant]$ ls
Vagrantfile  modules  roles
```

適用は、いつもの通り`puppet apply`コマンドを使います。今回は、`--modulepath`の引数に`roles`ディレクトリを追加しています。役割を定義するmanifestも、実際にはmoduleを組合せたmoduleとして構成しているからです。
```
[vagrant@app vagrant]$ sudo puppet apply --modulepath=modules:roles --execute 'include app'
Notice: /Stage[main]/Nginx::Install/Yumrepo[nginx]/descr: descr changed '' to 'nginx yum repository'
Notice: /Stage[main]/Nginx::Install/Yumrepo[nginx]/baseurl: baseurl changed '' to 'http://nginx.org/packages/centos/$releasever/$basearch/'
Notice: /Stage[main]/Nginx::Install/Yumrepo[nginx]/enabled: enabled changed '' to '1'
Notice: /Stage[main]/Nginx::Install/Yumrepo[nginx]/gpgcheck: gpgcheck changed '' to '0'
Notice: /Stage[main]/Nginx::Install/Package[nginx]/ensure: created
Notice: /Stage[main]/Nginx::Install/File[/var/log/nginx]/owner: owner changed 'root' to 'nginx'
Notice: /Stage[main]/Nginx::Install/File[/var/log/nginx]/group: group changed 'root' to 'nginx'
Notice: /Stage[main]/Nginx::Config/File[/usr/share/nginx/html/index.html]/content: content changed '{md5}e3eb0a1df437f3f97a64aca5952c8ea0' to '{md5}1db16ebfb21d376e5b2ae9d1eaf5b3c8'
Notice: /Stage[main]/Nginx::Config/File[/etc/nginx/conf.d/my.conf]/ensure: defined content as '{md5}3fdf372b19ce42e6e52beee512083ba0'
Notice: /Stage[main]/Nginx::Service/Service[nginx]/ensure: ensure changed 'stopped' to 'running'
Notice: /Stage[main]/Nginx::Service/Service[nginx]: Triggered 'refresh' from 1 events
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/descr: descr changed '' to 'treasuredata repo'
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/baseurl: baseurl changed '' to 'http://packages.treasure-data.com/redhat/$basearch/'
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/enabled: enabled changed '' to '1'
Notice: /Stage[main]/Td-agent::Install/Yumrepo[treasuredata]/gpgcheck: gpgcheck changed '' to '0'
Notice: /Stage[main]/Td-agent::Install/Package[td-agent]/ensure: created
Notice: /Stage[main]/Td-agent::Config/File[/etc/td-agent/conf.d]/ensure: created
Notice: /Stage[main]/Td-agent::Config/File[/etc/td-agent/td-agent.conf]/content: content changed '{md5}c61a851e347734f4500a9f7f373eed7f' to '{md5}f3d4e2ffaec6ef9b67bd01171844fa60'
Notice: /Stage[main]/App::Td-agent::Config/File[/etc/td-agent/conf.d/app.conf]/ensure: defined content as '{md5}5f24a1a0a30ae2e85916dfa5526a0b73'
Notice: /Stage[main]/Td-agent::Service/Service[td-agent]/ensure: ensure changed 'stopped' to 'running'
Notice: /Stage[main]/Td-agent::Service/Service[td-agent]: Triggered 'refresh' from 1 events
Notice: Finished catalog run in 369.54 seconds
```

実際にnginxとtd-agentが起動しているか、確認してみましょう。

```
[vagrant@app vagrant]$ ps auxw | grep -E '/(nginx|td-agent)/'
root      5926  0.0  0.2  44672  1088 ?        Ss   14:51   0:00 nginx: master process /usr/sbin/nginx -c /etc/nginx/nginx.conf
td-agent  6158  0.0  2.8 210928 13588 ?        Sl   14:57   0:00 /usr/lib64/fluent/ruby/bin/ruby /usr/sbin/td-agent --group td-agent --log /var/log/td-agent/td-agent.log --daemon /var/run/td-agent/td-agent.pid
td-agent  6161  0.1  4.0 238760 18984 ?        Sl   14:57   0:00 /usr/lib64/fluent/ruby/bin/ruby /usr/sbin/td-agent --group td-agent --log /var/log/td-agent/td-agent.log --daemon /var/run/td-agent/td-agent.pid
```

ちゃんと起動しているようです。

### まとめ

本章では、サーバの具体的な役割を、moduleの組合せとして定義していく方法を学びました。また、その役割自体もmoduleを組合せたmoduleとして定義したのでした。

仮想ホストを2台使うようになって、構成が急速に複雑化しましたが、やっていること自体はこれまでと変わるところはありません。じっくり取り組んでみてください。
