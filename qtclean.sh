#! /bin/sh

if [ -f Makefile ]; then
    make distclean
fi

test -d .moc && rmdir .moc
test -d .obj && rmdir .obj
test -d .ui  && rmdir .ui

