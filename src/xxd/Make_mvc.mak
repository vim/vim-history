# The most simplistic Makefile for Win32 using Microsoft Visual C++
# (NT and Windows 95)

xxd: xxd.exe

xxd.exe: xxd.c
     cl -DWIN32 xxd.c /link setargv.obj

clean:
     - del xxd.obj
     - del xxd.exe
