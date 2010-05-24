#!/bin/sh

orig_plugin_path=$GST_PLUGIN_PATH

out=/tmp/$$-out
err=/tmp/$$-err

trap "rm -f $out $err" INT TERM EXIT

error() {
  exit 1
}


##### test 0a: any faults in gst-inspect itself?
export GST_PLUGIN_PATH=''
gst-inspect > $out 2> $err

if [ $? -ne 0 ];then
  echo ERROR: base gst-inspect returns error code $?
  error
fi

if [ -s $err ];then
  echo ERROR: base gst-inspect returned stderr:
  echo -----
  cat $err
  echo -----
  error
fi


##### test0b: repeat with the test elements in the search path and make sure they show
export GST_PLUGIN_PATH=$orig_plugin_path:`pwd`/.libs
gst-inspect > $out 2> $err

if [ $? -ne 0 ];then
  echo ERROR: gst-inspect with test path returns error code $?
  error
fi

if [ -s $err ];then
  echo ERROR: gst-inspect with test path returned stderr:
  echo -----
  cat $err
  echo -----
  error
fi

if ! `grep -q voidstar-sink $out`;then
  echo ERROR: gst-inspect does not show voidstar-sink at all
  error
fi


##### test0c: check various properties of voidstar-sink
gst-inspect voidstar-sink > $out 2> $err

if ! `grep -q "SINK template: 'input'" $out`;then
  echo ERROR: gst-inspect of voidstar-sink doesn\'t show a sink pad \'input\'
  error
fi

if ! `grep -A 3 "SINK template:" $out | grep -q '^      ANY'`;then
  echo ERROR: voidstar-sink\'s input pad doesn\'t have ANY caps
  error
fi
