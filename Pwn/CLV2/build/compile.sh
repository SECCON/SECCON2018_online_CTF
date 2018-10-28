#!/bin/sh

gcc -o main main.c -Ofast -Wl,-z,relro,-z,now -fpic -pie
strip -s ./main
