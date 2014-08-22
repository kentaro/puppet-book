
= Hello, Puppet!


開発環境も整ったところで、さっそくPuppetを始めましょう。本章では、定番の"Hello, World!"をPuppetで実行してみます。さらに、もうすこし実践的な例として、Puppetを使ってパッケージをインストールする方法を紹介します。


まずはざっくりと、Puppetを使う最初の一歩を簡単な例から始めることにしましょう。

== Puppetのインストール


実は、前章で準備したVagrantの仮想ホストには、最初からPuppetがインストールされています。そのため、Puppetを特別にインストールする必要はありません。@<tt>{vagrant ssh}で仮想ホストにログインして、確かめてみましょう。

//cmd{
$ vagrant ssh
Welcome to your Vagrant-built virtual machine.
[vagrant@puppet-book ~]$ puppet --version
3.4.2
//}


本書執筆時点で最新のバージョンは3.6.2ですが、本書ではバージョン3.4.2のまま進めます。


以下、本書全体を通して、次の表記によりホストOS上と仮想ホスト上とでのコマンド実行を区別します(Vagrantを使わずに本書を読み進める場合は、適宜読み替えてください)。

 * @<tt>{$}のみで始まる行はホストOSでのコマンド実行
 * @<tt>{[vagrant@puppet-book ~]$}のように、@<tt>{[vagrant@ホスト名 ディレクトリ名]$}から始まる行は仮想ホストでのコマンド実行



本書の前提とする以外の、Puppetがインストールされていない環境では、以下のように@<tt>{gem}コマンドによってインストールできます。

//cmd{
[vagrant@puppet-book ~]$ sudo gem install puppet -v 3.4.2 --no-rdoc --no-ri
Successfully installed facter-1.7.6
Successfully installed json_pure-1.8.1
Successfully installed hiera-1.3.4
Successfully installed puppet-3.4.2
4 gems installed
//}


ちなみに、Vagrantで起動した仮想ホストでは、デフォルトのログインユーザである@<tt>{vagrant}ユーザが、パスワードの入力なしに@<tt>{sudo}コマンドを実行できるよう設定されています。

== manifestを置く場所


さて、さっそくmanifestを書いていきましょう。とはいっても、どこにファイルを置けばいいのでしょうか。ここでもVagrantがその便利さを発揮します。


仮想ホストにログインした状態で、@<tt>{/vagrant}ディレクトリに移動し、@<tt>{ls}コマンドを実行してみてください。

//cmd{
[vagrant@puppet-book ~]$ cd /vagrant
[vagrant@puppet-book vagrant]$ ls
Vagrantfile  puppet
//}


@<tt>{Vagrantfile}が見えますね。Vagrantは、特に指定しなくても、仮想ホストの@<tt>{/vagrant}ディレクトリに、ホストOS上の@<tt>{Vagrantfile}のあるディレクトリをマウントしてくれます。また、サンプルコードを@<tt>{git clone}した場合は、@<tt>{puppet}ディレクトリも見えているはずです。


これからmanifestを書いていく際は、ホストOS上でファイルの作成・編集を行い、Puppetの実行のみを仮想ホスト上で行うことにします。そうすることで、ファイルの編集をホストOS上でいつも使っているエディタで行いつつ、Puppetの実行は仮想ホストで、といったことが可能になり、とても便利です。

== manifestを書いてみる


まずはホストOS上で、manifestを置くためのディレクトリを作成しましょう。なお、このディレクトリの作成は必須ではなく、単純に、のちの章で作成するものとわけて置くほうが整理されていいだろうというだけの理由です。

//cmd{
$ mkdir -p puppet/hello_puppet
$ cd puppet/hello_puppet/
//}


次に、以下の内容で、@<tt>{hello_world.pp}というファイルを作成します。

//emlist{
notice("Hello, World!")
//}


このように、Puppetのmanifestファイルは、@<tt>{.pp}という拡張子をつけることになっています。

== manifestを適用する


このmanifestを適用してみましょう。今度は、仮想ホスト上でmanifestファイルのあるディレクトリに移動してから、@<tt>{puppet apply}コマンドを実行します。

//cmd{
[vagrant@puppet-book ~]$ cd /vagrant/puppet/hello_puppet/
[vagrant@puppet-book hello_puppet]$ puppet apply hello_world.pp
Notice: Scope(Class[main]): Hello, World!
Notice: Compiled catalog for puppet-book.local in environment production in 0.05 seconds
Notice: Finished catalog run in 0.02 seconds
//}


"Hello, World!"と表示されています。簡単ですね。

== パッケージをインストールする


