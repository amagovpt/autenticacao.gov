#!/bin/bash

FLATPAK_REPO=$HOME/flatpak-repo

#Clean all artifacts from regular (manual) builds
git clean -fx
rm -rf ./eidguiV2/build

echo "Building flatpak package version: "
grep -A1 "<releases>" pt.gov.autenticacao.metainfo.xml | tail -n1

#Create local flatpak repo if it doesn't exist
if [ ! -d $FLATPAK_REPO ]
then
    mkdir $HOME/flatpak-repo
fi

#Update modification time to make sure the QML resource file timestamps are different to previous build
touch pt.gov.autenticacao.yml

#Build flatpak and install locally for the current user (--user flag as the same meaning as for flatpak-install)
flatpak-builder --repo=$FLATPAK_REPO --install --user --force-clean build-dir pt.gov.autenticacao.yml

if [ $? -eq 0 ]
then
    echo "Flatpak build installed and available for export at $FLATPAK_REPO"
fi
