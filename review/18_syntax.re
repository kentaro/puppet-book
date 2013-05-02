
= 第18章 最低限必要なPuppet言語の構文を学ぶ


前章で記述した@<tt>{td-agent::plugin}には、これまで説明してこなかったいくつかの言語要素が含まれていました。本章では、その内容に沿って、manifestを書く上で最低限必要となるだろう構文について見ていきます。

== 前章のおさらい


前章で作成した@<tt>{td-agent::plugin}は、以下の通りでした。

//emlist{
define td-agent::plugin (
  $ensure = 'present'
) {
  $plugin_name = "fluent-plugin-${title}"       #=> (1), (2), (3)

  Exec {                                        #=> (4)
    path => '/bin:/usr/lib64/fluent/ruby/bin',
  }

  if $ensure == 'present' {                     #=> (5)
    exec { "fluent-gem install ${plugin_name}":
      command => "fluent-gem install ${plugin_name}",
      unless  => "fluent-gem list | grep ${plugin_name} 2>/dev/null",
    }
  }
  elsif $ensure == 'absent' {
    exec { "fluent-gem uninstall ${plugin_name}":
      command => "fluent-gem uninstall ${plugin_name}",
      onlyif  => "fluent-gem list | grep ${plugin_name} 2>/dev/null",
    }
  }
  else {
    fail "${ensure} for `ensure` is not supported!"
  }
}
//}


ここに現れる言語要素のうち、これまでに説明してこなかったものとして、

 1. 変数とデータ型
 1. 文字列中の変数展開
 1. 変数のスコープ
 1. resourceのデフォルト値
 1. 条件分岐



が挙げられます。以下、それぞれについて簡単に見ていきましょう。

== 変数とデータ型


変数については、第5章のnginxのmanifestで、以下のような記述がありました。

//emlist{
$port = 80
//}


変数はこのように、なんらかの値を代入することで宣言されます。変数に代入可能な値には、以下のものがあります。

 * 真偽値
 * 未定義値
 * 文字列
 * 数値
 * 配列
 * ハッシュ



このうち、文字列、数値、配列については、既に使いました。実際、それらのデータ型を変数に代入することがよく行われます。データ型の詳細については、以下のドキュメントを参照してください。


