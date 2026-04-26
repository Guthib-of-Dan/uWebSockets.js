#!/bin/sh
# start like "./update-version.sh 21.0.0"
if [[ "$1" =~ ^[0-9\.]{5,}$ ]]; then
    echo "Started updating README.md dist/installer.js dist/uws.js dist/package.json docs"
else
    echo "Start like ./update-version.sh 21.0.0"
    echo "Invalid version format. Length >= 5 characters, 'MAJOR.MINOR.PATCH', where MAJOR, MINOR, PATCH - numbers"
    exit
fi

sed -i "s/\\(npm install.*#v\\)\\([0-9\.]*\\)/\\1$1/" README.md 

sed -i "s/\\(var tag = \"v\\)[0-9\.]*/\\1$1/" dist/installer.js

sed -i "s/\\(uWebSockets.js (v\\)[0-9\.]*/\\1$1/" dist/uws.js

sed -i "s/\\(\"version\": \"\)[0-9\.]*/\1$1/" dist/package.json

sed -i "s/\\(uWebSockets.js v\\)[0-9\.]*/\1$1/" docs/tsconfig.json

echo "Regenerating documentation for GitHub Pages in docs/"
cd docs && npx typedoc && cd ..

echo "Running 'git add'"
git add README.md dist/installer.js dist/uws.js dist/package.json docs
git status
