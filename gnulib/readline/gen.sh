#!/bin/bash -e

URL="https://ftp.gnu.org/gnu/readline/readline-8.2.tar.gz"

if [ ! -d readline ] ; then
	curl -LO $URL
	tar -xkf $(basename $URL)
	mv readline-* readline
	cd readline/
	patch -p1 < ../readline82.patch
	cd ..
fi


