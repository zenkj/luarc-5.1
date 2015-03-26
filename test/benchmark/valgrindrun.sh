#!/bin/sh

rm -f valgrind.result
valgrind --num-callers=50 luarc51g mandelbrot.lua-6.lua 2>&1 | tee valgrind.result
