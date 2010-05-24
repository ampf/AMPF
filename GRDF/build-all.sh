#!/bin/sh

if [ ! -e Makefile ]; then
  ./autogen.sh
fi
make
