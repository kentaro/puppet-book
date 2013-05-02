
= manifestの共通部分をくくりだす


manifestを書いていると、同じような記述を何度も書いていることに気付くことがあるでしょう。そのような場合、プログラミング言語では共通の処理を関数としてくくりだし、差異を引数として渡されるパラメタにより調整するということが行われます。


本章では、あたかもプログラミング言語における関数のようにmanifestの共通部分をくくりだし、再利用可能にする方法について説明します。

== Defined Type


先に「関数のように」と述べましたが、すこし不正確な表現です。Puppetにおける共通部分のくくりだしは、実際には、defined type(ユーザ定義resource type)と呼ばれる、あらたなresource typeの定義に他なりません。


@<href>{http://docs.puppetlabs.com/puppet/latest/reference/lang_defined_types.html,http://docs.puppetlabs.com/puppet/latest/reference/lang_defined_types.html}


とはいえ、ここでは言葉の定義には深入りしません。関数のようなものだと思って使っても、特に問題はないからです。

== td-agentのプラグイン


defined typeがどういうものなのか、どう役立つのかを知るためには、まずは実際にそれを定義し、使ってみるのがはやいでしょう。ここでは、前章までに記述したmanifestに、td-agentのプラグインをインストールする記述を追加することで、具体例を見ていきます。


今回は、@<href>{http://rubygems.org/gems/fluent-plugin-extract_query_params,fluent-plugin-extract_query_params}というプラグインを使ってみましょう。これは、指定したキーの指す値をURLとしてパースし、クエリパラメタを抽出して、それらを元のログのkey/valueとして追加するためのプラグインです。


具体的には、下記のようなログがあったとして、

//emlist{
{
  "url" : "http://example.com/?foo=bar&baz=qux
}
//}


@<tt>{url}というキーの指す値をURLとしてパースし、クエリパラメタを抽出し、ログを以下のように加工するというものです。

//emlist{
{
  "url" : "http://example.com/?foo=bar&baz=qux"
  "foo" : "bar",
  "baz" : "qux"
}
//}

== @<tt>{exec}を生で使うことの問題点


td-agentのプラグインをインストールするには、td-agentに同梱されている@<tt>{fluentd-gem}コマンドを使います。CentOSのx86_64アーキテクチャでは、@<tt>{/usr/lib64/fluent/ruby/bin}以下にインストールされているはずです。


さて、@<tt>{fluent-gem}コマンドを使ってプラグインをインストールするmanifestを書いていくわけですが、これまでの知識を使って書くなら、@<tt>{exec}を使ってこのように書くことになるでしょう。

//emlist{
exec { "fluent-gem install ${plugin_name}":
  path    => '/bin:/usr/lib64/fluent/ruby/bin',
  command => "fluent-gem install ${plugin_name}",
  unless  => "fluent-gem list | grep ${plugin_name} 2>/dev/null",
}
//}


単にプラグインをインストールする分には、これだけでも十分に用をなしますが、よりよいmanifestを書いていくには以下の点が問題となります。

 1. 「インストールされていないこと」という状態を記述したい場合にどうするか
 1. 冪等性をいちいち自分で担保するのは面倒
 1. プラグインのインストール状態自体をあらたなresource typeとして定義する方が再利用性が高い



これらの問題を、defined typeを使って解決していきましょう。

== defined typeを定義する


上記に挙げた問題は、主に@<tt>{exec}を生で使っていることに起因します。第11章で以下のように書かれていたことをおぼえているでしょうか。

//quote{
manifestを書いていて、@<tt>{exec}を多用しているなと感じたら、たいていはmanifestの書き方に問題があります。@<tt>{exec}を使わざるを得ない場合でも、「第17章 manifestの共通部分をくくりだす」で解説するdefined typeでラップして、できるだけ生でexecを使わないようにする方がよいでしょう。
//}


@<tt>{exec}は、便利な反面、自分で冪等性を担保する必要があり、その取り扱いが難しいresource typeです。また、@<tt>{exec}が多用されているmanifestは、ぱっと見で何をやっているのかを判別するのが困難です。define typeを用いて適切に名前づけしてやることで、manifestの可読性を向上させ、再利用性を高めることができます。


さっそく、td-agentプラグインのインストール状態を記述するためのdefined typeを書いていきましょう。これはどのロールからも利用され得るものですので、moduleに追加します。以下の内容で、@<tt>{modules/td-agent/manifests/plugin.pp}というファイルを作成してください。

//emlist{
define td-agent::plugin (
  $ensure = 'present'
) {
  $plugin_name = "fluent-plugin-${title}"

  Exec {
    path => '/bin:/usr/lib64/fluent/ruby/bin',
  }

  if $ensure == 'present' {
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


見慣れない記法がたくさんでてきましたね。このうちのいくつかは次章で説明しますので、いまは深く考えないようにしてください。とはいえ、その意味はだいたい想像がつくでしょう。ここでは、おさえておきたいふたつのことがらについてのみ説明します。


まずは、冒頭の記述について。

//emlist{
define td-agent::plugin (
  $ensure = 'present'
) {
  ...
}
//}


ここではふたつのことが行われています。まず、@<tt>{td-agent::plugin}という名前であらたなresource typeが定義されています。また、この名前とファイル名とを一致させる必要があるため、@<tt>{modules/td-agent/manifests/plugin.pp}というファイルに、このdefined typeを記述したのでした。


また@<tt>{$ensure = 'present'}という記述によって、このdefined typeを利用する時に使えるattributeを宣言しています。@<tt>{= 'present'}の箇所は、もし@<tt>{ensure}が指定されていなかった場合の、デフォルト値を示します。


ふたつめは、@<tt>{$title}という変数にresourceを宣言した時の名前が自動的に格納されるということです。具体的には、

//emlist{
td-agent::plugin { 'extract_query_params': }
//}


と書いた時、@<tt>{$title}には@<tt>{extract_query_params}という文字列が自動的に格納されます。あとの記述ではそれを用いて、コマンド文字列などを構築しています。

== @<tt>{td-agent::plugin}が行っていること


今回作成した@<tt>{td-agent::plugin}は、以下のことを行っています。

 1. プラグインのインストール状態に対応する、@<tt>{td-agent::plugin}というresource typeを定義する
 1. @<tt>{$ensure}の値によって、インストール/アンインストールという状態を条件分岐する
 1. それぞれについて、@<tt>{exec}を用いて、冪等性を担保しつつ状態を実現する
 1. もし、@<tt>{$ensure}の値として不適当なものがわたってきた場合、メッセージを表示してエラーとする



これらにより、先に挙げた3つの問題を全て解決します。つまり、

 1. @<tt>{$ensure}の値で条件分岐することで、インストール/アンインストール両方の状態に対応する
 1. defined typeの中に冪等性の担保を隠蔽することで、使う側がそれを意識する必要をなくす
 1. @<tt>{td-agent::plugin}というあらたなresource typeを定義することで、manifestの可読性と再利用性を高める



このように、一度defined typeとして定義しておけば、使う側は@<tt>{exec}の羅列に悩まされることなく、明確な名前に基づく簡単な記述のみで済むのです。

== defined typeを使う


さっそく@<tt>{td-agent::plugin}を使ってみましょう。@<tt>{log}ロールのtd-agentに、プラグインがインストールされた状態の記述を追加します。

//emlist{
$ cd puppet/cluster
//}


以下の内容で、@<tt>{roles/log/manifests/td-agent/plugin.pp}というファイルを作成してください。

//emlist{
class log::td-agent::plugin {
  td-agent::plugin { 'extract_query_params':
    ensure => present,
  }
}
//}


大元のclassで@<tt>{log::td-agent::plugin}を@<tt>{include}するため、@<tt>{roles/log/manifests/td-agent.pp}を以下のように変更します。

//emlist{
class log::td-agent {
  include ::td-agent
  include log::td-agent::config
  include log::td-agent::plugin

     Class['::td-agent::install']
  -> Class['log::td-agent::config']
  ~> Class['::td-agent::service']

     Class['::td-agent::install']
  -> Class['log::td-agent::plugin']
  ~> Class['::td-agent::service']
}
//}


プラグインのインストール状態が変更にともなってtd-agentを再起動する必要がありますので、refresh relationshipも定義しておきます。

== manifestを適用する


さて、今度はmanifestを適用してみましょう。前回同様、@<tt>{log}ロールで@<tt>{puppet apply}を実行するだけです。

//emlist{
[vagrant@log vagrant]$ sudo puppet apply --modulepath=modules:roles manifests/log.pp
Notice: /Stage[main]/Log::Td-agent::Plugin/Td-agent::Plugin[extract_query_params]/Exec[fluent-gem install fluent-plugin-extract_query_params]/returns: executed successfully
Notice: /Stage[main]/Td-agent::Service/Service[td-agent]: Triggered 'refresh' from 1 events
Notice: Finished catalog run in 28.99 seconds
//}


プラグインがインストールされているかどうか、@<tt>{fluent-gem list}コマンドで確認してみましょう。

//emlist{
[vagrant@log vagrant]$ /usr/lib64/fluent/ruby/bin/fluent-gem list fluent-plugin-extract_query_params
*** LOCAL GEMS ***

fluent-plugin-extract_query_params (0.0.2)
//}


冪等性が正しく担保されているかどうか、もう一度@<tt>{puppet apply}を実行することで確かめてみましょう。

//emlist{
[vagrant@log vagrant]$ sudo puppet apply --modulepath=modules:roles manifests/log.pp
Notice: Finished catalog run in 0.56 seconds
//}


既にプラグインがインストールされているので、今度はなにも出力されることなく、適用が終了しました。


さらに、アンインストールされている状態も正しく適用されるかどうか、確かめてみましょう。@<tt>{ensure => present}と書いた箇所を@<tt>{ensure => absent}に変更した上で、@<tt>{puppet apply}を実行します。

//emlist{
[vagrant@log vagrant]$ sudo puppet apply --modulepath=modules:roles manifests/log.pp
Notice: /Stage[main]/Log::Td-agent::Plugin/Td-agent::Plugin[extract_query_params]/Exec[fluent-gem uninstall fluent-plugin-extract_query_params]/returns: executed successfully
Notice: /Stage[main]/Td-agent::Service/Service[td-agent]: Triggered 'refresh' from 1 events
Notice: Finished catalog run in 1.41 seconds
//}


@<tt>{Exec[fluent-gem uninstall fluent-plugin-extract_query_params]}が実行されていることから、意図通りアンインストールが実行されたようです。

== プラグインの設定を追加する


せっかくfluent-plugin-extract_query_paramsプラグインをインストールしたので、defined typeとは関係ありませんが、そのための設定もmanifestに追加してみましょう。


@<tt>{roles/log/templates/td-agent/log.conf}を、以下のように変更します。

//emlist{
<source>
  type forward
  port 24224
</source>

<match forward.**>
  type   extract_query_params
  key    path
  except time, host, method, path, version, status, size, referer, ua, restime, ustime
  add_tag_prefix with_queries.
</match>

<match with_queries.**>
  type file
  path /var/log/td-agent/app/access
  time_slice_format %Y%m%d
</match>
//}


これまでは、送信されてきたログをそのままファイルに書き出していましたが、今回はその前に、extract_query_paramsプラグインで加工する処理を挟んでいます。再度@<tt>{sudo puppet apply --modulepath=modules:roles manifests/log.pp}を実行し、変更を適用してください。


前回同様に、動作確認をしてみましょう。今回は、アクセスするURLにクエリパラメタを付与します。@<tt>{app}ロールの仮想ホストから、以下の通りコマンドを実行します。

//emlist{
[vagrant@app vagrant]$ curl 'http://app.puppet-book.local/?foo=bar&baz=qux'
Hello, Puppet!
//}


今度は@<tt>{log}ロールの仮想ホストにログインします。

//emlist{
[vagrant@log vagrant]$ ls /var/log/td-agent/app/
access.20130421.b4dad900a93dc7767
//}


ログファイルが作成されています。中身を見てみましょう。

//emlist{
[vagrant@log vagrant]$ cat /var/log/td-agent/app/access.20130421.b4dad900a93dc7767
2013-04-21T06:24:19+00:00       with_queries.forward.app.access {"time":"21/Apr/2013:06:24:19 +0000","host":"127.0.0.1","method":"GET","path":"/?foo=bar&baz=qux","version":"HTTP/1.1","status":"200","size":"15","referer":"-","ua":"curl/7.19.7 (x86_64-redhat-linux-gnu) libcurl/7.19.7 NSS/3.13.6.0 zlib/1.2.3 libidn/1.18 libssh2/1.4.2","restime":"0.000","ustime":"-","foo":"bar","baz":"qux"}
//}


ログの末尾に@<tt>{"foo":"bar","baz":"qux"}と、クエリパラメタがあらたなkey/valueとして追加されているのが確認できますね。


これで、td-agentプラグインも自由に使えるようになりました。

== まとめ


本章では以下のことを学びました。

 * @<tt>{exec}を生で使うことの問題
 * defined typeによる解決
 * td-agentプラグインのインストール、設定方法



defined typeを積極的に使っていくことで、manifestの可読性を向上させ、再利用性を高めることができます。みなさんがこれからmanifestを書いていく上で同様の問題に直面したら、defined typeを使ってみることをおすすめします。
