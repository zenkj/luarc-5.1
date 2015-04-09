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
  realrun luarc51 $*
}

rm -f $LOGFILE
run binarytrees.lua-2.lua 
run binarytrees.lua-3.lua
run fannkuchredux.lua 5
run fasta.lua
run knucleotide.lua-2.lua <knucleotide.lua-2.lua
run mandelbrot.lua
run mandelbrot.lua-2.lua
run mandelbrot.lua-6.lua
run meteor.lua
run meteor.lua-2.lua
run meteor.lua-3.lua
run meteor.lua-4.lua
run nbody.lua
run nbody.lua-2.lua
run nbody.lua-4.lua
#need c-gmp module
#run pidigits.lua
#run pidigits.lua-2.lua
#run pidigits.lua-3.lua
#run pidigits.lua-4.lua
#run pidigits.lua-5.lua
#run pidigits.lua-7.lua
#run pidigits.lua-8.lua
run regexdna.lua <regexdna.lua
run regexdna.lua-2.lua <regexdna.lua-2.lua
#need 're' module
#run regexdna.lua-3.lua <regexdna.lua-3.lua
#run revcomp.lua <revcomp.lua
#run revcomp.lua-3.lua <revcomp.lua-3.lua
run spectralnorm.lua
run threadring.lua-3.lua 1
