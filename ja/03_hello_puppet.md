## 第3章: Puppetを試す - Hello Puppet!

試験環境も整ったところで、さっそくPuppetを始めましょう。本章では、定番の"Hello, World!"をPuppetで実行してみます。さらに、もうすこし実践的な例として、Puppetを使ってパッケージをインストールする方法を紹介します。

まずはざっくりと、Puppetを使う最初の一歩を、簡単な例から始めることにしましょう。

### Puppetのインストール

実は、前章で準備したVagrantの仮想ホストには、最初からPuppetがインストールされているので、Puppetを特別にインストールする必要はありません。`vagrant ssh`で仮想ホストにログインして、確かめてみましょう。

```
$ vagrant ssh
Last login: Wed Apr  3 14:16:35 2013 from 10.0.2.2
Welcome to your Vagrant-built virtual machine.
[vagrant@puppet-book ~]$ puppet --version
3.1.0
```

バージョン3.1.0が既に入っているのを確認できます。

以下、本書では、`$`のみで始まる行はホストOSでのコマンド実行、`[vagrant@puppet-book ~]$`で始まる行(`~`の部分は現在いるディレクトリを表すので変化します)は仮想ホストでのコマンド実行を示します。

これをそのまま使ってもいいのですが、本章執筆時点での最新バージョン3.1.1を使うために、Rubyのライブラリを管理する`gem`コマンドによってインストールしてみましょう。

```
[vagrant@puppet-book ~]$ sudo gem install puppet
Successfully installed facter-1.6.18
Successfully installed json_pure-1.7.7
Successfully installed hiera-1.1.2
Successfully installed puppet-3.1.1
4 gems installed
Installing ri documentation for facter-1.6.18...
Installing ri documentation for json_pure-1.7.7...
Installing ri documentation for hiera-1.1.2...
Installing ri documentation for puppet-3.1.1...
Installing RDoc documentation for facter-1.6.18...
Installing RDoc documentation for json_pure-1.7.7...
Installing RDoc documentation for hiera-1.1.2...
Installing RDoc documentation for puppet-3.1.1...
```

Vagrantで起動した仮想ホストでは、デフォルトのログインユーザである`vagrant`ユーザが、パスワードの入力なしに`sudo`コマンドを実行できるよう設定されています。

これで、本書執筆時点で最新の3.1.1がインストールされました。

```
[vagrant@puppet-book ~]$ puppet --version
3.1.1
```

### manifestを置く場所

さて、さっそくmanifestを書いていきましょう。とはいっても、どこにファイルを置けばいいのでしょうか。ここでもVagrantがその便利さを発揮します。

仮想ホストにログインした状態で、`/vagrant`ディレクトリに移動し、`ls`コマンドを実行してみてください。

```
[vagrant@puppet-book ~]$ cd /vagrant
[vagrant@puppet-book vagrant]$ ls
Vagrantfile
```

`Vagrantfile`が見えますね。Vagrantは、特に指定しなくても、仮想ホストの`/vagrant`ディレクトリに、ホストOS上の`Vagrantfile`のあるディレクトリをマウントしてくれます。

これからmanifestを書いていく際は、ホストOS上でファイルの編集を行い、Puppetの実行のみを仮想ホスト上で行うことにします。そうすることで、ファイルの編集をホストOS上でいつも使っているエディタで行いつつ、Puppetの実行は仮想ホストで、といったことが可能になり、とても便利です。

### manifestを書いてみる

まずはホストOS上で、manifestを置くためのディレクトリを作成しましょう。なお、このディレクトリの作成は必須ではなく、単純に、のちの章で作成するものとわけて置くほうが整理されていいだろうというだけの理由です。

```
$ mkdir -p puppet/04
$ cd puppet/04/
```

次に、以下の内容を`hello_world.pp`というファイル名で作成します。

```
notice("Hello, World!")
```

このように、Puppetのmanifestは、`.pp`という拡張子をつけることになっています。

### Puppetを実行する

このmanifestを実行してみましょう。今度は、仮想ホスト上でmanifestファイルのあるディレクトリに移動してから、`puppet apply`コマンドを実行します。

