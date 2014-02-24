
if(NOT ARCH)
    set(ARCH i386)
endif()

if(ENABLE_CCACHE)
    set(CCACHE "ccache" CACHE STRING "ccache")
else()
    set(CCACHE "" CACHE STRING "ccache")
endif()

# The name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR "x86")
set(WATCOM TRUE)

# Which tools to use
set(CMAKE_C_COMPILER wcl386 -aa)
set(CMAKE_CXX_COMPILER wcl386)
set(CMAKE_ASM_COMPILER wasm)
set(CMAKE_ASM_COMPILER_ID "WATCOM")

#### two alternative solutions to getting MC.exe functionality ####
#### a port of the Wine mc (wmc) to watcom ####
#### https://github.com/staalmannen/owmc ####
#set(CMAKE_MC_COMPILER owmc) 
#### install MinGW binutils and make sure that it is in your PATH ####
set(CMAKE_MC_COMPILER windmc)
#### coment out one of the options ####

set(CMAKE_RC_COMPILER wrc)
set(CMAKE_AR wlib)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86")
    add_definitions(-D__i386__)
endif()

set(CMAKE_C_CREATE_STATIC_LIBRARY "<CMAKE_AR> <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_CXX_CREATE_STATIC_LIBRARY ${CMAKE_C_CREATE_STATIC_LIBRARY})
set(CMAKE_ASM_CREATE_STATIC_LIBRARY ${CMAKE_C_CREATE_STATIC_LIBRARY})

# Don't link with anything by default unless we say so
set(CMAKE_C_STANDARD_LIBRARIES CACHE STRING "Standard C Libraries")

#MARK_AS_ADVANCED(CLEAR CMAKE_CXX_STANDARD_LIBRARIES)
set(CMAKE_CXX_STANDARD_LIBRARIES CACHE STRING "Standard C++ Libraries")

set(CMAKE_SHARED_LINKER_FLAGS_INIT "")

