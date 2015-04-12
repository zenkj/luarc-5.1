@echo off
call :logblank 
call :log =======================================
call :log %*
call :log %time%
%* 1>>result\run.output 2>>result\run.result
call :log %time%
exit

:log
echo %*
echo %* >>result\run.output
echo %* >>result\run.result
exit /b 0

:logblank
rem any unused specila char append to echo to echo blank line
rem ( is the recommanded always-compatible char
echo(
echo( >>result\run.output
echo( >>result\run.result
exit /b 0
