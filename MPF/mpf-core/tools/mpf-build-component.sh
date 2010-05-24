#!/bin/sh

usage()
{
cat << EOF
usage: $0 [-I <includepath>] [-L <library>] [-p <pkg>] <source.c> <object.o> <object.lo>
EOF
}

includes=""
libraries=""
packages="mpf-core"
sources=""
objects=""
libname=""

while test $# != 0; do
  case $1 in
    -c)
      clean=1
      ;;
    -I)
      includes="$includes $2"
      shift
      ;;
    -I*)
      arg=`expr "X$1" : 'X-I\(.*\)'`
      includes="$includes $arg"
      ;;
    -L)
      libraries="$libraries $2"
      shift
      ;;
    -L*)
      arg=`expr "X$1" : 'X-L\(.*\)'`
      libraries="$libraries $arg"
      ;;
    -p)
      packages="$packages $2"
      shift
      ;;
    -p*)
      arg=`expr "X$1" : 'X-p\(.*\)'`
      packages="$packages $arg"
      ;;
    -n)
      arg=`expr "X$1" : 'X-n\(.*\)'`
      libname=$arg
      ;;
    -n*)
      libname=$2
      shift
      ;;
    *.c)
      sources="$sources $1"
      ;;
    *.o)
      objects="$objects $1"
      ;;
    *.lo)
      objects="$objects $1"
      ;;
  esac
  shift
done


echo "Includes: $includes"
echo "Libraries: $libraries"
echo "Packages: $packages"
echo "Sources: $sources"
echo "Objects: $objects"


# Pick out the first source file on the initial theory it's the master
mainsource=`echo $sources | awk '{print $1}'`

# If it doesn't define a plugin, search through the rest
if ! `grep -q mpfcomponenttemplate.h $mainsource`;then
  mainsource=""
  for source in $sources;do
    if `grep -q mpfcomponenttemplate.h $source`;then
      if [ "x$mainsource" != "x" ];then
        echo "ERROR: more than one source file with GST_PLUGIN_DEFINE()"
        exit 1
      fi
      mainsource=$source
    fi
  done
fi

echo "Main source file: $mainsource"

# If there's still no main source file, error out
#if [ "x$mainsource" != "x" ];then
#  echo "ERROR: must have a source file with GST_PLUGIN_DEFINE()"
#  exit 1
#fi

# If the libname isn't specified, derive it from the main source's name
#if [ "x$libname" == "x" ];then
#  libname=`expr "X$mainsource" : 'X\(.*\)\.c'`
#fi

componentname=`grep COMPONENT_NAME $mainsource | awk '{print $3}'`
libname=$componentname


relink=0


buildobjs=""

# Check each source file and see if we need to rebuild it
for source in $sources;do
  base=`expr "X$source" : 'X\(.*\)\.c'`

  buildobjs="$buildobjs $base.lo"

  if [ ! -e $base.lo -o $base.c -nt $base.lo ];then
    libtool --mode=compile gcc `pkg-config --cflags $packages` \
	-c -o $base.lo $base.c -DPACKAGE=$componentname -DVERSION=\"0.0.1\"

    # new compile means relink
    relink=1
  fi
done

# Check each object file to see if they've been updated
for object in objects;do
  if [ $object -nt $libname.la ];then
    relink=1
  fi
done

libs=""
for library in libraries;do
  libs="$libs -L$library"
done

# If we have to relink, do it now
if [ $relink -eq 1 ];then
  libtool --mode=link gcc -g -O2 -o lib$base.la -rpath \
	/usr/lib/gstreamer-0.10 -module -avoid-version \
	-export-symbols-regex _*\(gst_\|Gst\|GST_\).* \
	`pkg-config --libs $packages` $libs \
	$buildobjs $objects
fi
