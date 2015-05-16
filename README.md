#Luarc-5.1

An implementation of reference counting memory management together with
Mark/Sweep GC implemented in Lua 5.1.4.

#Implementation

A pure reference counting implementation is used, i.e., heap objects will
be reclaimed as soon as the reference count becomes 0, deferring release
is not used. Yes, more cpu cycles are used for parameter passing and 
other stack manipulation because of the reference count update, and there's
chance that big delay may occur when a big data structure is released. 
But the benefit is great too, any memory block will be released as soon as
it is not used, no "garbage memory bubble" exists, the memory footprint is
very small, and less GC overhead occurs. In some of tests, almost no GC is 
triggered when the application is running. This is extremely suitable for
devices with small memory capacity and many temporary heap objects.

The Mark/Sweep GC is used to reclaim garbage heap objects that have
circular reference, which can't be reclaimed by reference counting.

#Performance

Test cases come from [Computer Language Benchmarks Game](http://benchmarksgame.alioth.debian.org/play.php). 
The cpu time is measured by the time command. The memory usage is RSS value
of ps command.

###Execution Time
![execution time](https://raw.github.com/zenkj/luarc-5.1/master/doc/cpu.png)

In most test cases, vanilla Lua is faster than Luarc. This is true because
most test cases do not test memory usage. But for the binary-tree-2
test case, Luarc is much faster than vanilla lua (almost 37% improvement).
In the binary-tree-3 test case, gcstepmul is set to 0, this means incremental
feature of the Lua GC is disabled. The overall GC throughput is improved, but
big delay occures.

###Memory Usage
![memory usage](https://raw.github.com/zenkj/luarc-5.1/master/doc/memory-usage.png)
Because of no garbage memory bubble exists, Luarc has small memory footprint.

###Memory Stability
![memory stability for binarytrees-2](https://raw.github.com/zenkj/luarc-5.1/master/doc/mem-binarytrees-2-arg16.png)
![memory stability for binarytrees-3](https://raw.github.com/zenkj/luarc-5.1/master/doc/mem-binarytrees-3-arg16.png)

Memory usage is more stable for Luarc.

#History
After some investigation, Only one reference counting implementation for lua
is found in LuaPlus, but the version that I can get has many bugs and is not usable.
So I started this project.
