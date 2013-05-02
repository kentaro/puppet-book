
= 再び、なぜPuppetが必要なのか？


"Hello, World!"から始まり、より実践的なクラスタ構成まで、ひと通りmanifestを書いてシステム記述・構築を行ってきました。本章では、それらの実践を経たいま、「なぜPuppetが必要なのか？」についてあらためて考えてみましょう。

== プログラマの三大美徳


プログラミング言語Perlの作者ラリー・ウォール氏は、その著書『@<href>{http://www.oreilly.co.jp/books/4873110963/,プログラミング言語Perl}』で「プログラマの三大美徳」として、以下の3つを挙げています。

 1. 無精(Laziness)
 1. 短気(Impatience)
 1. 傲慢(Hubris)



読んで字のごとくという表現ではなく、いずれも反語的な意味を持っています。


「無精」とは、何度も同じことをくりかえすようなことを面倒だと思うあまりに、便利なプログラムを書いたり、ドキュメントを書いたりするような性質。つまり「面倒なことを避けるためなら、いくら面倒なことでもやる」という、一見矛盾するような意味です。


「短気」とは、コンピュータが自分の思い通りにならないことをよしとしないがために、「傲慢」とは、他人に自らのプログラムを悪しざまにいわれるのを受け入れられないがために、よいプログラムを書かざるを得ない、そういう性質のことです。


その中でも「無精」が、プログラマにとっての第1の美徳として顕彰されています。このことについて、もう少し掘り下げていきましょう。

== システム構築は面倒


あらためて考えてみると、システム構築は、まさに「面倒」の固まりです。そう思いませんか？


やるべきことは山のようにあります。タスクを洗い出し、実行し、さらにはあとに続く人々(その中には未来の自分も含まれるでしょう)のために作業のリストを手順書に残したところで、今度はその手順書のメンテナンスといった、あらたな面倒が増えるばかりです。「第1章 なぜPuppetが必要なのか？」で述べた問題点は、あらかたそれらが「面倒だから」ということに尽きます。

 * 手順書を作成するのが面倒
 * 手順書を更新するのが面倒
 * スクリプト化するのが面倒
 * スクリプトが肥大化して保守するのが面倒
 * スクリプトの冪等性を確保するのが面倒



また、システムは一度構築すればそれで終わりではありません。一般に以下の2軸において、変化への圧力を受けます。

 * システム上に構築されるアプリケーション(Webアプリケーションなど)の要件の変化
 * OSやライブラリ、パッケージの更新



みなさんが構築するシステムは、普通はその上でなにかしらのアプリケーションを実行するためのものでしょう。であるからには、アプリケーションの要件の変化によって、システム状態へも変化の圧力がかかってきます。また、そもそもそうしたアプリケーション要件の変化は、社会情勢の変化と密接に結びついたものです。アプリケーションが世の中に価値を提供し続けるためには、情勢の変化に適応し続ける必要があるためです。


また、Linuxを始めとするオープンソースのプロダクトを、その利用やサポートにかかる料金という意味おいて「無償」で使っている場合には、それらのプロダクトの更新に常に対応していく必要もでてきいます。いつまでも古い、パッチもリリースされないようなバージョンのプロダクトを使い続けるわけにもいきません(もちろん自力で対応できる、あるいは、サポートにお金を払えるならそれでもよいかもしれませんが)。


つまりシステム構築とは、ある一時点での営みではなく、そこからどれだけ将来にわたるか不明な時間軸における「面倒」の固まりなのです。

== システム構築に「無精」を導入する


"Infrastructure as code"というスローガンを目にしたことがある方も多いでしょう。伊藤直也氏による『@<href>{http://www.amazon.co.jp/dp/B00BSPH158,入門Chef Solo}』のサブタイトルにも採用されている、有名なフレーズです。この言葉の含意には様々なものがあると思いますが、本章の文脈に引き寄せ「システム構築にプログラマの美徳を導入しよう」という意味であると解釈したとしても、大きく外してはいないように思います。


上述した通り、多大な「面倒の固まり」であるシステム構築を「コード」の力によって解決していこうという試み。その一助となるだろうのがPuppetやChefといったフレームワークなのです。


筆者の勤務先では、アプリケーション開発者もPuppetのmanifestを書きます。とはいえ、いちからシステム状態のすべてを記述するというわけではありません。基本的には、最初のシステム構築時にオペレーションエンジニアが基盤を作った上で、アプリケーション要件の変化に対応していく部分については、アプリケーション開発者も積極的にmanifestを書いていくというスタイルを採っています。


多人数によるシステム構築においては、構築時のいちいちが手順書のような実行不可能なものに頼って行われていては、不便でしかたありません。それが正しいのか正しくないのか、実行できないことには「面倒」は解決しないからです。スクリプトによる手順化にしても、変化していく環境の中で、状態を壊さないよう保ちつつ、大規模なシステムを記述していくのは至難の業でしょう。


Puppetのようなシステム記述に特化したフレームワークは、それらの問題を、冪等性の確保などの「面倒」をフレームワーク内で隠蔽しつつ「システム状態をコード化する」というアプローチにより、解決します。一見すると、あらたな記法を憶えなければならないのは、またひとつ「面倒」が増えるだけのようにも思えますが、「無精」の定義を思い出してみると明らかなように、それは「面倒なことを避けるためなら、いくら面倒なことでもやる」という性質の発露に他ならないのです。

== 「無精」を発揮する


上述の通り、筆者の勤務先ではアプリケーション開発者もmanifestを書くわけですが、その際、GitHub/GitHub Enterpriseのpull request機能を活用し、職種に関わらず相互レビューを行いながら、manifestを育てています。これは、アプリケーション開発者が、アプリケーションコードについて日常的にやっていることとまったく同じです。


Webアプリケーションを開発し、サービスとして提供するには、それを動かすためのサーバが必要です。また、ふだんの機能開発においても、新機能のために新しいパッケージやミドルウェアを使いたいのでインストールするといったことはよくあることでしょう。そのような場合に、いちいちオペレーションエンジニア(多くの場合、山積みのタスクを前に忙しくしている)にお願いして導入してもらうのは「面倒」ではありませんか？


それが「面倒」であるのは、システム構築が、開発/オペレーションを問わず、エンジニアの共通言語である「コード」になっていない場合です。Puppetによりmanifest化されていれば、ただそれを編集してpull requestを投げれば済む話です。筆者は、主にアプリケーション開発者として仕事をしてきたので、システム構築に関しては(開発に関することと比較すると)それほど詳しくはありません。そのため、pull requestを通してレビューをしてもらいながら、必要な機能開発のためにシステム状態を変更していっています。


そのようにして「面倒」を「コード」で解消していくうちに、あらたな面倒にぶちあたるようになりました(かように「面倒」の種は尽きないものです……)。

 1. manifestの適用が特定の環境(開発サーバ等)でしかできないのが面倒
 1. 修正箇所が他に悪影響をもたらしていないか検証するのが面倒



いずれも後述の通り、勤務先の同僚らといっしょになって、現在解決しつつある問題です。

== モジュール化とローカル開発


1については、agent/master構成の便利さを認めつつも、環境セットアップの「面倒」さを避けるため、本書でこれまで説明してきたような、@<tt>{puppet apply}のみを使ってmanifestをシステムに適用するという方法で、日々の開発を行っています。また、そのためにはmanifestの各部分が高度にモジュール化されていないとなりませんので、いわゆるPuppetにおけるmodule化を進めると同時に、これまでに説明してきたロールという概念を導入しました。


ふだん、Ruby on Railsなどのモダンな開発環境でアプリケーション開発をしている読者には共感していただけると思うのですが、筆者はどこか別の場所にあるサーバにログインして開発するということを、非常に「面倒」に感じます。できるだけ手元のマシンのみで済ませたい。そのため、本書ではVagrantを使って、すべてを手元のマシン1台のみで完結できるスタイルで説明してきました。


前章で示した通り、そのような手法を採ったとしても本番環境への適用も十分に行えますし、現に筆者が勤務先で開発しているサービスも、同様にして日々の開発を行っています。

== インテグレーション時の問題を解決する


実際の開発では、td-agentクラスタの箇所で述べたように、おのおののエンジニアがその時点で主に担当する各ロールに注力して、manifestを書いていくことになります。


そうすると、最終的にモジュールやロールが統合され、ひとつのシステムとして適用される際に、自分の変更が他に悪影響を与えてしまっていないかを確認・検証する必要がでてきます。これは、アプリケーション開発におけるインテグレーション時の問題と同様です。モジュール化を進めるには、統合時における問題解決が避けては通れません。それが2に述べた「面倒」の内実です。


この問題を解決するのが第19章で説明したserverspecです。


Webアプリケーションを開発する際に、インテグレーションテストをまったく行わずに、手動でブラウザをポチポチと操作して、膨大なテスト項目を非常な労力を使って検証していくなんてことはもはやあるまいとは思いますが、これまでのシステム構築は、まさにアプリケーションにおける統合テストの不在そのものの状態だったわけです。


各モジュールやロールがおりなすシステムは、アプリケーションがそうであるのと同様に、複雑なものです。システム記述がコード化されたいま、インテグレーション時の検証についてもコード化することは、いまとなっては自然な発想であるように思えます。これでまた、ひとつ「面倒」が解消されたのでした。

== 「無精」なエンジニアのためのPuppet


面倒を省みず、なにごとにも一生懸命に取り組むというような勤勉さが美徳であることは、いうまでもないことです。しかし我々は、職業としてエンジニアを選択し、社会的な価値を提供するべき存在であるからには、単に勤勉であるだけではなく、結果を残さなければなりません。そのためには、本章で述べた「無精」であることとという、プログラマ的な美徳が役に立つでしょう。


システム構築におけるあらゆる状況を、可能な限り、エンジニアにとっての共通言語「コード」によって可視化していくこと。システム構築、ひいては、サービスの提供には、技術的・組織的な面でいろいろな「面倒」がついてまわるものですが、それら「面倒」をものともしない強い「無精」によって、よりよい価値を提供できるエンジニアになるために、Puppetはおおいに役立つものと確信しています。
