#!/bin/sh

for i in components/*; do
  if [ -d $i ]; then
    pushd $i
    make svnclean
    popd
  fi
done

for i in prototypes/*; do
  if [ -d $i ]; then
    pushd $i
    make svnclean
    popd
  fi
done

for i in data-types/*; do
  if [ -d $i ]; then
    pushd $i
    make svnclean
    popd
  fi
done

for i in tests/*; do
  if [ -d $i ]; then
    pushd $i
    make svnclean
    popd
  fi
done

find components -name .autogen.done -exec rm {} \;

pushd mpf-core
make svnclean
popd

pushd tools/mpf-template
make svnclean
popd

find . -name .autogen.done -exec rm {} \;