@<href>{http://docs.puppetlabs.com/puppet/latest/reference/lang_datatypes.html,http://docs.puppetlabs.com/puppet/latest/reference/lang_datatypes.html}

== 文字列中の変数展開


Puppetの言語では、文字列リテラル中の変数展開(interpolation)が可能です。スクリプト言語によく見られるのと同様の機能です。先の例だと、以下のようになっていました。

//emlist{
$plugin_name = "fluent-plugin-${title}"
//}


この場合、@<tt>{$title}に@<tt>{extract_query_params}が格納された状態で、@<tt>{$plugin_name}に@<tt>{$title}が展開された値、すなわち、@<tt>{fluent-plugin-extract_query_params}が代入されたのでした。


ちなみに、この箇所は以下のように書いても同じです。

//emlist{
$plugin_name = "fluent-plugin-$title"
//}


ただし、変数展開する際は@<tt>{{}と@<tt>{\}}で必ず囲むことが、Style Guideで推奨されています。


@<href>{http://docs.puppetlabs.com/guides/style_guide.html#quoting,http://docs.puppetlabs.com/guides/style_guide.html#quoting}

== 変数のスコープ


変数のスコープ(変数が参照できる範囲)には、以下の3つがあります。

 1. トップスコープの変数
 1. 現在のローカルスコープ内の変数
 1. 他のローカルスコープ内の変数



@<href>{http://docs.puppetlabs.com/puppet/latest/reference/lang_scope.html,スコープについてのドキュメント}に掲げられている、以下の図の通りです。


//image[18-scope][スコープの概略図]{
//}


図中にはNode Scopeについても描かれていますが、nodeは本書の範囲外なので、ここではスコープの種類を3つとしています。


以下のような@<tt>{scope.pp}というファイルがあったとします。これを@<tt>{puppet apply scope.pp}として実行する場合、@<tt>{$top}はトップスコープに属することになります。

//emlist{
$top = "Hello, Puppet!"

class foo {
  notice($top) #=> "Hello, Puppet!"
}

include foo
//}


トップスコープとは、その名の通り、manifest中のどこからでも参照できる変数です。第9章で、templateに文字列を埋め込む際にその名前が出てきたのを憶えているでしょうか。

//emlist{
$to_be_overwritten = "top"

class bar {
  $local = "Hello, Puppet!"
  notice($local)               #=> "Hello, Puppet!"

  $to_be_overwritten = "local"
  notice($to_be_overwritten)   #=> "local"
}

include bar

notice($local)                 #=> undef
notice($to_be_overwritten)     #=> "top"
//}


現在のローカルスコープとは、上記における@<tt>{bar}というclassの中のような、現在の文脈におけるスコープのことです。@<tt>{bar}の中で定義された@<tt>{$local}は、トップレベルのスコープからは参照できません(未定義の変数となる)。


また、@<tt>{$to_be_overwritten}という変数に見られるように、トップスコープの変数を現在のスコープで上書きすると、現在のスコープの値が優先されます。しかし、トップスコープでは元の値のままです。

//emlist{
class baz {
  $local = "baz"
}

class hoge {
  include baz
  notice($baz::local) #=> "baz"
}

include hoge
//}


上記のように、他のクラス内で定義された変数も、@<tt>{::}で修飾して参照することができます。ただし、代入することはできません。

== resourceのデフォルト値


@<tt>{td-agent::plugin}には、以下のような記述がありました。

//emlist{
Exec {
  path => '/bin:/usr/lib64/fluent/ruby/bin',
}
//}


@<tt>{td-agent::plugin}には、@<tt>{exec}が2回出てきます。そのそれぞれについて、同じ内容のattributeを指定するのはDRYではありません。


そこで、第5章で紹介したresource referenceを用い、resource type名の先頭を大文字にして@<tt>{Exec { ... \}}と書くことで、それ以降に現れる@<tt>{exec}に対してattributeのデフォルト値を指定し、記述を省略できます。


「それ以降に現れる」とは、正確には「同じスコープに属する」という意味です。そのスコープは、前述の、変数のスコープと同じです。

== 条件分岐


Puppetには、いくつかの条件分岐のための構文が用意されています。

//emlist{
if $ensure == 'present' {
  ...
}
elsif $ensure == 'absent' {
  ...
}
else {
  ...
}
//}


最もよく使うのは、@<tt>{td-agent::plugin}でも使った上記のような@<tt>{if}/@<tt>{elsif}/@<tt>{else}文でしょう。これは、@<tt>{case}文を使って以下のようにも書き換えられます。

//emlist{
case $ensure {
  'present': { ... }
  'absent' : { ... }
  default  : { ... }
}
//}


条件が多くなってくると、@<tt>{elsif}が増えて可読性を損うので、@<tt>{case}文を使うのがよいでしょう。


その他、詳細についてはドキュメントを参照してください。


@<href>{http://docs.puppetlabs.com/puppet/latest/reference/lang_conditional.html,http://docs.puppetlabs.com/puppet/latest/reference/lang_conditional.html}

== まとめ


本章では以下のことを学びました。

 * 変数とデータ型
 * 文字列中の変数展開
 * 変数のスコープ
 * resourceのデフォルト値
 * 条件分岐



Puppetの言語は、複雑なシステムの記述に耐えうるよう、たくさんの機能を持った言語です。しかし、これまでの章、および、本章で学んだ程度の知識があれば、だいたいの必要はまかなえるはずです。manifestを書いていく上で、必要に応じてドキュメントにあたりながら憶えていけば、十分事足ります。


@<href>{http://docs.puppetlabs.com/puppet/latest/reference/lang_visual_index.html,http://docs.puppetlabs.com/puppet/latest/reference/lang_visual_index.html}
