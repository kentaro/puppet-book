
= システム状態をテストする - serverspec


現実のシステムは複雑です。また、manifestはいちど書いたら終わりではありません。現実のシステムはたえず変化していきます。我々は、そのような複雑さと変化にうまく対応していかなければなりません。


本章では、manifestにより記述・構築されるシステム状態を、効果的・効率的にテストする方法を紹介します。

== serverspec


プログラミングにおけるテストが、現実のアプリケーションの複雑さと変化に対応するためのものであるように、システム状態についても同様のアプローチが可能なのではないか？そんな着想に基づいて、@<href>{http://mizzy.org/,宮下剛輔氏}により開発されているのがserverspecです。


@<href>{http://serverspec.org/,http://serverspec.org/}


serverspecは、以下の特徴を持つシステム状態のテストツールです。

 1. RSpecのカスタムマッチャによる宣言的な記法でシステム状態をテストする
 1. manifestのシンタクスレベルではなく、Puppetなどを適用した実際のサーバに対するテストを行う
 1. どの構成管理ツールを使っているかに関わらず、テストを行える



RSpecの強力な語彙とカスタマイズ性をフルに活かして、Puppetがそうであるような宣言的なボキャブラリを提供することで、ちょっとしたルールとマッチャを憶えさえすれば自然に使えるようになり、習得は非常に容易です。


また、Puppetのmanifestレベルでのテストではなく、manifestが適用された後の、実際の状態をテストするため、より現実の状態に近いテストが可能です。また、そのことにより、Puppetのみならず、Chefやその他のツールによって構築したサーバのテストにも使える、汎用的なツールとなっています。

== SSHログインの設定


さっそく、これまで書いてきたmanifestを適用したサーバに対するテストを書いていきましょう。


serverspecは、対象ホストに対してテストを実行する方法を複数用意しています。今回は、テスト対象のホスト(ここではVagrantの仮想ホスト)にSSHログインし、コマンドを実行することでサーバ状態をテストする方法を採ります。まずは、Vagrantの仮想ホストに@<tt>{ssh}コマンドでログインできるよう設定しましょう。


今回のテスト対象は、前章までに引き続き、td-agentのクラスタです。@<tt>{Vagrantfile}のあるディレクトリに移動します。

//cmd{
$ cd puppet/cluster
//}


Vagrantfileでは、以下の通り設定がされています。

 * @<tt>{app}ロールのホスト名はapp.puppet-book.local
 * @<tt>{log}ロールのホスト名はlog.puppet-book.local



ホストOS側から、それらのホスト名で仮想ホストにアクセスできるようにします。まずは@<tt>{app}ロールから設定します。

//emlist{
$ vagrant ssh-config app --host app.puppet-book.local >> ~/.ssh/config
//}


上記コマンドの実行により、@<tt>{~/.ssh/config}に、以下の通り設定が追加されているはずです。

//emlist{
Host app.puppet-book.local
  HostName 127.0.0.1
  User vagrant
  Port 2222
  UserKnownHostsFile /dev/null
  StrictHostKeyChecking no
  PasswordAuthentication no
  IdentityFile "/Users/kentaro/.vagrant.d/insecure_private_key"
  IdentitiesOnly yes
  LogLevel FATAL
//}


ログインできるか確かめてみましょう。

//cmd{
$ ssh app.puppet-book.local
Last login: Mon Apr 29 10:59:02 2013 from 10.0.2.2
Welcome to your Vagrant-built virtual machine.
[vagrant@app ~]$
//}


ちゃんとログインできたようです。@<tt>{log}ロールの仮想ホストに対しても、同じように設定してください。

== serverspecのセットアップ


まずはホストOSに、@<tt>{gem}コマンドでserverspecをインストールします。

//cmd{
$ gem install serverspec
//}


@<tt>{serverspec-init}コマンドで、初期設定ファイルを作成します。プロンプトで環境についてたずねられるので、適宜解答してください。

//cmd{
$ serverspec-init
Select a backend type:

  1) SSH
  2) Exec (local)
  3) Puppet providers (local)

Select number: 1

Input target host name: app.puppet-book.local

Select OS type of target host:

  1) Auto Detect
  2) Red Hat
  3) Debian
  4) Gentoo
  5) Solaris

Select number: 2

 + spec/
 + spec/app.puppet-book.local/
 + spec/app.puppet-book.local/httpd_spec.rb
 + spec/spec_helper.rb
 + Rakefile
//}


コマンドの実行が終わると、上記のようにいくつかのファイルが作成されます。

== nginxのserverspecを書く


それでは、さっそくserverspecを書いていきましょう。今回は、nginxの状態をテストしてみます。


我々は先に、nginxのmanifestを書いたわけですが、そこではどういう状態が記述されていたでしょうか。リストアップしてみましょう。

 * @<tt>{nginx}というパッケージがインストールされていること
 * システム起動時に、nginxも起動するよう設定されていること
 * nginxが起動した状態にあること
 * nginxが80番ポートでリクエストを待ち受けていること
 * @<tt>{/etc/nginx/conf.d/my.conf}というファイルが存在すること
 * @<tt>{/usr/share/nginx/html/index.html}というファイルが存在すること



ざっと思い出してみるだけでも、これぐらいは挙げられます。これらの状態それぞれについて、テストを書いていきます。


@<tt>{serverspec-init}コマンドは、便利なことに、@<tt>{spec/app.puppet-book.local/httpd_spec.rb}という、テストファイルのひな形も作成してくれています。これを参考に、nginxのテストを書いていきましょう。

== マッチャ


serverspecでは、マッチャ(matcher)という、サーバの個々の状態をテストするためのメソッドのようなものを使って、テストを記述していきます。File/Directory、Package、Serviceなど、Puppetのresource typeに似た分類がなされた上で、下記ドキュメントに整理されています。


