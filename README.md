wenboi
======

A simple Game Boy emulator

Build instructions
==================

wenboi uses CMake for building and depends on Qt, so you should install
the qt development package for your distro of choice and do this at the
repository root:

    mkdir build
    cd build
    cmake ..
    make
    
This will build libwenboicore.a, the wenboi core library, qtboi, the
emulator front-end, and wendi, the old wenboi disassembler. The emulator
front-end has an integrated debugger and interactive disassembler, you
probably want to run that one :)
