@echo off
mkdir result 2>nul
del result\run.result
del result\run.output
del result\run.memory

call :run lua\binarytrees.lua-2.lua 15
exit /b 0

:run
call :runone lua5132.exe %*
call :runone lua4g5132.exe -x1 %*
call :runone luarc5132.exe -x1 %*
call :runone lua5164.exe %*
call :runone lua4g5164.exe -x1 %*
call :runone luarc5164.exe -x1 %*
exit /b 0


:runone
call :logblank
call :log  ====================================
call :log  %* ...
set command=%1
start runwrapper.bat %* 
echo %time%
:again
timeout /t 1 >nul
tasklist | findstr %command% >>result\run.memory
echo %time%
if errorlevel 1 exit /b 0
goto :again

:log
echo %*
echo %* >>result\run.memory
exit /b 0

:logblank
rem any unused specila char append to echo to echo blank line
rem ( is the recommanded always-compatible char
echo(
echo( >>result\run.memory
exit /b 0
