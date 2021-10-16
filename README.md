[![Docker CI](https://github.com/DaOnlyOwner/DOOCE/actions/workflows/CI.yml/badge.svg)](https://github.com/DaOnlyOwner/DOOCE/actions/workflows/CI.yml)

This project provides very efficient chess draw-gen functionality.

## Dockerized Build

Install Docker to your dev machine.

```sh
sudo apt-get update && sudo apt-get install -y docker.io
sudo usermod -aG docker $USER && reboot
```

Download the source code.

```sh
git clone https://github.com/DaOnlyOwner/DOOCE
cd DOOCE
```

Build and test the source code.

```sh
docker build . -t "dooce-test"
```

## Manual Build
```sh
mkdir build
cd build
cmake . ..
cmake --build .
```