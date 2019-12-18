@echo off

call "C:\Microsoft SDK\vcvarsall.bat"
call "%~dp0minicrt\minicrt.cmd"

rc /I "resource" /Fo"resource.res" resource\resource.rc

cl /O1 /Os /GL /GF /GS- switchMode.c resource.res ^
/link minicrt.lib kernel32.lib user32.lib Gdi32.lib Wininet.lib Comctl32.lib Advapi32.lib ^
/NODEFAULTLIB /DYNAMICBASE:NO /FIXED /SUBSYSTEM:WINDOWS
del *.obj
pause