#!/bin/sh
# start like "./update-version.sh 21.0.0"
if [[ "$1" =~ ^[0-9\.]{5,}$ ]]; then
    echo "Started updating README.md, src/installer.js/ src/uws.js, package.json"
else
    echo "Start like ./update-version.sh 21.0.0"
    echo "Invalid version format. Length >= 5 characters, 'MAJOR.MINOR.PATCH', where MAJOR, MINOR, PATCH - numbers"
    exit
fi

sed -i "s/\\(npm install.*#v\\)\\([0-9\.]*\\)/\\1$1/" README.md 

sed -i "s/\\(var tag = \"v\\)[0-9\.]*/\\1$1/" src/installer.js

sed -i "s/\\(const VERSION = \"v\\)[0-9\.]*/\\1$1/" src/uws.js

sed -i "s/\\(\"version\": \"\)[0-9\.]*/\1$1/" package.json

git add README.md src/installer.js src/uws.js
