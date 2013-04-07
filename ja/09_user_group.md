## 第9章: ユーザやグループを作成したい - user/group

ユーザやグループの状態を記述するには、それぞれ`user`と`group`というresource typeを使います。

  * [http://docs.puppetlabs.com/references/latest/type.html#user](http://docs.puppetlabs.com/references/latest/type.html#user)
  * [http://docs.puppetlabs.com/references/latest/type.html#group](http://docs.puppetlabs.com/references/latest/type.html#group)

### User

さっそくユーザを作成してみましょう。例によって、今回のmanifest用のディレクトリを用意します。
```
$ cd puppet/
$ mkdir -p user_group
$ cd user_group/
```

以下の内容で、`user.pp`というファイルを作成してください。

```
user { 'kentaro':
  ensure     => present,
  comment    => 'kentaro',
  home       => '/home/kentaro',
  managehome => true,
  shell      => '/bin/bash',
}
```

`pupply appet`で適用してみましょう。

```
[vagrant@puppet-book user_group]$ sudo puppet apply user.pp
Notice: /Stage[main]//User[kentaro]/ensure: created
Notice: Finished catalog run in 0.11 seconds
```

以下の通りユーザが作成されたのが確認できます。

```
[vagrant@puppet-book user_group]$ cat /etc/passwd | grep kentaro
kentaro:x:502:503:kentaro:/home/kentaro:/bin/bash
```

### managehome

ここではattributeのうち`managehome`について補足することにします。他のattributeについては、見ての通りの意味で、自明でしょう。詳細はドキュメントをご覧ください。

`home`では、単にそのユーザのホームディレクトリのパスを指定するだけですが、それに加えて`managehome => true`としておくと、ユーザの作成時にはそのディレクトリも同時に作成し、ユーザの削除時(`ensure => absent`)にはそのホームディレクトリも同時に削除します。ホームディレクトリが実際に必要な場合は、`managehome => true`も指定しておくとよいでしょう。

### Group

グループの状態記述には`group`を使います。以下の内容で、`group.pp`というファイルを作成してください。

```
group { 'developers':
  ensure => present,
  gid    => 999,
}
```

適用してみましょう。

```
[vagrant@puppet-book user_group]$ sudo puppet apply group.pp
Notice: /Stage[main]//Group[developers]/ensure: created
Notice: Finished catalog run in 0.07 seconds
```

以下の通りグループが作成されたのが確認できます。

```
[vagrant@puppet-book user_group]$ cat /etc/group | grep developers
developers:x:999:
```

### ユーザをグループに追加する

ユーザを作成し、かつ、あるグループに所属させたいとします。その場合、`gid`にグループ名、あるいは、グループIDを指定することになるのですが、Puppetはそのグループを定義したresourceを、自動的に`user`の定義への依存関係にあるものとして扱います。

以下の内容で、`add_user_to_group.pp`というファイルを作成してください。

```
user { 'antipop':
  ensure     => present,
  gid        => 'guest',
  comment    => 'antipop',
  home       => '/home/antipop',
  managehome => true,
  shell      => '/bin/bash',
}

group { 'guest':
  ensure => present,
  gid    => 1000,
}
```

上記では、antipopというユーザを作成し、かつ、guestというグループに追加された状態を記述しています。manifestを適用してみましょう。

```
[vagrant@puppet-book user_group]$ sudo puppet apply add_user_to_group.pp
Notice: /Stage[main]//Group[guest]/ensure: created
Notice: /Stage[main]//User[antipop]/ensure: created
Notice: Finished catalog run in 0.11 seconds
```

確認してみましょう。

```
[vagrant@puppet-book user_group]$ cat /etc/group | grep guest
guest:x:1000:antipop
```

ちゃんと追加されていますね。

 (ここ、ドキュメントでは追加されることになってるけど、追加されないんだが…)

### まとめ

本章では、システム状態の記述に欠かせないユーザやグループについて、Puppetでどのように記述するのかを学びました。
