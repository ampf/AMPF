#!/bin/bash
# This build script is designed to be simple.
# For more functionality, we should use ant or another build system.

# FIX This won't work unless executed in the local directory
TOP=`pwd`

. setup.sh

function usage()
{
cat << EOF
usage: $0 [--clean | --no-install | --help]
  --clean: $0 will "make clean" and rerun "autogen.sh" before building.
  --no-install: Do not attempt to install libraries on the system.
  --help: Print this usage message.
EOF
}

# Parameters
unset CLEAN
unset NO_INSTALL

# State variables
unset INSTALL
unset HAVE_OPENCV
unset HAVE_GRDF
unset BUILT

# Parameter parsing
while [ $# -gt 0 ]
do
  if [ "x$1" = "x--clean" ]
  then
    CLEAN=true
  else
    if [ "x$1" = "x--no-install" ]
    then
      NO_INSTALL=true
    else
      usage
      exit 1
    fi
  fi
  shift
done

# Check for pkg-config
if [ ! `which pkg-config` ]
then
  echo "Error: Pkg-config not found. Install pkg-config or set your PATH."
  exit 1
fi

# Check and set PKG_CONFIG_PATH
unset PCP
PCPFOUND=`env |grep PKG_CONFIG_PATH |wc -l`
if [ $PCPFOUND -eq 0 ]
then
  echo "PKG_CONFIG_PATH not found."
  echo "Searching for pkgconfig directories..."
  PCPDIRS=`find /usr/lib -type d -name pkgconfig -print`
  PCPDIRS="$PCPDIRS `find /usr/local/lib -type d -name pkgconfig -print`"
  for pcpdir in $PCPDIRS
  do
    PCP="$PCP:$pcpdir"
  done
  read -n 1 -p "Do you want to set PKG_CONFIG_PATH to $PCP (recommended) (y/n)?" SETPCP
  if [ "x$SETPCP" = "xy" ]
  then
    export PKG_CONFIG_PATH=$PCP
  fi
  echo
  echo "PKG_CONFIG_PATH: " $PKG_CONFIG_PATH
fi

# Check for gstreamer-0.10
if ! `pkg-config --exists gstreamer-0.10`
then
  echo "Error: Gstreamer not found. Install gstreamer-0.10 or set PKG_CONFIG_PATH."
  exit 1
fi

# Check and set GST_PLUGIN_PATH
unset GPP
GPPFOUND=`env |grep GST_PLUGIN_PATH |wc -l`
if [ $GPPFOUND -eq 0 ]
then
  GPPDIRS="`find /usr/local/lib -type d -name gstreamer-0.10 -print`"
  for gppdir in $GPPDIRS
  do
    GPP="$GPP:$gppdir"
  done
fi

# Check for opencv
if ! `pkg-config --exists opencv`
then
  echo "Info: OpenCV not found. IplImage support will not be built. Install opencv or set PKG_CONFIG_PATH."
  unset HAVE_OPENCV
else
  HAVE_OPENCV=true
fi

# Check for grdf
if ! `pkg-config --exists grdf`
then
  echo "Info: GRDF not found. RDF support will not be built. Install grdf or set PKG_CONFIG_PATH."
  unset HAVE_GRDF
else
  HAVE_GRDF=true
fi

# Check for sudo permissions
if [ $NO_INSTALL ]
then
  unset INSTALL
else
  echo "Checking sudo permissions for library installation."
  if [ `sudo echo checking` ]
  then
    INSTALL=true
  else
    NO_INSTALL=true
    unset INSTALL
  fi
fi

echo "Building MPF"

# mpf-core
if [ -d mpf-core ]
then
  echo "Building mpf-core"
  cd mpf-core
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
      sudo make install || {
        echo Error: make install in `pwd` failed!
        exit 1
      }
    else
      export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:`pwd`/mpf-core-uninstalled.pc"
    fi
  fi
  cd $TOP
  echo "mpf-core build complete."
  BUILT="$BUILT mpf-core"
fi

. setup.sh

# data-types
# FIX specifically handling data-types dependencies here.
echo Building data-types
SECTIONTOP=$TOP/data-types
cd $SECTIONTOP
BUILT="$BUILT data-types ("

# data-types/mpf-iplimage
subdir=mpf-iplimage
if [ -d $subdir ]
then
  if [ $HAVE_OPENCV ]
  then
    echo Building $subdir
    cd $subdir
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
        sudo make install || {
          echo Error: make install in `pwd` failed!
          exit 1
        }
      else
        export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:`pwd`/mpf-iplimage-uninstalled.pc"
      fi
    fi
    cd $SECTIONTOP
    echo $subdir build complete.
    BUILT="$BUILT $subdir"
  fi
fi

# data-types/mpf-opencv
subdir=mpf-opencv
if [ -d $subdir ]
then
  if [ $HAVE_OPENCV ]
  then
    echo Building $subdir
    cd $subdir
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
        sudo make install || {
          echo Error: make install in `pwd` failed!
          exit 1
        }
      else
        export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:`pwd`/mpf-opencv-uninstalled.pc"
      fi
    fi
    cd $SECTIONTOP
    echo $subdir build complete.
    BUILT="$BUILT $subdir"
  fi
fi

# data-types/mpf-rdf
subdir=mpf-rdf
if [ -d $subdir ]
then
  if [ $HAVE_GRDF ]
  then
    echo Building $subdir
    cd $subdir
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
        sudo make install || {
          echo Error: make install in `pwd` failed!
          exit 1
        }
      else
        export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:`pwd`/mpf-rdf-uninstalled.pc"
      fi
    fi
    cd $SECTIONTOP
    echo $subdir build complete.
    BUILT="$BUILT $subdir"
  fi
fi
cd $TOP
echo data-types build complete.
BUILT="$BUILT )"

# tools
section=tools
if [ -d $section ]
then
  echo Building $section
  SECTIONTOP=$TOP/$section
  cd $SECTIONTOP
  BUILT="$BUILT $section ("
  SUBDIRS=`ls -1`
  for subdir in $SUBDIRS
  do
    if [ -d $subdir ]
    then
      echo Building $subdir
      cd $subdir
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
          ./autogen.sh && touch .autogen.done
        fi
      fi
      if [ -e Makefile ]
      then
        unset TOOL_BUILT
        make && TOOL_BUILT=true
        if [ $INSTALL ]
        then
          sudo make install
        fi
      fi
      cd $SECTIONTOP
      echo $subdir build complete.
      if [ $TOOL_BUILT ]
      then
        BUILT="$BUILT $subdir"
      fi
    fi
  done
  cd $TOP
  echo $section build complete.
  BUILT="$BUILT )"
fi

# components
section=components
if [ -d $section ]
then
  echo Building $section
  SECTIONTOP=$TOP/$section
  cd $SECTIONTOP
  BUILT="$BUILT $section ("
  SUBDIRS=`ls -1`
  for subdir in $SUBDIRS
  do
    if [ -d $subdir ]
    then
      echo Building $subdir
      cd $subdir
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
          ./autogen.sh && touch .autogen.done
        fi
      fi
      if [ -e Makefile ]
      then
        unset COMP_BUILT
        make && COMP_BUILT=true
        if [ $INSTALL ]
        then
          sudo make install
        else
          GPP="$GPP:`pwd`/.libs"
        fi
      fi
      cd $SECTIONTOP
      echo $subdir build complete.
      if [ $COMP_BUILT ]
      then
        BUILT="$BUILT $subdir"
      fi
    fi
  done
  cd $TOP
  echo $section build complete.
  BUILT="$BUILT )"
fi

# prototypes
section=prototypes
if [ -d $section ]
then
  echo Building $section
  SECTIONTOP=$TOP/$section
  cd $SECTIONTOP
  BUILT="$BUILT $section ("
  SUBDIRS=`ls -1`
  for subdir in $SUBDIRS
  do
    if [ -d $subdir ]
    then
      echo Building $subdir
      cd $subdir
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
          ./autogen.sh && touch .autogen.done
        fi
      fi
      if [ -e Makefile ]
      then
        unset COMP_BUILT
        make && COMP_BUILT=true
        if [ $INSTALL ]
        then
          sudo make install
        else
          GPP="$GPP:`pwd`/.libs"
        fi
      fi
      cd $SECTIONTOP
      echo $subdir build complete.
      if [ $COMP_BUILT ]
      then
        BUILT="$BUILT $subdir"
      fi
    fi
  done
  cd $TOP
  echo $section build complete.
  BUILT="$BUILT )"
fi

# tests
section=tests
if [ -d $section ]
then
  echo Building $section
  SECTIONTOP=$TOP/$section
  cd $SECTIONTOP
  BUILT="$BUILT $section ("
  SUBDIRS=`ls -1`
  for subdir in $SUBDIRS
  do
    if [ -d $subdir ]
    then
      echo Building $subdir
      cd $subdir
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
          ./autogen.sh && touch .autogen.done
        fi
      fi
      if [ -e Makefile ]
      then
        unset COMP_BUILT
        make && COMP_BUILT=true
        if [ $INSTALL ]
        then
          sudo make install
        else
          GPP="$GPP:`pwd`/.libs"
        fi
      fi
      cd $SECTIONTOP
      echo $subdir build complete.
      if [ $COMP_BUILT ]
      then
        BUILT="$BUILT $subdir"
      fi
    fi
  done
  cd $TOP
  echo $section build complete.
  BUILT="$BUILT )"
fi

# Add docs when they are buildable

echo "MPF build complete."
echo

if [ $CLEAN ]
then
  echo "Built from clean."
fi
if [ $INSTALL ]
then
  echo "Installed libraries on system."
fi
if [ $PKG_CONFIG_PATH ]
then
  echo "PKG_CONFIG_PATH: $PKG_CONFIG_PATH"
else
  echo "PKG_CONFIG_PATH: not set"
fi
if [ $HAVE_OPENCV ]
then
  echo "Have OpenCV."
else
  echo "OpenCV not found."
  echo "To enable IplImage data-type support, install the 'opencv' package."
fi
if [ $HAVE_GRDF ]
then
  echo "Have GRDF."
else
  echo "GRDF not found."
  echo "To enable RDF data-type support, install the 'grdf' package."
fi
echo "Built: $BUILT"
echo
if [ $GPP ]
then
  echo "It is recommended that you set your GST_PLUGIN_PATH:"
  echo "export GST_PLUGIN_PATH=$GST_PLUGIN_PATH:$GPP"
fi

