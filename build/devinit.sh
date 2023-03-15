#!/bin/bash

cd /usr/local/src/swig-python-todolist

apt-get update
apt install -y build-essential vim make swig electric-fence gdb git procps electric-fence
apt install -y python3-pip python3 python3-setuptools python3-dbg python3-dev
#apt install -y python-pip python2.7 python2.7-setuptools python2-dbg python2-dev
apt install -y cmake libcurl libcurl-dev libcurlpp-dev zlib1g-dev pkg-config libgtest-dev libcivetweb-dev civetweb libbenchmark-dev libgmock-dev

# Install prometheus-client-cpp
cd /usr/local/src
git clone --depth 1 --branch v1.1.0 https://github.com/jupp0r/prometheus-cpp.git
cd prometheus-cpp
git submodule init
git submodule update
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=ON -DENABLE_PUSH=OFF -DENABLE_COMPRESSION=ON
cmake --build . --parallel 4
ctest -V
cmake --install .

# install Opentelemetry
cd /usr/local/src
apt-get install -y libgtest-dev libgmock-dev libbenchmark-dev
git clone --branch v1.8.2 --recursive https://github.com/open-telemetry/opentelemetry-cpp
cd opentelemetry-cpp
mkdir build
cd build
cmake -DWITH_PROMETHEUS=ON -DWITH_OTLP=ON -DWITH_OTLP_HTTP=ON -DBUILD_SHARED_LIBS=ON -DWITH_LOGS_PREVIEW=ON ..
cmake --build . --target all -j3
cmake --install .

echo "" > ~/.viminfo
echo "syn on" > ~/.vimrc
echo "color desert" >> ~/.vimrc

echo "export LS_OPTIONS='--color=auto'" >> ~/.bashrc
echo "alias ls='ls \$LS_OPTIONS'" >> ~/.bashrc
echo "alias ll='ls \$LS_OPTIONS -l'" >> ~/.bashrc
echo "alias l='ls \$LS_OPTIONS -lA'" >> ~/.bashrc

exec bash
