#!/bin/sh

touch config.rpath
aclocal
automake --add-missing --foreign
autoconf
intltoolize --automake --force
