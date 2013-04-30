# 第3章 Vagrantで開発環境を用意する

本書では、「はじめに」で述べたように、Puppetの実行環境としてLinux(Amazon LinuxとCentOS)を使用します。といっても、いますぐ手元に、自分の自由になるLinux環境を用意できないという読者も多いでしょう。

そこで、Puppetの実践に入る前に、まずはVagrantというツールを使って、開発環境を用意してみましょう。

### Vagrantとは？

[Vagrant](http://www.vagrantup.com/)は、[VirtualBox](https://www.virtualbox.org/)や[VMware](http://www.vmware.com/)、[Amazon EC2](http://aws.amazon.com/ec2/)といった仮想化ツールを、簡単にコントロールするためのラッパツールです。元々はVirtualBox専用に開発されたものでしたが、本書で使用するバージョン1.1以降は、[Providersという仕組み](http://docs.vagrantup.com/v2/providers/)を導入したことで、プラグインによってVirtualBox以外の仮想化ツールにも対応するようになりました。

本書では、これまでVagrantでの利用実績が豊富なVirtualBoxを使っていくことにします。

ちなみに、VagrantにはPuppetと連携できる機能があり、仮想ホストの起動時や起動後に、指定した設定をもとにmanifestを適用する仕組みがあります。しかし、本書は現場で使えるPuppet入門を目指しており、より実践的な方法を採るため、その機能は使いません。

### Vagrantのインストール

まずはVirtualBoxをインストールしましょう。[VirtualBoxのダウンロードページ](https://www.virtualbox.org/wiki/Downloads)からお使いの環境にあったパッケージをダウンロードし、インストーラの指示に従ってインストールしてください。

VirtualBoxのインストールが終わったら、今度はVagrantをインストールします。[Vagrantのダウンロードページ](http://downloads.vagrantup.com/)からお使いの環境にあったパッケージをダウンロードしてください。その際、本書の環境と合わせるため、1.1.0以上のバージョンをダウンロードするようにしてください。その後、インストーラの指示に従ってインストールします。

### 仮想ホストの起動

Vagrantで利用できる仮想ホストのひな形(boxといいます)は、有志により様々なディストリビューションのものが用意されています([http://www.vagrantbox.es/](http://www.vagrantbox.es/))。また、Puppet提供元のPuppet Labsからも、様々なディストリビューションのboxが提供されています([http://puppet-vagrant-boxes.puppetlabs.com/](http://puppet-vagrant-boxes.puppetlabs.com/))。

本書では、Puppet Labsが提供するCentOS 6.4のboxを利用します。

[http://puppet-vagrant-boxes.puppetlabs.com/centos-64-x64-vbox4210.box](http://puppet-vagrant-boxes.puppetlabs.com/centos-64-x64-vbox4210.box)

仮想ホストを起動するための設定は、非常に簡単です。適当なディレクトリで、以下のコマンドを実行します。

```
$ vagrant init
```

すると、コマンドを実行したディレクトリに`Vagrantfile`というファイルが作成されます。中を見てみるといろいろ書かれていますが、本書の範囲内で必要とする設定はわずかです。設定の詳細については[Vagrantのドキュメント](http://docs.vagrantup.com/v2/)を見ていただくとして、ここではまず、以下の内容に変更してください。

```ruby
Vagrant.configure("2") do |config|
  config.vm.box      = "centos-6.4-puppet"
  config.vm.box_url  = "http://puppet-vagrant-boxes.puppetlabs.com/centos-64-x64-vbox4210.box"
  config.vm.hostname = "puppet-book.local"
end
```

このファイルでは、以下の設定を行っています。

  * 仮想ホストに使用するbox名の指定
  * boxが存在しなかった場合に取得する先のURL
  * 仮想ホストのhostnameの指定

ファイルを作成したのと同じディレクトリで、`vagrant up`コマンドを実行すると、仮想ホストが起動します。

```
$ vagrant up
Bringing machine 'default' up with 'virtualbox' provider...
[default] Box 'centos-6.4-puppet' was not found. Fetching box from specified URL for
the provider 'virtualbox'. Note that if the URL does not have
a box for this provider, you should interrupt Vagrant now and add
the box yourself. Otherwise Vagrant will attempt to download the
full box prior to discovering this error.
Downloading with Vagrant::Downloaders::HTTP...
Downloading box: http://puppet-vagrant-boxes.puppetlabs.com/centos-64-x64-vbox4210.box
Extracting box...
Cleaning up downloaded box...
Successfully added box 'centos-6.4-puppet' with provider 'virtualbox'!
[default] Importing base box 'centos-6.4-puppet'...
[default] Matching MAC address for NAT networking...
[default] Setting the name of the VM...
[default] Clearing any previously set forwarded ports...
[default] Creating shared folders metadata...
[default] Clearing any previously set network interfaces...
[default] Preparing network interfaces based on configuration...
[default] Forwarding ports...
[default] -- 22 => 2222 (adapter 1)
[default] Booting VM...
[default] Waiting for VM to boot. This can take a few minutes.
[default] VM booted and ready for use!
[default] Setting hostname...
[default] Configuring and enabling network interfaces...
[default] Mounting shared folders...
[default] -- /vagrant
```

以下、本書の説明を通じて、`vagrant`コマンドは、`Vagrantfile`のあるディレクトリで実行してください。

初回実行時には、boxをダウンロードするために時間がかかりますが、次回からは既にダウンロードしたboxを使用するため、すぐに起動します。

### 仮想ホストにSSHログインする

以下のコマンドを実行すると、仮想ホストにSSHでログインできます。

```
$ vagrant ssh
```

また、のちの章では通常のsshコマンドによるログインが必要になりますので、以下のようにコマンドを実行して、準備しておいてください。

```
$ vagrant ssh-config --host puppet-book.local >> ~/.ssh/config
```

これで、いつものようにsshコマンドで仮想ホスト(ここではpuppet-book.localというホスト名を指定)にログインできます。

```
$ ssh puppet-book.local
```

ログインしたら、適当にコマンドを実行してみたりして、触ってみてください。ふだん使っているLinuxと変わらない環境が簡単にできてしまったことに、驚くことでしょう。

また、あれこれといじってみた結果、たとえ壊してしまったとしても、後述の通り簡単にリセットして元通りにできますので、安心です。

### 仮想ホストの停止・破棄

仮想ホストを停止するには`halt`サブコマンドを、破棄(リセット)するには`destroy`サブコマンドを使います。

```
$ vagrant halt
```

を実行すると一時停止、

```
$ vagrant destroy
```

で破棄(リセット)されます。再度、仮想ホストを起動したい場合は、最初と同じく

```
$ vagrant up
```

を実行してください。

Vagrantには、上記で紹介したものの他にもたくさんの便利なサブコマンドがありますが、本書の範囲内では、以上で十分です。より詳しく知りたい方は、[Vagrantのドキュメント](http://docs.vagrantup.com/v2/cli/)を参照するとよいでしょう。

### まとめ

本章では以下のことを学びました。

  * Vagrantを使って開発環境を用意する方法
  * Vagrantの基本的な操作

Vagrantを使うと、仮想ホストを`vagrant up`で起動し、あれこれいじった後に気に入らなくなってきたら`vagrant destroy`でまっさらなんてことも、簡単にできてしまいます。これからPuppetでこの仮想ホストをいじり倒していくには、もってこいの機能です。

manifestを書いていく途上で、何度も仮想ホストを作り直していくことになるでしょう。言葉を変えていえば、何度作り直してもmanifestさえあればすぐに元通りになるという状態を作っていくことが「あるべき状態を記述する」ということになるのです。
