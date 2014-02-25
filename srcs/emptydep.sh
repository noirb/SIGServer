#!/bin/sh -x
# $Id: emptydep.sh,v 1.6 2011-07-27 06:06:04 okamoto Exp $

for dir in $@
do
  for f in `find $dir -name Makefile.dep`
	do
	  pushd ./
	  cd `dirname $f`
	  rm -f Makefile.dep
	  touch Makefile.dep
	  popd
	done
done

