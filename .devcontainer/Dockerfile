# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.177.0/containers/cpp/.devcontainer/base.Dockerfile

# [Choice] Debian / Ubuntu version: debian-10, debian-9, ubuntu-20.04, ubuntu-18.04
ARG VARIANT="buster"
ARG TOOLCHAIN_RELEASE=latest

FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}
LABEL org.opencontainers.image.source https://github.com/nesto-software/LPTProxy

ADD https://raw.githubusercontent.com/nesto-software/cross-toolchain-armhf/master/scripts/install-from-release.sh \
    /tmp/install-cross-toolchain-armhf.sh

RUN chmod +x /tmp/install-cross-toolchain-armhf.sh \
    && /tmp/install-cross-toolchain-armhf.sh $TOOLCHAIN_RELEASE

ENV STAGING_DIR /usr/raspberry-build/staging
ENV TOOLCHAIN=arm-unknown-linux-gnueabi
ENV TOOLCHAIN_PATH=/opt/crosstool-ng/x-tools/${TOOLCHAIN}/bin
ENV PATH=${TOOLCHAIN_PATH}:${PATH}

RUN mkdir -p "${STAGING_DIR}/lib"

RUN sudo apt-get update -y && apt-get install -y libtool autoconf automake g++

# wiringpi:
RUN git clone https://github.com/WiringPi/WiringPi.git /tmp/wiringpi \
    && cd /tmp/wiringpi \
    && git checkout f66c883d7c75280971a01619cd503d1809754801 \
    && cd wiringPi \
    && CC=${TOOLCHAIN}-gcc make \
    && DESTDIR=${STAGING_DIR} make install