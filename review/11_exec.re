
= 任意のコマンドを実行する - exec


Puppetは@<href>{http://docs.puppetlabs.com/references/latest/type.html,ドキュメント}
にある通り、たくさんのresource typeを標準で用意していますが、時にはそれでも足りないこともあります。@<tt>{exec}を使うと、それら標準のresource typeだけではできないことが、任意のコマンドを実行することで可能となります。


@<href>{http://docs.puppetlabs.com/references/latest/type.html#exec,http://docs.puppetlabs.com/references/latest/type.html#exec}

== Exec


ここでは、@<href>{https://github.com/tagomoris/xbuild,xbuild}という、様々な言語のビルドスクリプトをラップし、それらの言語を簡単にビルドできるようにしたツールを使うことを想定して、manifestを書いてみましょう。


今回のmanifest用のディレクトリを用意します。

//cmd{
$ cd puppet/
$ mkdir exec
$ cd exec/
//}


以下の内容で、@<tt>{xbuild.pp}というファイルを作成してください。xbuildを@<tt>{git clone}した上で、Rubyのバージョン2.1.2をビルドします。

//emlist{
package { 'git': }
package { 'openssl-devel': }

exec { 'xbuild':
  user    => 'vagrant',
  cwd     => '/home/vagrant',
  path    => ['/usr/bin'],
  command => 'git clone git://github.com/tagomoris/xbuild.git local/xbuild',
  creates => '/home/vagrant/local/xbuild',
  require => Package['git', 'openssl-devel'],
}

exec { 'xbuild ruby':
  user        => 'vagrant',
  cwd         => '/home/vagrant',
  environment => ['USER=vagrant'],
  path        => ['/bin', '/usr/bin', '/home/vagrant/local/xbuild'],
  command     => 'ruby-install 2.1.2 /home/vagrant/local/ruby-2.1.2',
  creates     => '/home/vagrant/local/ruby-2.1.2',
  timeout     => 0,
  require     => Exec['xbuild'],
}
//}


適用してみましょう。

//cmd{
[vagrant@puppet-book ~]$ cd /vagrant/puppet/exec/
[vagrant@puppet-book exec]$ sudo puppet apply xbuild.pp
Notice: Compiled catalog for puppet-book.local in environment production in 0.28 seconds
Notice: /Stage[main]/Main/Package[openssl-devel]/ensure: created
Notice: /Stage[main]/Main/Exec[xbuild]/returns: executed successfully
Notice: /Stage[main]/Main/Exec[xbuild ruby]/returns: executed successfully
Notice: Finished catalog run in 481.78 seconds
//}


以下の通り、指定したバージョンのRubyがインストールされたことが確認できます。

//cmd{
[vagrant@puppet-book exec]$ /home/vagrant/local/ruby-2.1.2/bin/ruby -v
ruby 2.1.2p95 (2014-05-08 revision 45877) [x86_64-linux]
//}

== コマンドの実行ユーザ・グループ


コマンドは、@<tt>{user}で指定したユーザ、あるいは、@<tt>{group}で指定したグループの権限で実行されます。いずれも指定されていない場合は、rootユーザの権限で実行されます。状況に応じて、適切に指定するようにしてください。


また、@<tt>{cwd}で指定したディレクトリをカレントディレクトリとしてコマンドが実行されます。

== environmentとpath


@<tt>{exec}では、環境変数やコマンドのサーチパスが空の状態でコマンドが実行されます。そのため、@<tt>{environment}で適切な環境変数を指定し、@<tt>{path}でサーチパスを指定する、あるいは、コマンドをフルパスで書く必要があります。


@<tt>{environment}は、以下のように配列で指定します。

//emlist{
environment => ['USER=vagrant'],
//}


@<tt>{path}は、以下のように配列で指定しても、

//emlist{
path => ['/bin', '/usr/bin', '/home/vagrant/local/xbuild'],
//}


@<tt>{:}で区切った文字列として指定しても、同じように扱われます。

//emlist{
path => '/bin:/usr/bin:/home/vagrant/local/xbuild',
//}


@<tt>{environment}で環境変数@<tt>{PATH}を指定した場合、@<tt>{path}で指定した内容が上書きされます。注意してください。

== 冪等性を担保する


@<tt>{exec}はなんでもできる反面、冪等性を自分で担保しなければなりません。すなわち、上記のmanifestを再度実行した場合、

//cmd{
[vagrant@puppet-book exec]$ sudo puppet apply xbuild.pp
Notice: Finished catalog run in 0.54 seconds
//}


と、再度実行されない状態にしなければなりません。下記の方法のいずれかによって、必ず冪等性を担保するようにしましょう。


実行するコマンドが、なんらかのファイルやディレクトリを作成するようなものである場合、@<tt>{create}でそのファイル/ディレクトリを指定することで、もしそれらが存在する場合は実行しないことにより、冪等性を担保できます。上記のxbuildの例では、以下のように書きました。

//emlist{
creates => '/home/vagrant/local/xbuild',
//}


その他、@<tt>{onlyif}や@<tt>{unless}によって、コマンドを実行し、その終了ステータスを見ることで、冪等性を担保する方法もあります。

 * @<tt>{onlyif}: 指定したコマンドが終了ステータス0を返した場合に限って実行する
 * @<tt>{unless}: 指定したコマンドが終了ステータス0以外を返した場合に限って実行する



上記の@<tt>{create}は、以下のように@<tt>{unless}を使って置きかえられます。

//emlist{
unless => 'test -d /home/vagrant/local/xbuild'
//}

== まとめ


本章では以下のことを学びました。

 * 任意のコマンドを実行することのできる@<tt>{exec}の使い方
 * @<tt>{creates}、@<tt>{onlyif}、@<tt>{unless}を使って冪等性を担保する方法



@<tt>{exec}は便利な反面、冪等性を自分で担保しなければならない、扱いの難しいresource typeです。


manifestを書いていて、@<tt>{exec}を多用しているなと感じたら、たいていはmanifestの書き方に問題があります。@<tt>{exec}を使わざるを得ない場合でも、「第17章 manifestの共通部分をくくりだす」で解説するdefined typeでラップして、できるだけ生でexecを使わないようにする方がよいでしょう。
