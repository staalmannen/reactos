
remove_definitions(-D_WIN32_WINNT=0x502)
add_definitions(-D_WIN32_WINNT=0x600)

add_executable(reg reg.c reg.rc)
set_module_type(reg win32cui UNICODE)
add_importlibs(reg advapi32 user32 msvcrt kernel32)
add_cd_file(TARGET reg DESTINATION reactos/system32 FOR all)
