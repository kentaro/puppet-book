## 第10章: 任意のコマンドを実行したい - exec

Puppetは
[ドキュメント](http://docs.puppetlabs.com/references/latest/type.html)
にある通り、たくさんのresource typeを標準で用意していますが、時にはそれでも足りないこともあるでしょう。`exec`を使うと、それら標準のresource typeだけではできないことが、任意のコマンドを実行することで可能となります。

[http://docs.puppetlabs.com/references/latest/type.html#exec](http://docs.puppetlabs.com/references/latest/type.html#exec)

### Exec

ここでは、[xbuild](https://github.com/tagomoris/xbuild)という、様々な言語のビルドスクリプトをラップし、それらの言語を簡単にビルドできるようにしたツールを使うことを想定して、manifestを書いてみましょう。

今回のmanifest用のディレクトリを用意します。

```
$ cd puppet/
$ mkdir -p exec
$ cd exec/
```

以下の内容で、`xbuild.pp`というファイルを作成してください。xbuildを`git clone`した上で、Rubyのバージョン2.0.0-p0をビルドします。

```
package { 'git': }

exec { 'xbuild':
  user    => 'vagrant',
  cwd     => '/home/vagrant',
  path    => ['/usr/bin'],
  command => 'git clone git://github.com/tagomoris/xbuild.git local/xbuild',
  creates => '/home/vagrant/local/xbuild',
  require => Package['git'],
}

exec { 'xbuild ruby':
  user        => 'vagrant',
  cwd         => '/home/vagrant',
  environment => ['USER=vagrant'],
  path        => ['/bin', '/usr/bin', '/home/vagrant/local/xbuild'],
  command     => 'ruby-install 2.0.0-p0 /home/vagrant/local/ruby-2.0.0-p0',
  creates     => '/home/vagrant/local/ruby-2.0.0-p0',
  timeout     => 0,
  require     => Exec['xbuild'],
}
```

適用してみましょう。

```
[vagrant@puppet-book ~]$ cd /vagrant/puppet/exec/
[vagrant@puppet-book exec]$ sudo puppet apply xbuild.pp
Notice: /Stage[main]//Exec[xbuild]/returns: executed successfully
Notice: /Stage[main]//Exec[xbuild ruby]/returns: executed successfully
Notice: Finished catalog run in 800.58 seconds
```

以下の通り、指定したバージョンのRubyがインストールされたことが確認できます。

```
[vagrant@puppet-book exec]$ /home/vagrant/local/ruby-2.0.0-p0/bin/ruby -v
ruby 2.0.0p0 (2013-02-24 revision 39474) [x86_64-linux]
```

### コマンドの実行ユーザ・グループ

コマンドは、`user`で指定したユーザ、あるいは、`group`で指定したグループの権限で実行されます。いずれも指定されていない場合は、rootユーザの権限で実行されます。状況に応じて、適切に指定すうようにしてください。

また、`cwd`で指定したディレクトリをカレントディレクトリとしてコマンドが実行されます。

### environmentとpath

`exec`では、環境変数やコマンドのサーチパスが空の状態で、コマンドが実行されます。そのため、`environment`で適切な環境変数を指定し、`path`でサーチパスを指定する、あるいは、コマンドをフルパスで書く必要があります。

`environment`は、以下のように配列で指定します。

```
environment => ['USER=vagrant'],
```

`path`は、以下のように配列で指定しても、

```
path => ['/bin', '/usr/bin', '/home/vagrant/local/xbuild'],
```

`:`で区切った文字列として指定しても、同じように扱われます。

```
path => '/bin:/usr/bin:/home/vagrant/local/xbuild',
```

`environment`で環境変数`PATH`を指定した場合、`path`で指定した内容が上書きされます。注意してください。

### 冪等性を担保する

`exec`はなんでもできる反面、冪等性を自分で担保しなければなりません。すなわち、上記のmanifestを再度実行した場合、

```
[vagrant@puppet-book exec]$ sudo puppet apply xbuild.pp
Notice: Finished catalog run in 0.54 seconds
```

と、再度実行されない状態にしなければなりません。下記の方法のいずれかによって、必ず冪等性を担保するようにしましょう。

実行するコマンドが、なんらかのファイルやディレクトリを作成するようなものである場合、`create`でそのファイル/ディレクトリを指定することで、もしそれらが存在する場合は実行しないことで、冪等性を担保できます。上記のxbuildの例では、以下のように書きました。

```
creates => '/home/vagrant/local/xbuild',
```

その他、`onlyif`や`unless`によって、コマンドを実行し、その終了ステータスを見ることで、冪等性を担保する方法もあります。

  * `onlyif`: 指定したコマンドが終了ステータス0を返した場合に限って実行する
  * `unless`: 指定したコマンドが終了ステータス0以外を返した場合に限って実行する

上記の`create`は、以下のように`unless`を使って置きかえられます。

```
unless => 'test -d /home/vagrant/local/xbuild'
```

### まとめ

本章では、任意のコマンドを実行できる`exec`について学びました。便利な反面、冪等性を自分で担保しなければならない、扱いの難しいresource typeです。

manifestを書いていて、`exec`を多用しているなと感じたら、たいていはmanifestの書き方に問題があります。`exec`を使わざるを得ない場合でも、第○章で解説するdefinitionでラップして、できるだけ生でexecを使わないようにする方がよいでしょう。
