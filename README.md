LPT Proxy for Raspberry Pi (armhf)   
========

<p align="center">
  <img src=".github/imgs/project_logo.png">
</p>

[![https://github.com/nesto-software/ProxySuite](https://img.shields.io/badge/part%20of-ProxySuite-blue)](https://github.com/nesto-software/ProxySuite)

Approach
--------

The Raspberry Pi has no [IEEE 1284 connectors](https://en.wikipedia.org/wiki/IEEE_1284#IEEE_1284_connectors_and_cables). 
In order to proxy bi-directional parallel communications, one needs a type A (DB-25 25 pi) connector for the host connection and a type B (Centronics aka "Micro Ribbon") 36-pin connector for the printer or device connection. That is the case for standard IEEE 1284-I cables which are common for e.g. thermal printers.

We rely on the [retro-printer module](https://www.retroprinter.com/) to extend the pi with a Centronics connector over GPIO.
That way, the pi is presented as an LPT printer to the host side, e.g. a POS system.

We forward the data to the actual printer using a [SBT-UPPC USB to Printer Cable](https://www.sabrent.com/product/SBT-UPPC/usb-2-0-centronics-printer-cable-cn36m/). The intercepted data is simultaneously extracted via ZMQ and can be further processed by a client of your choice (see <a href="nodejs-client/">nodejs-client example</a>). 

Although our approach needs some special hardware to work, we call this a software approach because the data on the host <-> printer connection is forwarded using the pi's CPU. Generally, we strive for a pure hardware solution which omits the pi's hardware completely to forward the data, but we could not find any vendor which provides such a solution. Pure hardware-based solutions exist for other interfaces in the <a href="https://github.com/nesto-software/ProxySuite">ProxySuite</a> though.

Development Status
------
<table>

  <tr><th>Variant</th><th>Status</th></tr>
  <tr><td>Software</td><td align="center">:gear:</td></tr>
  <tr><td>Hardware</td><td align="center">:question:</td></tr>

</table>

Install via GitHub Releases Download (binary)
---------------------------------------------

| Method    | Command                                                                                           |
|:----------|:--------------------------------------------------------------------------------------------------|
| **curl**  | `bash -c "$(curl -fsSL https://raw.githubusercontent.com/nesto-software/LPTProxy/master/scripts/install-from-release.sh)"` |
| **wget**  | `bash -c "$(wget -O- https://raw.githubusercontent.com/nesto-software/LPTProxy/master/scripts/install-from-release.sh)"`   |

Contributors
------------ 

- Rich Mellor
- Martin Löper `<martin.loeper@nesto-software.de>`