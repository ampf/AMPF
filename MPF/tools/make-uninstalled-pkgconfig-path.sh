#!/bin/bash
echo `find $1 -name \*uninstalled.pc.in | sed 's@/[^/]*$@@'` | tr ' ' ':'
