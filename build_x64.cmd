@echo off

call "C:\Microsoft SDK\vcvarsall.bat" x64
call "%~dp0minicrt\minicrt.cmd" x64

rc /I "resource" /Fo"resource.res" resource\resource.rc

cl /O1 /Os /GL /GF /GS- switchMode.c resource.res ^
/link memcpy.obj minicrt.lib kernel32.lib user32.lib Gdi32.lib Wininet.lib Advapi32.lib ^
/NODEFAULTLIB /DYNAMICBASE:NO /FIXED /SUBSYSTEM:WINDOWS
del *.obj
pause