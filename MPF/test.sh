#!/bin/bash
# Simple test wrapper to establish environment then run a test in its directory.

# Must cleanup processes since they may not terminate when this (parent) does.
term ()
{
    # take out children.
    echo "reaping children of $$"
    pkill -9 -P $$
    exit 0
}

trap 'term' SIGTERM SIGINT

prefix=$3
if [ $prefix = "''" ]; then
    prefix=
fi
version=$4
if [ $version = "''" ]; then
    version=
fi
args=$5
if [ $args = "''" ]; then
    args=
fi
gstargs=$6
if [ $gstargs = "''" ]; then
    gstargs=
fi

echo prefix=$prefix
echo args=$args
echo version=$version
echo gstargs=$gstargs

. ./setup.sh

cd $1
prefix=$prefix version=$version args=$args gstargs=$gstargs ./$2
