#!/bin/bash

set -e

FILE=/tmp/lptproxy

# install deps if not available
bash -c "$(curl -fsSL https://raw.githubusercontent.com/nesto-software/LPTProxy/master/scripts/install-dependencies.sh)"

echo "Downloading lptproxy binary from latest GitHub release..."
curl -s https://api.github.com/repos/nesto-software/LPTProxy/releases/latest \
| grep "browser_download_url.*lptproxy" \
| cut -d : -f 2,3 \
| tr -d \" \
| wget -qi - -O "$FILE"

echo "Installing binary..."
sudo install -m 755 "${FILE}" "/usr/bin/lptproxy"