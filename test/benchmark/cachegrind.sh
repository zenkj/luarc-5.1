#!/bin/sh

LOGFILE=result/cachegrind.annotate
log() {
  echo $*
  echo $*>>$LOGFILE
}

realrun() {
  log
  log ===================================================
  log $* ...
  valgrind --tool=cachegrind --cachegrind-out-file=result/cachegrind.out.%p $*
}

run() {
  realrun lua51 $*
  realrun lua4g51 $*
  realrun lua4g51t $*
  realrun luarc51 $*
}

cg() {
  for f in result/cachegrind.out.*; do
    log
    log ===================================================
    log annotate $f ...
    cg_annotate $f >>$LOGFILE
  done
  log done.
}

rm -f result/cachegrind.out.* $LOGFILE
run binarytrees.lua-2.lua 5
run binarytrees.lua-3.lua 5
#run fannkuchredux.lua 8

cg
