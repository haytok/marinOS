# 概要

- このディレクトリにはこのプロジェクトを通して試行錯誤したことを書き記していく。

## 開発環境の構築に関して

- 開発環境の構築が一番難しかった。基本的には、「[12ステップで作る 組込みOS自作入門 - ■ binutils, gcc について](https://kozos.jp/books/makeos/#gnu_tools)」を参考に環境構築をすることを試みていた。
- 開発者が開発ツールをまとめた VM イメージ ([makeos-CentOS-20150504.ova](https://kozos.jp/books/makeos/#vmimage)) を使用することを検討していたが、QEMU, VirtualBox を用いた開発の体験がよくなかったので他のツールの模索をおこなっていた。基本的に手元のマシン (Ubuntu 22.04) に ssh して開発を行いたかったので、GUI を使いたくなかった。QEMU や
 VM の経験が浅く、上手く使いこなせなかったので今回は使用することを見送った。X11 転送や VM に ssh するのも試みたが、上手くいかなかった。以下に環境構築の際に参考にした記事を記す。
  - [12 ステップで作る組込み OS 自作入門 - Work Log](https://github.com/zulinx86/12steps-embedded-os)
  - [Linux カーネルを QEMU 上で起動する](http://kuniyu.jp/ja/blog/2/)
- Cygwin や WSL2 や Ubuntu 22.04 を使った開発環境の構築も検討したが、どうしてもツール (binutils-2.19.1 と gcc-3.4.6) のビルドが上手くいかなかったので、最終的にこの 2 つのツールが入った Docker Image を見つけたので、その Image をもとに開発を行うことにした。(この過程を経て OSS に特定のパッチを適用する方法を学ぶことができて良かった :))
- 以下のリポジトリも参考にしたが、binutils や gcc のバージョンが新しすぎで、後々のこのバージョンのせいで不具合等があると嫌だったので、今回は採用は見送った。
  - [12ステップで作る 組込みOS自作入門を進めていくための開発環境構築用Dockerfile](https://github.com/kjmatu/12step_self_embedded_os_dev_enviroment)
  - [古い gcc が欲しくて CentOS 5 の docker image を作った話](https://srz-zumix.blogspot.com/2021/02/gcc-centos-5-docker-image.html)
  - [gcc3](https://github.com/srz-zumix/gcc3)

## ssh で手元の Mac から Ubuntu 22.04.1 LTS on VAIO に接続する際のパスワードを省略する方法

- 以下のコマンドを参考にした。

```bash
ssh-copy-id -i ~/.ssh/id_rsa.pub ログインユーザ名@ホスト名
```

### 参考

- [(Linux) sshログインをパスワードなしにする方法](https://hara-chan.com/it/infrastructure/ssh-login-without-password/)

## 開発環境を構築するための docker コマンドにおける謎な箇所

- コンテナからホストの `/dev/ttyUSB0` を操作するには `--device=/dev/ttyUSB0:/dev/ttyUSB0` のように指定するのかと考えていたが、`-v /dev/ttyUSB0:/dev/ttyUSB0` でも上手くいった。マウントで上手くいくのはわかるが ...

### 参考

- [【ラズパイでもDocker！】Dockerコンテナ越しにシリアル通信を叩くNode.jsアプリの作成方法  - コンテナに外部デバイスへのアクセス権限を追加する](https://geek.tacoskingdom.com/blog/63#part-h2-1)
