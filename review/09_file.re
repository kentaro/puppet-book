
= ファイルやディレクトリを作成する - file


システム状態の記述に欠かせないファイルやディレクトリの操作について、本章では見ていきましょう。@<tt>{file}というresource typeを使います。


@<href>{http://docs.puppetlabs.com/references/latest/type.html#file,http://docs.puppetlabs.com/references/latest/type.html#file}

== File


第5章で、nginxの設定ファイルを配置するのに、以下のように書きました。

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


この例では、nginxの大元の設定ファイルである@<tt>{/etc/nginx/nginx.conf}からロードされることを前提に、@<tt>{/etc/nginx/conf.d/my.conf}に自分用にカスタマイズした設定を記述しました。


@<tt>{owner}、@<tt>{group}、@<tt>{mode}は見ての通りの意味ですので、詳述するまでもないでしょう。ここでは@<tt>{content => template('my.conf')}という箇所を見ていきます。

== ファイルの内容を文字列で指定する


@<tt>{content}は、ファイルの内容を指定するために使われるattributeです。その値として、ファイルの内容を文字列で記述します。まずは、本章で作成するmanifestを置くためのディレクトリを作成しましょう。

//emlist{
$ cd puppet/
$ mkdir file
$ cd file/
//}


このディレクトリで、以下の内容のファイルを、@<tt>{content_string.pp}というファイル名で作成してください。

//emlist{
file { '/tmp/hello_puppet.txt':
  content => "Hello, Puppet!\n",
}
//}


さっそく実行してみましょう。

//cmd{
[vagrant@puppet-book ~]$ cd /vagrant/puppet/file/
[vagrant@puppet-book file]$ sudo puppet apply content_string.pp
Notice: Compiled catalog for puppet-book.local in environment production in 0.07 seconds
Notice: /Stage[main]/Main/File[/tmp/hello_puppet.txt]/ensure: defined content as '{md5}1db16ebfb21d376e5b2ae9d1eaf5b3c8'
Notice: Finished catalog run in 0.03 seconds
//}


@<tt>{cat}コマンドで中身を表示してみましょう。

//emlist{
[vagrant@puppet-book file]$ cat /tmp/hello_puppet.txt
Hello, Puppet!
//}


意図通りに作成されたようです。

== テンプレートを使う


このように、@<tt>{content}に文字列をわたしてやれば、任意の文字列をその内容に持つファイルを作成できるのですが、manifestにベタ書きしてしまうのは、保守性を考えるとおすすめできるやり方ではありません。そこで、別のファイルを読み込んで@<tt>{content}にわたす方法を見ていきましょう。


先のnginxの設定では@<tt>{content => template('my.conf')}という指定があり、@<tt>{puppet apply}実行時に@<tt>{--templatedir=.}というオプションをわたしてやることで、同ディレクトリにある@<tt>{my.conf}というテンプレートファイルを読み込んでいました。


また、この読み込まれたファイルは、単なる文字列としてではなく、@<tt>{template()}によりerbのテンプレートとして評価されるのでしたね。


以下の内容のファイルを、@<tt>{content_template.pp}というファイル名で作成してください。

//emlist{
$content = "Hello, Puppet!"

file { '/tmp/hello_puppet_template.txt':
  content => template('hello_puppet_template.erb'),
}
//}


次に、@<tt>{hello_puppet_template.erb}という名前で、以下の内容のテンプレートファイルを作成します。

//emlist{
<%= @content %>
//}


今回は、@<tt>{puppet apply}時に、テンプレートファイルを格納したディレクトリ(ここではカレントディレクトリ)を指定するため、@<tt>{--templatedir=.}として、オプション引数をわたして実行します。

//cmd{
[vagrant@puppet-book file]$ sudo puppet apply --templatedir=. content_template.pp
Notice: Compiled catalog for puppet-book.local in environment production in 0.06 seconds
Notice: /Stage[main]/Main/File[/tmp/hello_puppet_template.txt]/ensure: defined content as '{md5}1db16ebfb21d376e5b2ae9d1eaf5b3c8'
Notice: Finished catalog run in 0.03 seconds
//}


@<tt>{cat}コマンドで中身を表示してみましょう。

//emlist{
[vagrant@puppet-book file]$ cat /tmp/hello_puppet_template.txt
Hello, Puppet!
//}


@<tt>{template()}によってerbとして評価された文字列が@<tt>{content}の値としてわたされ、意図通りにファイルが作成されました。

== 変数展開


@<tt>{template()}は、引数としてわたされたファイル名をerbのテンプレートファイルとして評価すると述べました。その際、どのような変数がerbのコンテキストで参照できるのか、すこし見ていきましょう。以下の3つがあります。

 1. トップスコープの変数
 1. 現在のローカルスコープ内の変数
 1. 他のローカルスコープ内の変数



今回、ファイルの中身となる文字列を格納した@<tt>{$content}という変数は、1のトップスコープの変数です。


1と2の変数については、テンプレート内で以下のように@<tt>{@}付きで参照できます。

//emlist{
<%= @content %>
//}


変数のスコープについては、「第18章 最低限必要なPuppet言語の構文を学ぶ」で説明します。ここでは、トップスコープの変数はerbテンプレートの中からも参照できるということだけ憶えておいてください。


erbの詳細については、以下の記事が簡便です。


@<href>{http://jp.rubyist.net/magazine/?0017-BundledLibraries,http://jp.rubyist.net/magazine/?0017-BundledLibraries}


ただし、Puppetで使う分には複雑なロジックを書くことはないでしょうので、@<tt>{<%= ... %>}による変数展開の記法程度を憶えておくところから始めるので十分です。むしろ、テンプレートで複雑なロジックを書くのは、保守性を損うことになりますので、おすすめできません。

== source


本章では、@<tt>{content}と、その中身を@<tt>{template()}で生成するという説明しかしてきませんでしたが、ファイルの中身を別ファイルから取得し、配置する方法は他にもあります。@<tt>{source}というattributeを使う方法です。


@<tt>{template()}のように、erbを使って変数展開するまでもない、どのような状況でも内容が一定で変わらないファイルもあり得ます。そこで@<tt>{source}を使うと、その値として指定したURI(@<tt>{puppet:///path/to/file}のような)から、そのままファイルをコピーしてくることができます。


しかし、本書では@<tt>{source}を用いることはしません。

 1. テンプレートは、@<tt>{puppet apply}時に@<tt>{--templatedir}オプションで格納場所を指定できるため、取り扱いが容易である
 1. 変数展開をするかしないかに関わらず、@<tt>{content => template('...')}で統一的に扱う方が見通しがいい



という理由によります。特に2について、最初の構築時には@<tt>{source}で用が足りると判断しても、あとになって変数展開が必要になることはあり得ることです。最初から@<tt>{content => template('...')}にしておくと、そのような場合も容易に対応可能です。

== ディレクトリとシンボリックリンク


@<tt>{ensure}の値を変更することで、ディレクトリやシンボリックリンクの状態を記述することが可能です。


ディレクトリの場合、@<tt>{ensure}に@<tt>{directory}を指定します。

//emlist{
file { '/etc/nginx/site-available':
  ensure => directory,
  owner  => 'root',
  group  => 'root',
  mode   => '0755',
}

file { '/etc/nginx/site-enabled':
  ensure => directory,
  owner  => 'root',
  group  => 'root',
  mode   => '0755',
}
//}


シンボリックリンクの場合、@<tt>{ensure}に@<tt>{link}を、@<tt>{target}にリンク元を指定します。

//emlist{
file { '/etc/nginx/site-available/mysite.conf':
  ensure => present,
  owner  => 'root',
  group  => 'root',
  mode   => '0644',
}

file { '/etc/nginx/site-enabled/mysite.conf':
  ensure => link,
  target => '/etc/nginx/site-available/mysite.conf',
  owner  => 'root',
  group  => 'root',
  mode   => '0644',
  require => File['/etc/nginx/site-available/mysite.conf'],
}
//}


以下のように、シンボリックリンクが作成できました。

//cmd{
[vagrant@puppet-book file]$ ls -la /etc/nginx/site-enabled/
total 8
drwxr-xr-x 2 root root 4096 Apr  7 07:18 .
drwxr-xr-x 5 root root 4096 Apr  7 07:16 ..
lrwxrwxrwx 1 root root   37 Apr  7 07:18 mysite.conf -> /etc/nginx/site-available/mysite.conf
//}

== まとめ


本章では以下のことを学びました。

 * @<tt>{file}でファイルの状態を記述する
 * @<tt>{template()}でerbテンプレートを評価し、変数を埋め込む
 * 通常のファイルの他、ディレクトリやシンボリックリンクの状態を記述する



@<tt>{file}は、数あるPuppetのresource typeでも、最も活用頻度の高いものでしょう。きっちり押さえておきましょう。
