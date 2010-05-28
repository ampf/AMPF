#!/bin/bash
# This build script is designed to be simple.
# For more functionality, we should use ant or another build system.

function usage()
{
cat << EOF
usage: $0 [--clean | --install | --help]
  --clean: $0 will "make clean" and rerun "autogen.sh" before building.
  --install: Will install the software into standard (e.g. /usr) system locations.
  --uninstall: Will remove the software from standard (e.g. /usr) system locations.
  --help: Print this usage message.
EOF
}

# Parameters
unset CLEAN

# State variables
unset INSTALL
unset BUILT

# Parameter parsing
while [ $# -gt 0 ]
do
  if [ "x$1" = "x--clean" ]
  then
    CLEAN=true
  else
    if [ "x$1" = "x--install" ]
    then
      INSTALL=install
    else
      if [ "x$1" = "x--uninstall" ]
      then
        INSTALL=uninstall
      else
        usage
        exit 1
      fi
    fi
  fi
  shift
done

if [ $CLEAN ]
then
  if [ -e Makefile ]
  then
    make clean
    fi
  rm -f .autogen.done
fi
if [ ! -e .autogen.done ]
then
  if [ -e autogen.sh ]
  then
    ./autogen.sh || {
      echo Error: `pwd`/autogen.sh failed!
      exit 1
    }
    touch .autogen.done
  fi
fi
if [ -e Makefile ]
then
  make || {
    echo Error: make in `pwd` failed!
    exit 1
  }
  if [ $INSTALL ]
  then
    sudo make $INSTALL || {
      echo Error: make $INSTALL in `pwd` failed!
      exit 1
    }
  fi
fi
