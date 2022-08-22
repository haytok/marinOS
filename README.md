# marinOS

## 開発環境の構築

- 開発環境には [Ubuntu 22.04.1 LTS](https://releases.ubuntu.com/22.04/) + [Docker](https://www.docker.com/) + macOS を使用する

```
docker run --name h83069f -it --privileged -v /dev/ttyUSB0:/dev/ttyUSB0 -v $HOME/h83069f:/root otmb/h83069f bash
```

## シリアルポートに関する設定

```bash
echo 'SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", MODE="0666"' | sudo tee /etc/udev/rules.d/50-usb-serial.rules
```

## 参考

- [12ステップで作る 組込みOS自作入門](https://kozos.jp/books/makeos/#gnu_tools)
