#!/bin/bash

set -e

FILE=/tmp/lptproxy

echo "Downloading .deb file from latest GitHub release..."
curl -s https://api.github.com/repos/nesto-software/LPTProxy/releases/latest \
| grep "browser_download_url.*lptproxy" \
| cut -d : -f 2,3 \
| tr -d \" \
| wget -qi - -O "$FILE"

echo "Installing binary..."
cp "${FILE}" "/usr/bin/lptproxy"