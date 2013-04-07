## 第8章: ファイルやディレクトリを扱いたい - file

システム状態の記述に欠かせないファイルやディレクトリの操作について、本章では見ていきましょう。`file`というresource typeを使います。

[http://docs.puppetlabs.com/references/latest/type.html#file](http://docs.puppetlabs.com/references/latest/type.html#file)

### File

第4章で、nginxの設定ファイルを配置するのに、以下のように書きました。

```
file { '/etc/nginx/conf.d/my.conf':
  ensure  => present,
  owner   => 'root',
  group   => 'root',
  mode    => '0644',
  content => template('my.conf'),
  require => Package['nginx'],
  notify  => Service['nginx'],
}
```

この例では、nginxの大元の設定ファイルである`/etc/nginx/nginx.conf`からロードされることを前提に、`/etc/nginx/conf.d/my.conf`に自分用にカスタマイズした設定を記述しました。

`owner`、`group`、`mode`は見ての通りの意味ですので、詳述するまでもないでしょう。ここでは`content => template('my.conf')`という箇所を見ていきます。

### ファイルの内容を文字列で指定する

`content`は、ファイルの内容を指定するために使われるattributeです。その値として、ファイルの内容を文字列で記述します。まずは、本章で作成するmanifestを置くためのディレクトリを作成しましょう。

```
$ cd puppet/
$ mkdir -p file
$ cd file/
```

このディレクトリで、以下の内容のファイルを、`content_string.pp`というファイル名で作成してください。

```
file { '/tmp/hello_puppet.txt':
  content => "Hello, Puppet!\n",
}
```

さっそく実行してみましょう。

```
[vagrant@puppet-book ~]$ cd /vagrant/puppet/file
[vagrant@puppet-book file]$ sudo puppet apply content_string.pp
Notice: /Stage[main]//File[/tmp/hello_puppet.txt]/ensure: defined content as '{md5}570be4af90660458e6c37633d5676ec2'Notice: Finished catalog run in 0.04 seconds
```

`cat`コマンドで中身を表示してみましょう。

```
[vagrant@puppet-book file]$ cat /tmp/hello_puppet.txt
Hello, Puppet!
```

意図通りに作成されたようです。

### テンプレートを使う

このように、`content`に文字列をわたしてやれば、任意の文字列をその内容に持つファイルを作成できるのですが、manifestにベタ書きしてしまうのは、保守性を考えるとおすすめできるやり方ではありません。そこで、別のファイルを読み込んで`content`にわたす方法を見ていきましょう。

先のnginxの設定では`content => template('my.conf'),`という指定があり、`puppet apply`実行時に`--templatedir=.`というオプションをわたしてやることで、同ディレクトリにある`my.conf`というファイルを読み込んで使っていました。

また、この読み込まれたファイルは、単なる文字列としてではなく、`template()`によりerbのテンプレートとして評価されるのでしたね。

以下の内容のファイルを、`content_template.pp`というファイル名で作成してください。

```
$content = "Hello, Puppet!"

file { '/tmp/hello_puppet_template.txt':
  content => template('hello_puppet_template.erb'),
}
```

次に、`hello_puppet_template.erb`という名前で、以下の内容のテンプレートファイルを作成します。

```
<%= content %>
```

今回は、`puppet apply`時に、テンプレートファイルを格納したディレクトリ(ここではカレントディレクトリ)を指定するため、`--templatedir=.`として、オプション引数をわたして実行します。

```
[vagrant@puppet-book file]$ sudo puppet apply --templatedir=.content_template.pp
Notice: /Stage[main]//File[/tmp/hello_puppet_template.txt]/ensure: defined content as '{md5}6f459c8c8efb17c22040ffd76a4335d6'
Notice: Finished catalog run in 0.04 seconds
```

`cat`コマンドで中身を表示してみましょう。

```
[vagrant@puppet-book file]$ cat /tmp/hello_puppet_template.txt
Hello, Puppet!
```

`template()`によってerbとして評価された文字列が`content`の値としてわたされ、意図通りにファイルが作成されました。

### 変数展開

`template()`は、引数としてわたされたファイル名をerbのテンプレートファイルとして評価すると述べました。その際、どのような変数がerbのコンテキストで参照できるのか、すこし見ていきましょう。以下の4つがあります。

  1. facts
  2. グローバルスコープの変数
  3. 現在のスコープ内の変数
  4. 他のスコープ内の変数

今回、ファイルの中身となる文字列を格納した`$content`という変数は、2のグローバルスコープの変数です。

1〜3の変数については、テンプレート内で以下のように`$`なしで参照できます。

```
<%= content %>
```

factsや変数のスコープについては、あとの章で説明します。ここでは、グローバルスコープの変数はerbテンプレートの中からも参照できるということだけ憶えておいてください。

erbの詳細については、以下の記事が簡便です。

[http://jp.rubyist.net/magazine/?0017-BundledLibraries](http://jp.rubyist.net/magazine/?0017-BundledLibraries)

ただし、Puppetで使う分には複雑なロジックを書くことはないでしょうので、`<%= ... %>`による変数展開の記法程度を憶えておくところから始めるので十分です。むしろ、テンプレートで複雑なロジックを書くのは、保守性を損うことになりますので、おすすめできません。

### source

本章では、`content`と、その中身を`template()`で生成するという説明しかしてきませんでしたが、ファイルの中身を別ファイルから取得し、配置する方法は他にもあります。`source`というattributeを使う方法です。

`template()`のように、erbを使って変数展開するまでもない、どのような状況でも内容が一定で変わらないファイルもあり得ます。そこで`source`を使うと、その値として指定したURI(`puppet:///path/to/file`のような)から、そのままファイルをコピーしてくることができます。

しかし、本書では`source`を用いることはしません。

  1. テンプレートは、`puppet apply`時に`--templatedir`オプションで格納場所を指定できるため、取り扱いが容易である
  2. 変数展開をするかしないかに関わらず、`content => template('...')`で統一的に扱う方が見通しがいい

という理由によります。特に2について、最初の構築時には`source`で用が足りると判断しても、あとになって変数展開が必要になることはあり得ることです。最初から`content => template('...')`にしておくと、そのような場合も容易に対応可能です。

### ディレクトリとシンボリックリンク

`ensure`の値を変更することで、ディレクトリやシンボリックリンクの状態を記述することが可能です。

ディレクトリの場合、`ensure`に`directory`を指定します。

```
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
```

シンボリックリンクの場合、`ensure`に`link`を、`target`にリンク元を指定します。

```
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
```

以下のように、シンボリックリンクが作成できました。

```
[vagrant@puppet-book file]$ ls -la /etc/nginx/site-enabled/
total 8
drwxr-xr-x 2 root root 4096 Apr  7 07:18 .
drwxr-xr-x 5 root root 4096 Apr  7 07:16 ..
lrwxrwxrwx 1 root root   37 Apr  7 07:18 mysite.conf -> /etc/nginx/site-available/mysite.conf
```

### まとめ

本章では、ファイルやディレクトリの状態を記述する`file`について見てきました。また、ファイルの内容を直接文字列で指定する方法と、テンプレートを使って動的に生成する方法とを紹介しました。

`file`を使うと、ディレクトリやシンボリックリンクについても同様に扱うことができます。数あるPuppetのresource typeでも、最も活用頻度の高いものでしょう。
