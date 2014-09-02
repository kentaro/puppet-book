
= ユーザやグループを作成する - user/group


ユーザやグループの状態を記述するには、それぞれ@<tt>{user}と@<tt>{group}というresource typeを使います。

 * @<href>{http://docs.puppetlabs.com/references/latest/type.html#user,http://docs.puppetlabs.com/references/latest/type.html#user}
 * @<href>{http://docs.puppetlabs.com/references/latest/type.html#group,http://docs.puppetlabs.com/references/latest/type.html#group}


== User


さっそくユーザを作成してみましょう。例によって、今回のmanifest用のディレクトリを用意します。

//cmd{
$ cd puppet/
$ mkdir user_group
$ cd user_group/
//}


以下の内容で、@<tt>{user.pp}というファイルを作成してください。

//emlist{
user { 'kentaro':
  ensure     => present,
  comment    => 'kentaro',
  home       => '/home/kentaro',
  managehome => true,
  shell      => '/bin/bash',
}
//}


@<tt>{pupply appet}で適用してみましょう。

//cmd{
[vagrant@puppet-book user_group]$ sudo puppet apply user.pp
Notice: Compiled catalog for puppet-book.local in environment production in 0.10 seconds
Notice: /Stage[main]/Main/User[kentaro]/ensure: created
Notice: Finished catalog run in 0.09 seconds
//}


以下の通りユーザが作成されたのが確認できます。

//cmd{
[vagrant@puppet-book user_group]$ cat /etc/passwd | grep kentaro
kentaro:x:502:503:kentaro:/home/kentaro:/bin/bash
//}

== managehome


ここではattributeのうち@<tt>{managehome}について補足することにします。他のattributeについては、見ての通りの意味で、自明でしょう。詳細はドキュメントをご覧ください。


@<tt>{home}では、単にそのユーザのホームディレクトリのパスを指定するだけですが、それに加えて@<tt>{managehome => true}としておくと、ユーザの作成時にはそのディレクトリも同時に作成し、ユーザの削除時(@<tt>{ensure => absent})にはそのホームディレクトリも同時に削除します。ホームディレクトリが実際に必要な場合は、@<tt>{managehome => true}も指定しておくとよいでしょう。

== Group


グループの状態記述には@<tt>{group}を使います。以下の内容で、@<tt>{group.pp}というファイルを作成してください。

//emlist{
group { 'developers':
  ensure => present,
  gid    => 999,
}
//}


適用してみましょう。

//cmd{
[vagrant@puppet-book user_group]$ sudo puppet apply group.pp
Notice: Compiled catalog for puppet-book.local in environment production in 0.04 seconds
Notice: /Stage[main]/Main/Group[developers]/ensure: created
Notice: Finished catalog run in 0.07 seconds
//}


以下の通りグループが作成されたのが確認できます。

//cmd{
[vagrant@puppet-book user_group]$ cat /etc/group | grep developers
developers:x:999:
//}

== ユーザをグループに追加する


ユーザを作成し、かつ、あるグループに所属させたいとします。その場合、@<tt>{gid}にグループ名、あるいは、グループIDを指定することになるのですが、Puppetはそのグループを定義したresourceを、自動的に@<tt>{user}の定義への依存関係にあるものとして扱います(autorequireと呼ばれる機能です)。


以下の内容で、@<tt>{add_user_to_group.pp}というファイルを作成してください。

//emlist{
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
//}


上記では、antipopというユーザを作成し、かつ、guestというグループに追加された状態を記述しています。manifestを適用してみましょう。

//cmd{
[vagrant@puppet-book user_group]$ sudo puppet apply add_user_to_group.pp
Notice: Compiled catalog for puppet-book.local in environment production in 0.14 seconds
Notice: /Stage[main]/Main/Group[guest]/ensure: created
Notice: /Stage[main]/Main/User[antipop]/ensure: created
Notice: Finished catalog run in 0.09 seconds
//}


確認してみましょう。

//cmd{
[vagrant@puppet-book user_group]$ cat /etc/passwd | grep antipop
antipop:x:502:1000:antipop:/home/antipop:/bin/bash
//}


グループIDが1000で、ちゃんと追加されていますね。

== まとめ


本章では以下のことを学びました。

 * @<tt>{user}を使ってユーザの状態を記述する
 * @<tt>{group}を使ってグループの状態を定義する
 * ユーザをグループに追加する方法



ユーザやグループもまた、システム状態の記述に欠かせないresourceです。本章で学んだ知識は、頻繁に活用することになるでしょう。