さて、"Hello, World!"の表示からもう一歩すすんで、今度は実際にシステムの状態を変更する操作を行ってみましょう。ここではzshを、Puppetを使ってインストールします。


さきほどの@<tt>{hello_world.pp}と同階層に、以下の内容で@<tt>{zsh.pp}というファイルを作成してください。

//emlist{
package { 'zsh':
  ensure => installed,
}
//}


今度は、新しく作成したファイルを引数にして@<tt>{puppet apply}を実行します。今度は、ログの表示だけではなく、システムへの変更(@<tt>{yum}コマンドによるパッケージのインストール)も行うので、@<tt>{sudo}権限で実行する必要があります。

//cmd{
[vagrant@puppet-book hello_puppet]$ sudo puppet apply zsh.pp
Notice: Compiled catalog for puppet-book.local in environment production in 0.28 seconds
Notice: /Stage[main]/Main/Package[zsh]/ensure: created
Notice: Finished catalog run in 33.10 seconds
//}


意図した通り、zshパッケージがインストールされたようですね。

//cmd{
[vagrant@puppet-book hello_puppet]$ which zsh
/bin/zsh
//}


ここで注目したいのは、CentOS上でパッケージをインストールするに際して、@<tt>{yum}コマンドのような、プラットフォーム固有のコマンドを指定していないということです。


Puppetには、RAL(Resouce Abstraction Layer)という仕組みがあり、プラットフォーム固有の事情を抽象化することで、差異を吸収してくれます。この仕組みのおかげで、どのプラットフォームにmanifestを適用するかを気にすることなく、システム構成の記述という本質に注力することができるのです。

== manifestの作成・適用の流れ


このように、Puppetでmanifestを作成・適用する流れは、

 1. 適当な場所にmanifestを書く
 1. manifestをシステムに適用する



という流れを繰り返していくことに他なりません。のちのちの説明では、さらに規模の大きなmanifestを書いていくために様々な記述方法を説明していきますが、本質的には上記の流れをくりかえしていくだけということに変わりありません。


どんどん書いて、どんどん@<tt>{apply}していきましょう。

== Function


さて、今回書いてみたmanifestについて、その中身をもうすこしくわしく見ていきましょう。"Hello, World!"を出力する箇所は、こういう内容でしたね。

//emlist{
notice("Hello, World!")
//}


この@<tt>{notice}は、一般的な言語でいうところの「関数」と同じ働きをするもので、Puppetではfunctionと呼ばれています。この@<tt>{notice}のように、Puppetはあらかじめいくつかのfunctionを提供しています。どのようなものがあるのか、その一覧については、以下のドキュメントをご参照ください。


@<href>{http://docs.puppetlabs.com/references/latest/function.html,http://docs.puppetlabs.com/references/latest/function.html}

== Resouce Type


zshパッケージをインストールする箇所は、こうでした。

//emlist{
package { 'zsh':
  ensure => installed,
}
//}


Puppetでは、manifestによって記述する「システムのあるべき状態」を構成するひとつひとつを、resourceと呼びます。ここでは@<tt>{package}とそれに続く記述によって、zshパッケージのインストールを行ってます。この場合の@<tt>{package}を、resourceの具体的な種類という意味で、resource typeと呼びます。


resource typeには、@<tt>{package}以外にも、たとえば@<tt>{file}や@<tt>{user}など、システムの構成要素の多様性に応じて、たくさんのものが用意されています。どのようなものがあるのか、その一覧については、以下のドキュメントをご参照ください。


@<href>{http://docs.puppetlabs.com/references/latest/type.html,http://docs.puppetlabs.com/references/latest/type.html}


また、ここで@<tt>{'zsh':}と記述されている箇所をtitle、@<tt>{ensure => installed}をattributesといいます。titleは、resource typeを一意に決定するための名前を、attributesはどういう状態であるべきかを記述するために使われます。

== まとめ


本章では以下のことを学びました。

 * Puppetのインストール、バージョンの確認方法
 * manifest作成の流れ
 * プログラミングにおける関数のような働きをするfunction
 * 「システムのあるべき状態」の構成要素としてのresource type



実際にPuppetのmanifestを書いて、"Hello, World!"を表示してみたり、パッケージをインストールしてみたりすることで、ひととおりのmanifestの作成から、システムへの適用の流れまでを体験してみました。


また、今回書いたmanifestには、functionやresource typeという、Puppet言語の構成要素が使われていることも学びました。このように本書では、Puppet言語の構成要素をまとめて網羅的に解説するというよりもむしろ、必要に応じて解説していきます。その方が、記憶への定着が行われやすいだろうからです。
