#!/bin/bash
VERSION=$(awk -F "=" '/PTEID_PRODUCT_VERSION=/{print $2}' ../pteid-mw-pt/_src/eidmw/release_data)
NOW=$(date +%x)

#filter out header, table of contents and css class .center
echo "Removing header 'page', table of contents and css 'center' image class."
perl -p0e "s/.*Content_begin -->//s" user_manual.md > user_manual_no_header.md
perl -p0e "s/End_of_content.*//s" user_manual_no_header.md > user_manual_no_header_no_footer.md
sed 's/{:.center}//g' user_manual_no_header_no_footer.md > user_manual_filtered.md

#generate pdf from ast
echo "Generating pdf manual."
pandoc -f gfm user_manual_filtered.md -o user_manual.pdf --template=template.tex --pdf-engine=pdflatex --toc --number-sections --variable version=$VERSION --variable date=$NOW

if [ $? -eq 0 ]
then echo "Done. PDF file user_manual.pdf created."
else
    echo "An error has ocurred. PDF file was not created."
fi

#clean-up
rm user_manual_no_header.md user_manual_no_header_no_footer.md user_manual_filtered.md