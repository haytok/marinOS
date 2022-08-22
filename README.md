# marinOS

## 開発環境の構築

- 開発環境には [Ubuntu 22.04.1 LTS](https://releases.ubuntu.com/22.04/) + [Docker](https://www.docker.com/) + macOS を使用する

```
docker run --name h83069f -it --privileged -v /dev/ttyUSB0:/dev/ttyUSB0 -v $HOME/h83069f:/root otmb/h83069f bash
```

## 開発環境の構成

```bash
「Ubuntu 22.04.1 LTS」 <---(シリアル通信)---> 「H8/3069F ネット対応マイコン LAN ボード (完成品)」
```

![arch.jpeg](arch.jpeg)

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
