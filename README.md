# marinOS

## 開発環境の構築

- 開発環境には [Ubuntu 22.04.1 LTS](https://releases.ubuntu.com/22.04/) on VAIO + [Docker](https://www.docker.com/) (+ macOS) を使用する。
  - なお、Windows10 と Ubuntu 18.04 でデュアルブートしていた VAIO から Windows を消し Ubuntu 18.04 からアップグレードをすると、画面がちらつくようになってしまい、その不具合を修正するのが面倒臭くなったので、同一ネットワークにいる macOS から ssh して基本的に開発を行う。

- 開発用 Docker Image の作成

```bash
make build
```

- 開発用コンテナを起動

```bash
make run
```

## 開発環境の構成

```bash
「Ubuntu 22.04.1 LTS」 <---(シリアル通信)---> 「H8/3069F ネット対応マイコン LAN ボード (完成品)」
```

![arch.jpeg](arch.jpeg)

### 部品

- [H8/3069F ネット対応マイコン LAN ボード (完成品](https://akizukidenshi.com/catalog/g/gK-01271/)
- [超小型スイッチング AC アダプター5V1A AD-D50P100](https://akizukidenshi.com/catalog/g/gM-06096/)

## シリアルポートに関する設定

```bash
echo 'SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", MODE="0666"' | sudo tee /etc/udev/rules.d/50-usb-serial.rules
```

## 参考

- [12ステップで作る 組込みOS自作入門](https://kozos.jp/books/makeos)

- [otmb/h83069f](https://registry.hub.docker.com/r/otmb/h83069f)
  - ベースの Docker Image
  - 色々試した結果、このやり方で開発環境を作るのが一番楽やった。
- [h83069f](https://github.com/otmb/h83069f)
  - Docker Image を作成するのためのソースコード
