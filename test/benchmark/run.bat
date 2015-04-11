@echo off
mkdir result 2>nul

call :run lua\binarytrees.lua-2.lua 16
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
echo ====================================
echo %* ...
set command=%1
start timer.bat %* 
:again
timeout /t 1
tasklist | findstr %command% 
if errorlevel 1 exit /b 0
goto :again

