## 第6章 パッケージをインストールする - package

前章まででmanifest作成・適用の流れをひと通りおさえたところで、本章からはしばらく、よく使うresource typeについてざっと見ていきましょう。

まず最初に、パッケージのインストール状態を記述するのに使う`package`について説明します。

[http://docs.puppetlabs.com/references/latest/type.html#package](http://docs.puppetlabs.com/references/latest/type.html#package)

### Package

第4章で、zshをインストールするのに以下のように書きました。

```
package { 'zsh':
  ensure => installed,
}
```

これだけの記述で、プラットフォームがRHEL系なら`yum`コマンドが、Debian系なら`apt`コマンドが自動的に選択され、パッケージのインストールが行なわれます。環境の差異にあわせ、resourceの状態を実際に担保する仕組みを、providerといいます。

### `ensure`で状態を記述する

ここでひとつ念頭に留めておきたいのは、`package`はパッケージをインストールするためだけに使われるものではないということです。resource typeは、あくまでもresourceの「状態」を記述するものであって、その「状態」には、インストールされていることの他に、「インストールされていないこと」という場合もあり得ます。

ここで、どういう状態であるべきかを指定しているのが`ensure`というattributeです。この場合は、`installed`に指定されています。他には、

  * `present`: `installed`と同じ
  * `absent`: インストールされていない
  * `latest`: 常に最新のものがインストールされている

という状態を記述できます。

### パッケージをまとめて記述したい

インストールしたいパッケージが複数ある際、そのいちいちについて前述のように書くのは面倒です。Puppetのresource typeは、その名前として配列を受け取ることができるので、この場合、以下のように書けます。

```
package {
  [
    'gcc',
    'gcc-c++',
    'kernel-devel',
    'make',
  ]:
  ensure => installed,
}
```

この「配列」は、Puppet言語でいう[Array](http://docs.puppetlabs.com/puppet/3/reference/lang_datatypes.html#arrays)のことです。変数を使って、以下のようにも書けます。

```
$packages = [
  'gcc',
  'gcc-c++',
  'kernel-devel',
  'make',
]

package { $packages:
  ensure => installed,
}
```

また、上記のような複数のパッケージがインストールされている状態に依存する他のresource typeがある場合、上記のように変数を使っておくと以下のように`require`を書けて、依存関係の記述が楽になります。

```
require => Package[$packages]
```

このような書き方を、condensed formと呼びます。`package`に限らず、全てのresource typeにおいて、同様の記法を使えます。詳しくは、以下のドキュメントを参照してください。

[http://docs.puppetlabs.com/puppet/latest/reference/lang_resources.html#condensed-forms](http://docs.puppetlabs.com/puppet/latest/reference/lang_resources.html#condensed-forms)

### ファイルからインストールしたい

以下のように、`source`でファイル名を、`provider`で`rpm`を指定します。

```
package { 'supervisor':
  ensure   => installed,
  source   => '/path/to/supervisor-3.0a12-2.el6.noarch.rpm',
  provider => 'rpm',
}
```

通常は`yum`でパッケージをインストールし、一部、独自のパッケージを使いたいという場合などに使えるでしょう。

### まとめ

本章では以下のことを学びました。

  * パッケージの状態記述のためのresource typeである`package`の使い方
  * `ensure`で状態を保証する
  * 同じようなresource typeをまとめて記述するcondensed form
  * ファイルからインストールする方法

パッケージのインストールは、システム構築の基本中の基本ですね。`package`は頻繁に使うresource typeです。
