export MPF_HOME=$PWD

# Source this script inside the MPF directory to setup for MPF development

export PKG_CONFIG_PATH_CONSTRUCTOR="$PWD/tools/make-uninstalled-pkgconfig-path.sh $PWD"
export GST_PLUGIN_PATH_CONSTRUCTOR="$PWD/tools/make-gst-plugin-path.sh $PWD"
export PKG_CONFIG_PATH_CONSTRUCTOR_GRDF="$PWD/tools/make-uninstalled-pkgconfig-path.sh $PWD/../grdf"

export GST_PLUGIN_PATH_CONSTRUCTOR_TESTS="$PWD/tools/make-gst-plugin-path.sh $PWD/tests"

export GST_PLUGIN_PATH=`$GST_PLUGIN_PATH_CONSTRUCTOR`:`$GST_PLUGIN_PATH_CONSTRUCTOR_TESTS`:$PWD/mpf-core/mpf/.libs
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:`$PKG_CONFIG_PATH_CONSTRUCTOR`:`$PKG_CONFIG_PATH_CONSTRUCTOR_GRDF`

# Put "mpf-new" scripts on the path.
export PATH=$PWD/tools/mpf-template/tools:$PATH

# Make sure we are using the development build of MPF.
#
export LD_LIBRARY_PATH=$GST_PLUGIN_PATH:$LD_LIBRARY_PATH

echo
echo PKG_CONFIG_PATH=$PKG_CONFIG_PATH
echo
echo GST_PLUGIN_PATH=$GST_PLUGIN_PATH
