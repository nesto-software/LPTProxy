# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.177.0/containers/cpp/.devcontainer/base.Dockerfile

# [Choice] Debian / Ubuntu version: debian-10, debian-9, ubuntu-20.04, ubuntu-18.04
ARG VARIANT="buster"
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

ADD https://raw.githubusercontent.com/nesto-software/SerialProxy/master/scripts/install-cross-toolchain-armhf.sh \
    /tmp/install-cross-toolchain-armhf.sh

RUN chmod +x /tmp/install-cross-toolchain-armhf.sh \
    && /tmp/install-cross-toolchain-armhf.sh

ENV TOOLCHAIN arm-unknown-linux-gnueabi
ENV STAGING_DIR /usr/raspberry-build/staging
ENV PATH="/opt/crosstool-ng/x-tools/${TOOLCHAIN}/bin:${PATH}"

RUN mkdir -p "${STAGING_DIR}/lib"

RUN sudo apt-get update -y && apt-get install -y libtool autoconf automake g++

# wiringpi:
RUN git clone https://github.com/WiringPi/WiringPi.git /tmp/wiringpi \
    && cd /tmp/wiringpi \
    && git checkout f66c883d7c75280971a01619cd503d1809754801 \
    && cd wiringPi \
    && CC=${TOOLCHAIN}-gcc make \
    && DESTDIR=${STAGING_DIR} make install