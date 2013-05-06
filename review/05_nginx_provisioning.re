
= nginxのmanifestを書く


前章では、簡単なmanifestを作成し、システムへ適用することを通して、Puppetでの作業の流れを体験してみました。本章では、さらに実践的なmanifestの作成に挑戦してみましょう。


今回取り上げるのは、HTTPサーバの@<href>{http://nginx.org/,nginx}です。

== 「nginxが使える状態」とは？


CentOSにnginxをインストールし、実際にHTTPサーバとしてユーザからのリクエストに応答できるようにするまでのステップを、まずはあらためて考えてみましょう。

 1. 必要に応じて、yumパッケージを提供するリポジトリをシステムに登録する
 1. @<tt>{yum}コマンドを使ってnginxをインストールする
 1. 設定ファイルを記述する
 1. nginxを起動する
 1. nginxがシステム起動時に自動的に起動するよう設定する



「nginxが使える状態」であるといえるためには、最低限これぐらいのことをする必要があります。実際、みなさんがこれまでnginxをシステムにインストールしてきた際には、同様のことを行ってきたことでしょう。

== manifestに落とす


次に、上記の1〜5を、manifestに落としていきます。まずは、本章で作成するmanifestを置くためのディレクトリを作成しましょう。

//cmd{
$ cd puppet/
$ mkdir nginx
$ cd nginx/
//}


以下の内容で、@<tt>{nginx.pp}というファイルを作成してください。

//emlist{
yumrepo { 'nginx':
  descr    => 'nginx yum repository',
  baseurl  => 'http://nginx.org/packages/centos/6/$basearch/',
  enabled  => 1,
  gpgcheck => 0,
}

package { 'nginx':
  ensure  => installed,
  require => Yumrepo['nginx'],
}

$port = 80

file { '/etc/nginx/conf.d/my.conf':
  ensure  => present,
  owner   => 'root',
  group   => 'root',
  mode    => '0644',
  content => template('my.conf'),
  require => Package['nginx'],
  notify  => Service['nginx'],
}

$target = 'Puppet'

file { '/usr/share/nginx/html/index.html':
  ensure  => present,
  owner   => 'root',
  group   => 'root',
  mode    => '0644',
  content => template('index.html'),
  require => Package['nginx'],
}

service { 'nginx':
  enable     => true,
  ensure     => running,
  hasrestart => true,
  require    => File['/etc/nginx/conf.d/my.conf'],
}
//}

== manifestの内容


作成したmanifestを、上から見ていきましょう。@<tt>{package}については前章で紹介したので、省略します。今回はあらたに@<tt>{yumrepo}、@<tt>{file}、そして@<tt>{service}というresource typeが使われています。これらresource typeについてはあとの章でより詳しく見ていきますので、ここでは簡単な説明にとどめます。


@<href>{http://docs.puppetlabs.com/references/latest/type.html#yumrepo,yumrepo}は、システムへのyumリポジトリの登録状態を記述するためのresource typeです。ここでは@<href>{http://wiki.nginx.org/Install,nginxのインストールマニュアル}に掲載されている公式のyumリポジトリを登録し、使用可能な状態にしています。


ちなみに、ここでは@<tt>{gpgcheck}を@<tt>{0}とし、チェックしないよう設定していますが、これはnginxのリポジトリがGPGキーを提供していないためです。EPELのようにGPGキーを合わせて提供しているリポジトリを利用する場合は、極力GPGキーのチェックも行うほうがよいでしょう。詳しくは「第7章 yumリポジトリを登録する - yumrepo」で解説します。


@<href>{http://docs.puppetlabs.com/references/latest/type.html#file,file}は、ファイルやディレクトリに関するresource typeです。ファイルやディレクトリが、指定されたattribute通りに存在しする(あるいは存在しない)という状態を記述するために使います。@<tt>{owner}や@<tt>{group}などの各attributeがどのような意味を持つかは、想像がつくでしょう。@<tt>{template()}、および@<tt>{require}と@<tt>{notify}の各Attributeについては、後述します。


@<tt>{$port = 80}や@<tt>{$target = "Puppet"}という箇所は、見ての通り、変数への代入を行っています。Puppetではこのように、manifestの中で変数を使えます。後述の@<tt>{template()}といっしょに説明します。


@<href>{http://docs.puppetlabs.com/references/latest/type.html#service,service}は、今回の例のnginxのような、サービスを提供するデーモンのあるべき状態を記述するためのresource typeです。@<tt>{enable}は、システム起動時にサービスとして起動するかどうかを、@<tt>{ensure}は、常に起動した状態を保っているべきかどうかを記述するのに使います。nginxのようなデーモンは常に起動しておきたいのがふつうでしょうから、このように記述します。

== テンプレートを用意する


@<tt>{yum}コマンドによってインストールされるnginxは、@<tt>{/etc/nginx/conf.d/*.conf}をロードするよう設定されています。そこで、自分用にカスタマイズした設定を@<tt>{/etc/nginx/conf.d/my.conf}として配置し、自動的にロードされるようmanifestを記述しています。その箇所をもう一度見てみましょう。

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


ここでは@<tt>{template()}というfunctionを用いて、設定ファイルの内容を生成しています。ここであえて「生成」と述べたのにはわけがあります。@<tt>{template()}は、引数に渡されたファイルの内容を、Rubyのテンプレートエンジンerbの形式で書かれたものとして評価し、その結果として生成された文字列を返す関数だからです。


以下の内容で、@<tt>{my.conf}というファイルを@<tt>{nginx.pp}と同じディレクトリに作成してください。

//emlist{
server {
  listen       <%= port %>;
  server_name  localhost;

  location / {
    root  /usr/share/nginx/html;
    index index.html;
  }
}
//}


この@<tt>{<%= port %>}の箇所が、erbの変数展開の記法として評価され、manifestで@<tt>{$port = 80}と記述されている内容がうめこまれます。


同様に、@<tt>{index.html}というファイルを、以下の通り作成してください。

//emlist{
Hello, <%= target %>!
//}


このファイル内の@<tt>{<%= target %>}も、同名変数@<tt>{$target}の値に展開されます。

== manifestを適用する


さて、manifestを作成したら、システムに適用してみましょう。今回は前回と違ってテンプレートも使うので、@<tt>{puppet apply}コマンド実行時に、@<tt>{--templatedir}オプションでカレントディレクトリ(@<tt>{.})を指定します。

//cmd{
[vagrant@puppet-book ~]$ cd /vagrant/puppet/nginx
[vagrant@puppet-book nginx]$ sudo puppet apply --templatedir=. nginx.pp
Notice: /Stage[main]//Yumrepo[nginx]/descr: descr changed '' to 'nginx yum repository'
Notice: /Stage[main]//Yumrepo[nginx]/baseurl: baseurl changed '' to 'http://nginx.org/packages/centos/6/$basearch/'
Notice: /Stage[main]//Yumrepo[nginx]/enabled: enabled changed '' to '1'
Notice: /Stage[main]//Yumrepo[nginx]/gpgcheck: gpgcheck changed '' to '0'Notice: /Stage[main]//Package[nginx]/ensure: created
Notice: /Stage[main]//File[/usr/share/nginx/html/index.html]/content: content changed '{md5}e3eb0a1df437f3f97a64aca5952c8ea0' to '{md5}1db16ebfb21d376e5b2ae9d1eaf5b3c8'
Notice: /Stage[main]//File[/etc/nginx/conf.d/my.conf]/ensure: created
Notice: /Stage[main]//Service[nginx]/ensure: ensure changed 'stopped' to 'running'
Notice: /Stage[main]//Service[nginx]: Triggered 'refresh' from 1 events
Notice: Finished catalog run in 33.69 seconds
//}


エラーなく終了したら、nginxが実際に起動しているかどうか、確認してみましょう。

//cmd{
[vagrant@puppet-book nginx]$ curl http://localhost:80/
Hello, Puppet!
//}


変数に代入した値がきちんと展開された文字列が、nginxによって返されました。

== Relationship


さて、nginxのmanifestを無事に適用できたところで、説明を先送りにした@<tt>{require}と@<tt>{notify}について、見ていきましょう。


今回は、@<tt>{yumrepo}、@<tt>{package}、@<tt>{file}、@<tt>{service}という、4つのresource typeを使ってmanifestを記述しました。それぞれのresource typeによって記述されたresourceは、任意の順番で個別に存在するのはなく、次の順番で依存関係を持っています。

 1. yumリポジトリを登録する
 1. nginxパッケージをインストールする
 1. 設定ファイルを配置する
 1. サービスを起動する



1から4までが順番通りに実行されないと、正しくnginxを起動できません。このような依存関係のことをdependency relationshipと呼び、@<tt>{require}によって記述します。

//emlist{
package { 'nginx':
  ensure  => installed,
  require => Yumrepo['nginx'],
}
//}


これは、nginxパッケージをインストールする前に、nginxのyumリポジトリを登録する必要がある、ということを意味します。


「関係」にはもうひとつ種類があります。nginxのインストール時のみならず、nginxの設定ファイルを変更した場合にも、nginxにその設定を再読み込みさせる必要があります。このような、あるresourceの変更にともなって再読み込みのようなアクションが必要となる関係をrefresh relationshipと呼び、@<tt>{notify}によって記述します。

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


Puppetのmanifestは、ファイル内の記述順で処理が実行されるわけではありません。Puppet言語は、これまで何度も述べている通り「システムのあるべき状態」を記述するためのものですから、ファイル内での記述の順序によって実行順が変わるのは好ましくありません。今回行ったように、明示的にrelationshipを記述してやる必要があります。

== Resource Reference


上記で、@<tt>{package}を宣言する際にはすべて小文字で書いていた一方で、@<tt>{require}によって依存関係を記述する際には@<tt>{Package['nginx']}と、最初の1文字を大文字で書いていたことに気付いたでしょうか？


このように、resourceの宣言時にはすべて小文字を使い、relationshipのあるresourceへの参照時には、最初の文字を大文字にした名前を使います。他のresourceへの参照を、resource referenceと呼びます。宣言時はすべて小文字で、それ以外は最初の1文字を大文字にする、と憶えておいてください。

== まとめ


本章では、nginxのmanifestを作成することを通して、多くのことを学びました。

 * @<tt>{yumrepo}、@<tt>{file}、@<tt>{service}という、あらたなresource type
 * @<tt>{template()}によってerb形式のテンプレートが評価・展開されること
 * resource間のrelationshipを記述すること



すこし大変だったかもしれませんが、ここで学んだことをきちんと理解しておけば、Puppetの本質の、多くの部分を把握したことになります。本章をよく読み返してみて、いまいちど理解を確かめてみてください。
