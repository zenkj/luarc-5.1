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
  realrun ./lua5164 $*
  realrun ./lua4g5164 $*
  realrun ./luarc5164 $*
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
mkdir -p result
run lua/binarytrees.lua-2.lua 5
run lua/binarytrees.lua-3.lua 5
run lua/fannkuchredux.lua 8
run lua/spectralnorm.lua 5

cg
