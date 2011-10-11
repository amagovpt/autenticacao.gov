#!/bin/bash

chmod +x download-install-xerces.sh
./download-install-xerces.sh

export UNIVERSAL=true
chmod +x make-mac.sh
./make-mac.sh
