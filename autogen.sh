#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="libmate-media-profiles"
REQUIRED_AUTOMAKE_VERSION=1.9

(test -f $srcdir/configure.ac \
  && test -d $srcdir/libmate-media-profiles) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level $PKG_NAME directory"
    exit 1
}

which mate-autogen.sh || {
    echo "You need to install mate-common from the MATE git"
    exit 1
}
USE_MATE2_MACROS=1 . mate-autogen.sh
