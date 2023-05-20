#!/bin/bash
rm -rf .git
mv $1.ioc $2.ioc
rm *.launch

sed -i.bak -e "s/$1/$2/g" .mxproject
sed -i.bak -e "s/$1/$2/g" .cproject
sed -i.bak -e "s/$1/$2/g" .project
