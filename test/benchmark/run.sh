#!/bin/sh

output() {
  echo $*
  echo $* >>test.output
}

realrun() {
  prog=$1
  
  output
  output begin to test $*
  output ==================================
  /usr/bin/time $* >>test.output&
  i=0
  pid=
  while [ "x$pid" = "x" ]; do
    echo try to get the pid of $prog process...
    pid=$(ps -u $USER -o pid,cmd | awk '/^ *[0-9][0-9]* '$prog'/ {print $1}')
    i=$(expr $i + 1)
    if [ $i -ge 5 ]; then
      echo "no process created"
      exit
    fi
    sleep 1
  done
  echo Got! $prog pid= $pid

  while kill -0 $pid; do
    ps -o pid,pcpu,pmem,rss,vsize -p $pid
    sleep 1
  done
}

rm -f test.result
rm -f test.output

runone() {
  echo -n $* ' ... '
  t1=`date +%s`
  realrun $* >>test.result 2>&1
  t2=`date +%s`
  dt=$(expr $t2 - $t1)
  echo $dt seconds
}

run() {
  echo ======================================
  runone lua51 $* 
  runone luarc51 $*
}


#2.67s/1.53s
#run binarytrees.lua-2.lua 13
#run binarytrees.lua-2.lua 14
#13s/8s
#run binarytrees.lua-2.lua 15
run binarytrees.lua-2.lua 16
#63s/42s
#run binarytrees.lua-2.lua 17
#run binarytrees.lua-2.lua 18
#1.54s/1.44s
#run binarytrees.lua-3.lua 13
#run binarytrees.lua-3.lua 14
#9s/8s
#run binarytrees.lua-3.lua 15
run binarytrees.lua-3.lua 16
#40s/49s
#run binarytrees.lua-3.lua 17
#run binarytrees.lua-3.lua 18
#10s/12s
run fannkuchredux.lua 10
#125s/143s
#run fannkuchredux.lua 11
#1s/1s
#run fasta.lua 10000
#0s/0s
#run knucleotide.lua-2.lua
#8s/9s
#run mandelbrot.lua 1500
#13s/15s
run mandelbrot.lua 2000
#15s/16s
run mandelbrot.lua-2.lua 2000
#15s/16s (double-free -- ok)
run mandelbrot.lua-6.lua 2000
#1s/1s (double-free coredump -- ok)
#run meteor.lua
#1s/2s (double-free coredump -- ok)
#run meteor.lua-2.lua
#1s/2s (double-free coredump -- ok)
#run meteor.lua-3.lua
#1s/1s
#run meteor.lua-4.lua
#1s/1s
#run nbody.lua 100000
#run nbody.lua-2.lua
#run nbody.lua-4.lua
#0/0 (no required module c-gmp)
#run pidigits.lua 10000
#0/0
#run pidigits.lua-2.lua
#run pidigits.lua-3.lua
#run pidigits.lua-4.lua
#run pidigits.lua-5.lua
#run pidigits.lua-7.lua
#run pidigits.lua-8.lua
#0/0
#run regexdna.lua 100000
#run regexdna.lua-2.lua
#run regexdna.lua-3.lua
#0/0
#run revcomp.lua
#run revcomp.lua-3.lua
#10s/11s
run spectralnorm.lua 1000
#34s/41s
#run spectralnorm.lua 2000
#0/0
#run threadring.lua-3.lua 1000
