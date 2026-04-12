#!/bin/bash
# запускать из корня проекта!
set -e


# build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 
cmake --build build --parallel 4
cmake --install build --prefix deploy

docker-compose down
docker-compose build

docker-compose up