@<href>{http://serverspec.org/matchers.html,http://serverspec.org/matchers.html}


まずは「@<tt>{nginx}というパッケージがインストールされていること」という状態をテストします。以降のテストコードは、@<tt>{spec/app.puppet-book.local/nginx_spec.rb}に書いていきます。

//emlist{
require 'spec_helper'

describe 'nginx' do
  it { should be_installed }
end
//}


ご覧の通り、直感的な記法で状態をテストできます。さっそく実行してみましょう(テストを実行する前に、仮想ホスト側で@<tt>{puppet apply}コマンドを実行しておいてください)。

//emlist{
$ rspec spec/app.puppet-book.local/nginx_spec.rb
.

Finished in 0.1286 seconds
1 example, 0 failures
//}


テストが通りました。続いて、

 * システム起動時に、nginxも起動するよう設定されていること
 * nginxが起動した状態にあること



という状態をテストしましょう。先の記述につけたします。

//emlist{
describe 'nginx' do
  it { should be_installed }
  it { should be_enabled   } # 追加する
  it { should be_running   } # 追加する
end
//}


再度、テストを実行します。

//cmd{
$ rspec spec/app.puppet-book.local/nginx_spec.rb
...

Finished in 0.17448 seconds
3 examples, 0 failures
//}


このようにして、先にリストアップした「システムのあるべき状態」を、serverspecに落としていきます。その結果は、以下の通りです。

//emlist{
require 'spec_helper'

describe 'nginx' do
  it { should be_installed }
  it { should be_enabled   }
  it { should be_running   }
end

describe 'port 80' do
  it { should be_listening }
end

describe '/etc/nginx/conf.d/my.conf' do
  it { should be_file }
  it { should contain "server_name  app.puppet-book.local;" }
end

describe '/usr/share/nginx/html/index.html' do
  it { should be_file }
  it { should contain "Hello, Puppet!" }
end
//}


あらためて、テストを実行してみます。

//cmd{
$ rspec spec/app.puppet-book.local/nginx_spec.rb
........

Finished in 0.2695 seconds
8 examples, 0 failures
//}


ちゃんと通ったようですね。これで、我々が先に@<tt>{puppet apply}によって適用した状態が、serverspecによる外部からの状態テストによっても正しいことが確認できました。

== テストが失敗したら？


テストが失敗した時、どういうことが起きるのでしょうか。確かめてみましょう。システムが誤った状態にあることを、故意にnginxを落とすことでエミュレートしてみます。

//cmd{
$ vagrant ssh app
[vagrant@app ~]$ sudo service nginx stop
Stopping nginx:                                            [  OK  ]
//}


この状態でテストを実行するとどうなるでしょうか。

//cmd{
$ rspec spec/app.puppet-book.local/nginx_spec.rb
..FF....

Failures:

  1) nginx
     Failure/Error: it { should be_running   }
       expected "nginx" to be running
     # ./spec/app.puppet-book.local/nginx_spec.rb:6:in `block (2 levels) in <top (required)>'

  2) port 80
     Failure/Error: it { should be_listening }
       expected "port 80" to be listening
     # ./spec/app.puppet-book.local/nginx_spec.rb:10:in `block (2 levels) in <top (required)>'

Finished in 0.29048 seconds
8 examples, 2 failures

Failed examples:

rspec ./spec/app.puppet-book.local/nginx_spec.rb:6 # nginx
rspec ./spec/app.puppet-book.local/nginx_spec.rb:10 # port 80
//}


なにやらいろいろといわれていますね。

 * nginxが起動した状態にあること
 * nginxが80番ポートでリクエストを待ち受けていること



の2点について、システム状態が正常でないようです。nginxを故意に止めたのだから当然です。このように、なんらかの原因でシステム状態が正しくない状態に陥ってしまった場合、テストを実行することで検知することができます。

== システムのテストがなぜ必要か


筆者の勤務先では、大量にあるサーバの状態を、膨大なmanifestによって記述・管理しています。アプリケーションエンジニアとオペレーションエンジニアを問わず、必要であれば誰もがmanifestを変更し、システムに反映します。本章の冒頭で述べた通り、その状態は複雑で、また、アプリケーションへの新機能の追加、社会の変化への対応にともない、その内実は刻々と変化していきます。


そうした、多人数による大規模なシステム構築は、プログラミング言語によるアプリケーション開発と、複雑さと変化への対応が必要であるという意味において、まったく変わるところはありません。そのため、serverspecのような、システムの状態をテストするための仕組みが生み出されたのです。


manifestが複雑なものになってくると、moduleやロール間の関係が入り組んできて、おもむろに変更を加えたところ、既存の状態を壊してしまうことがよくあります。


筆者の勤務先では、単に開発者の手元でテストを実行するというところから一歩すすんで、serverspecを継続的に実行する環境を構築し、運用しています。筆者もしょっちゅう、継続テストに助けられています。


システム状態の継続テストについては、前述のserverspec作者の宮下剛輔氏の以下のブログエントリをご覧ください。


@<href>{http://mizzy.org/blog/2013/03/27/1/,Ukigumo と serverspec で Puppet の継続的インテグレーション}


前章までに作成したクラスタについて、残りの状態についてもぜひ、読者自身の手でテストを書いてみてください。

== まとめ


本章では以下のことを学びました。

 * serverspecは、複雑さと変化に対応するための仕組み
 * serverspecの使い方
 * アプリケーション同様、システム状態についてもテストが必要であること



システム状態をプログラマティカルにテストするserverspecは、始まったばかりのまだまだ新しい取り組みです。是非みなさんの開発の現場で取り入れて、確実なシステム構築に役立てるとともに、プロジェクトにフィードバックを送っていきましょう。
