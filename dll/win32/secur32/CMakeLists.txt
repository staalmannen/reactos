
add_definitions(-D__SECUR32__)
include_directories(${REACTOS_SOURCE_DIR}/include/reactos/subsys)

spec2def(secur32.dll secur32.spec ADD_IMPORTLIB)

list(APPEND SOURCE
    dllmain.c
    secext.c
    sspi.c
    stubs.c
    precomp.h)

add_library(secur32 SHARED
    ${SOURCE}
    secur32.rc
    ${CMAKE_CURRENT_BINARY_DIR}/secur32_stubs.c
    ${CMAKE_CURRENT_BINARY_DIR}/secur32.def)

set_module_type(secur32 win32dll UNICODE ENTRYPOINT DllMain 12)

target_link_libraries(secur32 lsalib)

add_importlibs(secur32 advapi32 kernel32 ntdll)
add_pch(secur32 precomp.h SOURCE)

add_cd_file(TARGET secur32 DESTINATION reactos/system32 FOR all)

