@rem Script to build Lua under "Visual Studio .NET Command Prompt".
@rem Do not run from this directory; run it from the toplevel: etc\luavs.bat .
@rem It creates lua51.dll, lua51.lib, lua.exe, and luac.exe in src.
@rem (contributed by David Manura and Mike Pall)

@setlocal
@set MYCOMPILE=cl /nologo /MD /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE /D_WINDOWS /GL /Gy /Oi /Zi /Fd"vc110.pdb"
@set MYLINK=link /nologo /LTCG /OPT:ICF /OPT:REF /DEBUG /PDB:"lua.pdb"
@set MYMT=mt /nologo

cd src
%MYCOMPILE% /DLUA_BUILD_AS_DLL l*.c slua.c
del lua.obj luac.obj
%MYLINK% /DLL /out:slua.dll l*.obj slua.obj
if exist slua.dll.manifest^
  %MYMT% -manifest slua.dll.manifest -outputresource:slua.dll;2
%MYCOMPILE% l*.c
del luac.obj
%MYLINK% /out:lua.exe *.obj
if exist lua.exe.manifest^
  %MYMT% -manifest lua.exe.manifest -outputresource:lua.exe
%MYCOMPILE% l*.c print.c
del lua.obj linit.obj lbaselib.obj ldblib.obj liolib.obj lmathlib.obj^
    loslib.obj ltablib.obj lstrlib.obj loadlib.obj
%MYLINK% /out:luac.exe *.obj
if exist luac.exe.manifest^
  %MYMT% -manifest luac.exe.manifest -outputresource:luac.exe
del *.obj *.manifest
cd ..
