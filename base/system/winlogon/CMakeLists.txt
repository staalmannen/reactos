
list(APPEND SOURCE
    environment.c
    sas.c
    screensaver.c
    setup.c
    winlogon.c
    wlx.c
    winlogon.h)

add_executable(winlogon ${SOURCE} winlogon.rc)
target_link_libraries(winlogon wine)
set_module_type(winlogon win32gui)
add_importlibs(winlogon user32 advapi32 userenv secur32 msvcrt kernel32 ntdll)
add_pch(winlogon winlogon.h SOURCE)
add_cd_file(TARGET winlogon DESTINATION reactos/system32 FOR all)
