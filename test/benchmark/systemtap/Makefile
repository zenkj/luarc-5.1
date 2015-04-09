prog = /home/jzjian/bin/lua51
luascript = ../binarytrees.lua-2.lua 16
systemtap: gc.stap
	stap -v --ldd -d $(prog) gc.stap -c $(prog) $(luascript)
