FROM debian:bullseye

RUN apt-get update -y && \
	apt-get install -y \
		autoconf \
		automake \
		gcc \
		g++ \
		git \
		gzip \
		libtool \
		make \
		tar \
		unzip \
		wget \
		python-is-python3 && \
	apt-get -y clean && \
	rm -rf /var/lib/apt/lists/*

WORKDIR /install

# CMake
RUN wget https://github.com/Kitware/CMake/releases/download/v3.23.1/cmake-3.23.1-linux-x86_64.sh && \
	/bin/sh cmake-3.23.1-linux-x86_64.sh --prefix=/usr/local --exclude-subdir --skip-license && \
	rm -rf *

# fmt
RUN wget https://github.com/fmtlib/fmt/archive/refs/tags/8.1.1.tar.gz && \
    tar zxf 8.1.1.tar.gz && \
    cd fmt-8.1.1 && \
	mkdir build && \
	cd build && \
	cmake .. && \
	make && \
	make install && \
	make clean && \
	cd ../.. && \
	rm -rf *

# spdlog
RUN wget https://github.com/gabime/spdlog/archive/refs/tags/v1.9.2.tar.gz && \
	tar zxf v1.9.2.tar.gz && \
	cd spdlog-1.9.2 && \
	mkdir build && \
	cd build && \
	cmake .. && \
	make && \
	make install && \
	make clean && \
	cd ../.. && \
	rm -rf *


WORKDIR /workspace

CMD ["ls", "-lFah"]