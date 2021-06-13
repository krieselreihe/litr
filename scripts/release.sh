#!/usr/bin/env bash

PUBLISH_DIR=build/publish

echo "Start building release ..."

echo "Did you define the proper version inside src/core/Version.hpp?"
select yn in "Yes" "No"; do
    case $yn in
        Yes ) echo "What is the release version? Format is x.x.x"; read -r VERSION; break;;
        No ) echo "Please define the release version in src/core/Version.hpp"; exit;;
    esac
done

echo "Version is $VERSION"

echo "Build application"
./build/debug/src/client/Client build --target=release

echo "Setup release"
rm -fr $PUBLISH_DIR
mkdir -p $PUBLISH_DIR

cp build/release/src/client/Client $PUBLISH_DIR/litr
tar -czf $PUBLISH_DIR/litr-mac-"$VERSION".tar.gz $PUBLISH_DIR/litr
shasum -a 256 $PUBLISH_DIR/litr-mac-"$VERSION".tar.gz | cut -f1 -d' ' > $PUBLISH_DIR/litr-mac-"$VERSION"-sha256.txt
rm -f $PUBLISH_DIR/litr

echo "Release files are located under $PUBLISH_DIR"
echo "Done"
