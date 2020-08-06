#!/bin/sh

cp docs/generate_doc.sh ../doc/generate_doc.sh
cp docs/layout_main.xml ../doc/config/layout_main.xml

# Change to afr root
pushd ../ > /dev/null

./doc/generate_doc.sh .

echo "--------------------------------------------------"
echo "Note to maintainers: AFR docs have been generated and main page modified to include links to cypress docs. These links are relative paths from the docs found in doc/output so be careful to maintain the folder structure or the you may end up with missing pages."
echo "--------------------------------------------------"

popd > /dev/null
