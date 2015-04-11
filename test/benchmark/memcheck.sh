#!/bin/sh

LOGFILE=result/memcheck.result

output() {
  echo $*
  echo $* >>$LOGFILE
}

realrun() {
  output
  output valgrind $* ...
  valgrind --num-callers=50 $*  2>&1 | tee -a $LOGFILE
}

run() {
  output =======================================
  realrun ./luarc5164 $*
  realrun ./luarc5132 $*
}

rm -f $LOGFILE
mkdir -p result
run lua/binarytrees.lua-2.lua 
run lua/binarytrees.lua-3.lua
run lua/fannkuchredux.lua 5
run lua/fasta.lua
run lua/knucleotide.lua-2.lua <lua/knucleotide.lua-2.lua
run lua/mandelbrot.lua
run lua/mandelbrot.lua-2.lua
run lua/mandelbrot.lua-6.lua
run lua/meteor.lua
run lua/meteor.lua-2.lua
run lua/meteor.lua-3.lua
run lua/meteor.lua-4.lua
run lua/nbody.lua
run lua/nbody.lua-2.lua
run lua/nbody.lua-4.lua
#need c-gmp module
#run lua/pidigits.lua
#run lua/pidigits.lua-2.lua
#run lua/pidigits.lua-3.lua
#run lua/pidigits.lua-4.lua
#run lua/pidigits.lua-5.lua
#run lua/pidigits.lua-7.lua
#run lua/pidigits.lua-8.lua
run lua/regexdna.lua <lua/regexdna.lua
run lua/regexdna.lua-2.lua <lua/regexdna.lua-2.lua
#need 're' module
#run lua/regexdna.lua-3.lua <lua/regexdna.lua-3.lua
#run lua/revcomp.lua <lua/revcomp.lua
#run lua/revcomp.lua-3.lua <lua/revcomp.lua-3.lua
run lua/spectralnorm.lua
run lua/threadring.lua-3.lua 1
