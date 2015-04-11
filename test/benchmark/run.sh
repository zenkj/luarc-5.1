#!/bin/sh

LOGFILE=result/test.result
OUTPUT=result/test.output

output() {
  echo $*
  echo $* >>$OUTPUT
}

realrun() {
  prog=$1
  cookedprog=`echo $1 | tr '/' '.'`
  
  output
  output begin to test $*
  output ==================================
  /usr/bin/time $* >>$OUTPUT&
  i=0
  pid=
  while [ "x$pid" = "x" ]; do
    echo try to get the pid of $prog process...
    pid=$(ps -u $USER -o pid,cmd | awk '/^ *[0-9][0-9]* '$cookedprog'/ {print $1}')
    i=$(expr $i + 1)
    if [ $i -ge 5 ]; then
      echo "no process created"
      exit
    fi
    sleep 1
  done
  echo Got! $prog pid= $pid

  while kill -0 $pid 2>/dev/null; do
    ps -o pid,pcpu,pmem,rss,vsize -p $pid
    sleep 1
  done
}

runone() {
  echo -n $* ' ... '
  t1=`date +%s`
  realrun $* >>$LOGFILE 2>&1
  t2=`date +%s`
  dt=$(expr $t2 - $t1)
  echo $dt seconds
}

run() {
  echo ======================================
  runone ./lua5164 $* 
  runone ./lua4g5164 $* 
  runone ./lua4g5164 -x2 $* 
  runone ./luarc5164 $*
  runone ./luarc5164 -x2 $*
  runone ./lua5132 $* 
  runone ./lua4g5132 $* 
  runone ./lua4g5132 -x2 $* 
  runone ./luarc5132 $*
  runone ./luarc5132 -x2 $*
}


rm -f $LOGFILE
rm -f $OUTPUT
mkdir -p result

#2.67s/1.53s
#run lua/binarytrees.lua-2.lua 13
#run lua/binarytrees.lua-2.lua 14
#13s/8s
#run lua/binarytrees.lua-2.lua 15
run lua/binarytrees.lua-2.lua 16
#63s/42s
#run lua/binarytrees.lua-2.lua 17
#run lua/binarytrees.lua-2.lua 18
#1.54s/1.44s
#run lua/binarytrees.lua-3.lua 13
#run lua/binarytrees.lua-3.lua 14
#9s/8s
#run lua/binarytrees.lua-3.lua 15
###run lua/binarytrees.lua-3.lua 16
#40s/49s
#run lua/binarytrees.lua-3.lua 17
#run lua/binarytrees.lua-3.lua 18
#10s/12s
#run lua/fannkuchredux.lua 10
#125s/143s
###run lua/fannkuchredux.lua 11
#1s/1s
#run lua/fasta.lua 10000
#0s/0s
#run lua/knucleotide.lua-2.lua
#8s/9s
#run lua/mandelbrot.lua 1500
#13s/15s
#run lua/mandelbrot.lua 2000
#15s/16s
#run lua/mandelbrot.lua-2.lua 2000
#15s/16s (double-free -- ok)
###run lua/mandelbrot.lua-6.lua 2000
#1s/1s (double-free coredump -- ok)
#run lua/meteor.lua
#1s/2s (double-free coredump -- ok)
#run lua/meteor.lua-2.lua
#1s/2s (double-free coredump -- ok)
#run lua/meteor.lua-3.lua
#1s/1s
#run lua/meteor.lua-4.lua
#1s/1s
#run lua/nbody.lua 100000
#run lua/nbody.lua-2.lua
#run lua/nbody.lua-4.lua
#0/0 (no required module c-gmp)
#run lua/pidigits.lua 10000
#0/0
#run lua/pidigits.lua-2.lua
#run lua/pidigits.lua-3.lua
#run lua/pidigits.lua-4.lua
#run lua/pidigits.lua-5.lua
#run lua/pidigits.lua-7.lua
#run lua/pidigits.lua-8.lua
#0/0
#run lua/regexdna.lua 100000
#run lua/regexdna.lua-2.lua
#run lua/regexdna.lua-3.lua
#0/0
#run lua/revcomp.lua
#run lua/revcomp.lua-3.lua
#10s/11s
#run lua/spectralnorm.lua 1000
#34s/41s
###run lua/spectralnorm.lua 2000
#0/0
#run lua/threadring.lua-3.lua 1000
