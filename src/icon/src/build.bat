@echo off
windres .\icon.rc -O coff -o ..\icon_i386.res -F pe-i386
windres .\icon.rc -O coff -o ..\icon_x64.res -F pe-x86-64