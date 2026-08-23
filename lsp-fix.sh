
#!bin/sh
set -e

./clean.sh

if ! [ -x "$(command -v bear)" ]; then
    echo "Serve il comando 'bear'. Installalo boy/girl/you!"
    exit 1
fi

bear -- ./headers.sh ./build.sh

echo "good boy"
