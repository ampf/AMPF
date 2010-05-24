#!/bin/sh
path=""
libs=0
plugins=0
for dir in `find $1 -type d -name .libs`;do
  for so in `find $dir -name \*.so`;do
    libs=0
    plugins=0
    if `strings $so | grep -q gst_plugin_desc`;then
      plugins=$(($plugins + 1))
    else
      libs=$(($libs + 1))
    fi
  done
#  echo $dir l:$libs p:$plugins
  if [ $libs -eq 0 -a $plugins -gt 0 ];then
#    echo adding...
    path="$path:$dir"
  fi
done

echo $path | sed 's/^://g'
