## 第6章: yumリポジトリを登録したい - yumrepo

今回は、OS標準のもの以外のyumリポジトリを利用したい場合に使う
`yumrepo`について紹介します。

[http://docs.puppetlabs.com/references/latest/type.html#yumrepo](http://docs.puppetlabs.com/references/latest/type.html#yumrepo)

### Package

第4章で、nginxの公式サイトで提供されているyumリポジトリを指定するため、以下のように書きました。

```
yumrepo { 'nginx':
  descr    => 'nginx yum repository',
  baseurl  => 'http://nginx.org/packages/centos/$releasever/$basearch/',
  enabled  => 1,
  gpgcheck => 0
}
```

この記述により、`/etc/yum.repos.d/nginx.repo`に、以下の内容でリポジトリが追加されます。

```
[nginx]
name=nginx yum repository
baseurl=http://nginx.org/packages/centos/$releasever/$basearch/
enabled=1
gpgcheck=0
```

### EPELを登録する

さて今度は、RHELの標準に含まれない追加のパッケージを提供するプロジェクト[EPEL](http://fedoraproject.org/wiki/EPEL)のリポジトリを登録してみましょう。

```
yumrepo { 'epel':
  descr      => 'epel repo',
  mirrorlist => 'http://mirrors.fedoraproject.org/mirrorlist?repo=epel-$releasever&arch=$basearch',
  enabled    => 1,
  gpgcheck   => 1,
  gpgkey     => 'https://fedoraproject.org/static/0608B895.txt',
}
```

EPELのように、複数のミラーサーバのリストを提供しているリポジトリの場合は、nginxのリポジトリを登録した時と異なり、`mirrorlist`でそのリストのURLを指定します。また、`gpgcheck`を1にし、`gpgkey`を指定することで、万が一、ミラーサーバでパッケージが書き換えられてしまうようなことがあった場合、危険なパッケージがインストールされないよう、設定しておきましょう。

GPGキーのリストは、以下のURLから取得できます。

[https://fedoraproject.org/keys](https://fedoraproject.org/keys)

### まとめ

本章では、yumリポジトリのシステムへの登録状態を記述する`yumrepo`について見ていきました。通常の利用には、本章の説明で十分です。より細かい制御を必要とする場合は、`man yum.conf`および[`yumrepo`のドキュメント](http://docs.puppetlabs.com/references/latest/type.html#yumrepo)をご参照ください。
