#!/bin/bash

cd /usr/local/src/swig-python-todolist

apt-get update
apt install -y build-essential vim make swig electric-fence gdb git procps electric-fence
apt install -y python3-pip python3 python3-setuptools python3-dbg python3-dev
#apt install -y python-pip python2.7 python2.7-setuptools python2-dbg python2-dev
apt install -y cmake libcurl libcurl-dev libcurlpp-dev zlib1g-dev pkg-config libgtest-dev libcivetweb-dev civetweb libbenchmark-dev libgmock-dev


echo "" > ~/.viminfo
echo "syn on" > ~/.vimrc
echo "color desert" >> ~/.vimrc

echo "export LS_OPTIONS='--color=auto'" >> ~/.bashrc
echo "alias ls='ls \$LS_OPTIONS'" >> ~/.bashrc
echo "alias ll='ls \$LS_OPTIONS -l'" >> ~/.bashrc
echo "alias l='ls \$LS_OPTIONS -lA'" >> ~/.bashrc

exec bash
