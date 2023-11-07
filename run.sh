#!/bin/bash

mkdir -p build && cd build &&
    cmake .. -DCMAKE_BUILD_TYPE:STRING="" -G Ninja && ninja &&
    ./smoke_sensor "$1" "$2"
