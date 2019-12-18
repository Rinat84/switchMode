@echo off

set MINICRT=%~dp0

if "%1" == "" goto x86

if /i %1 == x86       goto x86
if /i %1 == x64       goto x64

:x86
  set INCLUDE=%MINICRT%include;%INCLUDE%
  set LIB=%MINICRT%lib;%LIB%
goto :eof

:x64
  set INCLUDE=%MINICRT%include;%INCLUDE%
  set LIB=%MINICRT%lib\amd64;%LIB%
goto :eof
