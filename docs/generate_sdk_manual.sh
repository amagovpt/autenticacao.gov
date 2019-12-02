#!/bin/bash
VERSION=$(awk -F "=" '/PTEID_PRODUCT_VERSION=/{print $2}' ../pteid-mw-pt/_src/eidmw/release_data)
NOW=$(date +%x)

#filter out header, table of contents and css class .center
echo "Removing header 'page', table of contents and css 'center' image class."
perl -p0e "s/.*Content_begin -->//s" manual_sdk.md > manual_sdk_no_header.md

#generate pdf from ast
echo "Generating pdf manual."
pandoc -f gfm manual_sdk_no_header.md -o Manual_de_SDK_novo.pdf --template=template_sdk.tex --toc --number-sections --variable version=$VERSION --variable date=$NOW --pdf-engine=xelatex --listings --variable colorlinks=true


if [ $? -eq 0 ]
then echo "Done. PDF file manual_sdk.pdf created."
else
    echo "An error has ocurred. PDF file was not created."
fi

#clean-up
rm manual_sdk_no_header.md