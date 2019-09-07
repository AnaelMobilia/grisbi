#!/bin/bash

# Warnings enabled
CFLAGS=""
CFLAGS="-Wall"
CFLAGS+=" -Wextra"

#CFLAGS+=" -Wbad-function-cast"
#CFLAGS+=" -Wcast-align"
CFLAGS+=" -Wchar-subscripts"
CFLAGS+=" -Wempty-body"
CFLAGS+=" -Wformat"
CFLAGS+=" -Wformat-security"
CFLAGS+=" -Winit-self"
CFLAGS+=" -Winline"
CFLAGS+=" -Wmissing-declarations"
CFLAGS+=" -Wmissing-include-dirs"
CFLAGS+=" -Wmissing-prototypes"
CFLAGS+=" -Wnested-externs"
CFLAGS+=" -Wold-style-definition"
CFLAGS+=" -Wpointer-arith"
CFLAGS+=" -Wredundant-decls"
#CFLAGS+=" -Wshadow"
CFLAGS+=" -Wstrict-prototypes"
CFLAGS+=" -Wswitch-enum"
CFLAGS+=" -Wundef"
CFLAGS+=" -Wuninitialized"
CFLAGS+=" -Wunused"
#CFLAGS+=" -Wwrite-strings"

# warnings disabled on purpose
CFLAGS+=" -Wno-unused-parameter"

export CFLAGS
echo "CFLAGS: $CFLAGS"

configure_args=""

# special configurations
case "$CONF" in
	*werror*)
		# fail on warning
		configure_args+=" --enable-real-werror"

		if [ "$TRAVIS_OS_NAME" = "osx" ]
		then
			# disable deprecated warnings since gdk-pixbuf fails to
			# build with:
			# /usr/local/Cellar/gdk-pixbuf/2.38.1_1/include/gdk-pixbuf-2.0/gdk-pixbuf/gdk-pixbuf-animation.h:122:85: warning: 'GTimeVal' is deprecated: Use 'GDateTime' instead [-Wdeprecated-declarations]
			configure_args+="-Wno-deprecated-declarations"
		fi

		# disable goffice since it uses -pthread that generate warnings and then
		# errors
		configure_args+=" --without-goffice"
	;;

	*goffice*)
		# enable goffice
		configure_args+=" --with-goffice"
	;;
esac

if [ "$TRAVIS_OS_NAME" = "osx" ]
then
	# from brew
	export PKG_CONFIG_PATH=/usr/local/opt/libxml2/lib/pkgconfig:/usr/local/opt/openssl/lib/pkgconfig:/usr/local/opt/libffi/lib/pkgconfig
	export PATH="$PATH:/usr/local/opt/gettext/bin"

	mkdir m4
	ln -sf /usr/local/opt/gettext/share/aclocal/nls.m4 m4
fi

# exit on error
set -e

sh ./autogen.sh

echo "configure_args: $configure_args $@"
./configure $configure_args "$@"