```
[vagrant@puppet-book vagrant]$ cd puppet/04/
[vagrant@puppet-book 04]$ puppet apply hello_puppet.pp
Notice: Scope(Class[main]): Hello, World!
Notice: Finished catalog run in 0.03 seconds
```

"Hello, World!"と表示されています。簡単ですね。

### パッケージをインストールする

さて、"Hello, World!"の表示からもう一歩すすんで、今度は実際にシステムの状態を変更する操作を行ってみましょう。ここではzshを、Puppetを使ってインストールします。

さきほどの`hello_world.pp`に、以下の内容を追記してください。

```
package { 'zsh':
  ensure => installed,
}
```

もう一度、`puppet apply`を実行します。今度は、ログの表示だけではなく、システムへの変更(具体的には`yum`コマンドによるパッケージのインストール)も行うので、`sudo`をつけます。

```
[vagrant@puppet-book 04]$ sudo puppet apply hello_puppet.pp
Notice: Scope(Class[main]): Hello, World!
Notice: /Stage[main]//Package[zsh]/ensure: created
Notice: Finished catalog run in 22.57 seconds
```

意図した通り、zshパッケージがインストールされたようですね。

```
[vagrant@puppet-book 04]$ which zsh
/bin/zsh
```

ここで注目したいのは、PuppetによってCentOS上でパッケージをインストールするに際して、`yum`コマンドのような、プラットフォーム固有のコマンドを指定していないということです。

Puppetには、RAL(Resouce Abstraction Layer)という仕組みがあり、プラットフォーム固有の事情を抽象化することで、差異を吸収してくれます。

### manifestの作成の流れ

このように、Puppetでmanifestを作成する流れは、

  1. 適当な場所にmanifestを書く
  2. manifestをシステムに適用する

という流れを繰り返していくことに他なりません。のちのちの説明では、さらに規模の大きなmanifestを書いていくために様々な記述方法を説明していきますが、本質的には上記の流れをくりかえしていくだけということに変わりありません。

どんどん書いて、どんどん`apply`していきましょう。

### Function

さて、今回書いてみたmanifestについて、その中身をもうすこしくわしく見ていきましょう。"Hello, World!"を出力する箇所は、こういう内容でしたね。

```
notice("Hello, World!")
```

この`notice`は、一般的な言語でいうところの「関数」と同じ働きをするもので、Puppetではfunctionと呼ばれています。この`notice`のように、Puppetはあらかじめいくつかのfuntionを提供しています。どのようなものがあるのか、その一覧については、以下のドキュメントをご参照ください。

[http://docs.puppetlabs.com/references/latest/function.html](http://docs.puppetlabs.com/references/latest/function.html)

### Resouce Type

zshパッケージをインストールする箇所は、こうでした。

```
package { 'zsh':
  ensure => installed,
}
```

Puppetでは、manifestによって記述する「システムのあるべき状態」を構成するひとつひとつを、resouceと呼びます。ここでは`package`とそれに続く記述によって、zshパッケージのインストールを行ってます。この場合の`package`を、resourceの具体的な種類という意味で、resouce typeと呼びます。

resource typeには、`package`以外にも、たとえば`file`や`user`など、システムの構成要素の多様性に応じて、たくさんのものが用意されています。どのようなものがあるのか、その一覧については、以下のドキュメントをご参照ください。

[http://docs.puppetlabs.com/references/latest/type.html](http://docs.puppetlabs.com/references/latest/type.html)

また、ここで`'zsh':`と記述されている箇所をtitle、`ensure => installed`をattributesといいます。titleは、resource typeを一意に決定するための名前を、attributesはどういう状態であるべきかを記述するために使われます。

### まとめ

本章では、実際にPuppetのmanifestを書いて、"Hello, World!"を表示してみたり、パッケージをインストールしてみたりすることで、ひととおりのmanifestの作成から、システムへの適用の流れまでを体験してみました。

また、今回書いたmanifestには、functionやresource typeという、Puppet言語の構成要素が使われていることも学びました。このように本書では、Puppet言語の構成要素をまとめて網羅的に解説するというよりもむしろ、必要に応じて解説していきます。その方が、記憶への定着が行われやすいだろうからです。
