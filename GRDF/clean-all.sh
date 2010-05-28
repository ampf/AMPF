#!/bin/sh

# Pretty simple for now.
make svnclean
find . -name .autogen.done -exec rm {} \;
