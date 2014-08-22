
= はじめに


クラウドが一般的になってきた昨今、サーバ構成管理の自動化は、もはやそれなしでは考えられないほど当たり前のものになっています。@<href>{https://puppetlabs.com/,Puppet}は、そのためのフレームワークのひとつです。


Puppetは2005年のリリース以来、後発の@<href>{http://www.opscode.com/chef/,Chef}とともに、サーバ構成管理の自動化に欠かせないフレームワークとして広く利用されてきました。とはいえ、ドキュメントが非常に充実してはいるもののその機能は膨大で、初心者にとって決してとっつきやすいものでないことは確かでしょう。現に、筆者の周りでも「Puppetを学習してみたいけど、どこから手をつけたらいいのか……」という声をよくききます。


クラウドの一般化によって、物理的な制約から離れ、サーバをあたかもプログラム上のオブジェクトであるかのように扱えるようになった現在、エンジニアにとって、Puppetのような自動化ツールを使いこなせるようになることは、技術スキルの向上に大きく寄与するでしょう。この本は、既にPuppetなどの自動化ツールを使いこなしているオペレーションエンジニアよりもむしろ、技術向上への意欲を燃やすアプリケーション開発者への入門となることを目指しています。


本書の目標は、この本を読んだ読者がPuppetの基本についてひととおり知り、オペレーションエンジニアの書いたmanifest(サーバのあるべき状態を記述した設定ファイルのようなもの。後述)に変更を加えたり、ある程度の規模のものなら自力でいちから書けるようになったりすることです。そのため、本書はあえてリファレンスとしての網羅性を目指しません。実際の学習段階で必要となる知識にしぼって説明します。


是非、本書を読みながら自分でも手を動かしてみて、一歩先行くエンジニアになってみませんか。

== システム環境


本書執筆時の、筆者のシステム環境は以下の通りです。

 * 作業環境: Mac OSX 10.9.4 + ruby 2.0.0
 * 本番環境: Amazon Linux AMI 2013.03 + ruby 1.8.7
 * 開発環境: Vagrant 1.6.3 + CentOS 6.5 + ruby 1.8.7
 * Puppet: 3.1.1



現状、最も広く使われているPuppetのバージョンは2.7系だと思われますが、本書における説明の範囲内では、3.1系でもたいした違いはありませんので、ご安心ください。

== フィードバック


本書に関するフィードバックは以下におよせください。

 * GitHub: @<href>{http://github.com/kentaro/puppet-book-support/issues,http://github.com/kentaro/puppet-book-support/issues}
 * メール: kentarok+puppet-book@gmail.com



なお、Puppetその他にまつわるテクニカルサポートは行っておりません。その類のお問い合せに関しましては一切おこたえしかねますので、あらかじめご了承ください。

== 本書の書誌情報

=== パブー版

 * puboo-v1.0.0: 2013年4月30日
 * puboo-v1.0.1: 2013年4月30日
 * puboo-v1.0.2: 2013年4月30日
 * puboo-v1.0.3: 2013年5月2日
 * puboo-v1.0.4: 2013年5月2日
 * puboo-v1.0.5: 2013年5月2日
 * puboo-v1.0.6: 2013年5月2日
 * puboo-v1.0.7: 2013年5月6日
 * puboo-v1.0.8: 2013年5月6日


=== Kindle版

 * kindle-v1.0.1: 2013年5月2日


=== 達人出版会版

 * tatsu-zine-v1.0.0: 2013年5月9日


== 筆者について


栗林健太郎。ネット上では「antipop/kentaro/あんちぽ」として知られる。市役所職員、株式会社はてな勤務を経て、現在は株式会社GMOペパボで技術責任者として勤務。Perl MongerでRubyist、時々ぺちぱー。@<href>{http://kentarok.org/,http://kentarok.org/}
