
include_directories(
    ${REACTOS_SOURCE_DIR}/include/reactos/libs/fast486
    ntvdm)

spec2def(ntvdm.exe ntvdm.spec)

list(APPEND SOURCE
    bios/bios32/bios32.c
    bios/bios32/kbdbios32.c
    bios/bios32/vidbios32.c
    bios/bios.c
    bios/kbdbios.c
    bios/rom.c
    bios/vidbios.c
    hardware/cmos.c
    hardware/pic.c
    hardware/ps2.c
    hardware/speaker.c
    hardware/timer.c
    hardware/vga.c
    dos/dos32krnl/bios.c
    dos/dos32krnl/dos.c
    dos/dos32krnl/dosfiles.c
    dos/dem.c
    bop.c
    callback.c
    clock.c
    emulator.c
    io.c
    registers.c
    utils.c
    vddsup.c
    ntvdm.c
    ntvdm.rc
    ${CMAKE_CURRENT_BINARY_DIR}/ntvdm.def)

add_executable(ntvdm ${SOURCE})
set_module_type(ntvdm win32cui UNICODE)
set_image_base(ntvdm 0x0F000000)
target_link_libraries(ntvdm fast486)
add_importlibs(ntvdm user32 gdi32 advapi32 msvcrt kernel32 ntdll)
add_cd_file(TARGET ntvdm DESTINATION reactos/system32 FOR all)
