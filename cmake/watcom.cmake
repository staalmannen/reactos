
if(ARCH STREQUAL "i386")
    add_definitions(-dWIN32 -d_WINDOWS -u_M_IX86)
endif()

add_definitions(-dinline=__inline)

add_compile_flags("-aa -ze -fp6 -oneatxl -mf -bcl=NT")


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS " ${CMAKE_MODULE_LINKER_FLAGS}")


set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> ${I18N_DEFS} -fo=<OBJECT> <SOURCE>")
set(CMAKE_ASM_COMPILE_OBJECT
    "wasm -i=${REACTOS_SOURCE_DIR}/include/asm -i=${REACTOS_BINARY_DIR}/include/asm <FLAGS> <DEFINES> -d__ASM__ <SOURCE> > <OBJECT>.tmp"
        "<CMAKE_ASM_COMPILER> <OBJECT>.tmp")

set(CMAKE_RC_CREATE_SHARED_LIBRARY "${CMAKE_C_CREATE_SHARED_LIBRARY}")
set(CMAKE_ASM_CREATE_SHARED_LIBRARY "${CMAKE_C_CREATE_SHARED_LIBRARY}")
set(CMAKE_ASM_CREATE_STATIC_LIBRARY ${CMAKE_C_CREATE_STATIC_LIBRARY})

macro(add_pch _target_name _FILE)
endmacro()

function(set_entrypoint _module _entrypoint)
    if(${_entrypoint} STREQUAL "0")
        add_target_link_flags(${_module} "")
    elseif(ARCH STREQUAL "i386")
        set(_entrysymbol ${_entrypoint})
        if(${ARGC} GREATER 2)
            set(_entrysymbol ${_entrysymbol}@${ARGV2})
        endif()
        add_target_link_flags(${_module} "export ::= ${_entrysymbol}")
    else()
        add_target_link_flags(${_module} "export ::= ${_entrypoint}")
    endif()
endfunction()

function(set_subsystem MODULE SUBSYSTEM)
    if(ARCH STREQUAL "amd64")
        add_target_link_flags(${MODULE} "-subsystem:${SUBSYSTEM},5.02")
    else()
        add_target_link_flags(${MODULE} "-subsystem:${SUBSYSTEM},5.01")
    endif()
endfunction()

function(set_image_base MODULE IMAGE_BASE)
    add_target_link_flags(${MODULE} "-base:${IMAGE_BASE}")
endfunction()

function(set_module_type_toolchain MODULE TYPE)
    if(CPP_USE_STL)
        if((${TYPE} STREQUAL "kernelmodedriver") OR (${TYPE} STREQUAL "wdmdriver"))
            message(FATAL_ERROR "Use of STL in kernelmodedriver or wdmdriver type module prohibited")
        endif()
        target_link_libraries(${MODULE} cpprt stlport oldnames)
    elseif(CPP_USE_RT)
        target_link_libraries(${MODULE} cpprt)
    endif()
    if((${TYPE} STREQUAL "win32dll") OR (${TYPE} STREQUAL "win32ocx") OR (${TYPE} STREQUAL "cpl"))
	add_compile_flags("-bd -l=nt_dll")
        add_target_link_flags(${MODULE} "")
    elseif(${TYPE} STREQUAL "kernelmodedriver")
	add_compile_flags("-mc -d1 -s -wx -zl -zfp -zgp -zu")
        add_target_link_flags(${MODULE} "")
    elseif(${TYPE} STREQUAL "wdmdriver")
        add_target_link_flags(${MODULE} "op quiet, map f")
    endif()
endfunction()

#define those for having real libraries
set(CMAKE_IMPLIB_CREATE_STATIC_LIBRARY "LINK  <LINK_FLAGS> -o=<TARGET> <OBJECTS>")
set(CMAKE_STUB_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> -cp -fo=<OBJECT> -c <SOURCE>")
function(add_delay_importlibs _module)
    get_target_property(_module_type ${_module} TYPE)
    if(_module_type STREQUAL "STATIC_LIBRARY")
        message(FATAL_ERROR "Cannot add delay imports to a static library")
    endif()
    foreach(_lib ${ARGN})
        target_link_libraries(${_module} lib${_lib}_delayed)
    endforeach()
    target_link_libraries(${_module} delayimp)
endfunction()

function(generate_import_lib _libname _dllname _spec_file)

    set(_def_file ${CMAKE_CURRENT_BINARY_DIR}/${_libname}_exp.def)
    set(_asm_stubs_file ${CMAKE_CURRENT_BINARY_DIR}/${_libname}_stubs.asm)

    # Generate the asm stub file and the def file for import library
    add_custom_command(
        OUTPUT ${_asm_stubs_file} ${_def_file}
        COMMAND native-spec2def --ms -a=${SPEC2DEF_ARCH} --implib -n=${_dllname} -d=${_def_file} -l=${_asm_stubs_file} ${CMAKE_CURRENT_SOURCE_DIR}/${_spec_file}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_spec_file} native-spec2def)

        # be clear about the "language"
        # Thanks MS for creating a stupid linker
        set_source_files_properties(${_asm_stubs_file} PROPERTIES LANGUAGE "STUB_ASM")

        # NOTE: as stub file and def file are generated in one pass, depending on one is like depending on the other
        add_library(${_libname} STATIC EXCLUDE_FROM_ALL ${_asm_stubs_file})
        add_dependencies(${_libname} ${_def_file})
        # set correct "link rule"
        set_target_properties(${_libname} PROPERTIES LINKER_LANGUAGE "IMPLIB")
    set_target_properties(${_libname} PROPERTIES STATIC_LIBRARY_FLAGS "-def:${_def_file}")
endfunction()

if(ARCH STREQUAL "amd64")
    add_definitions(-d__x86_64)
    set(SPEC2DEF_ARCH x86_64)
else()
    set(SPEC2DEF_ARCH i386)
endif()
function(spec2def _dllname _spec_file)
    # do we also want to add importlib targets?
    if(${ARGC} GREATER 2)
        if(${ARGN} STREQUAL "ADD_IMPORTLIB")
            set(__add_importlib TRUE)
        else()
            message(FATAL_ERROR "Wrong argument passed to spec2def, ${ARGN}")
        endif()
    endif()

    # get library basename
    get_filename_component(_file ${_dllname} NAME_WE)

    # error out on anything else than spec
    if(NOT ${_spec_file} MATCHES ".*\\.spec")
        message(FATAL_ERROR "spec2def only takes spec files as input.")
    endif()

    #generate def for the DLL and C stubs file
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_file}.def ${CMAKE_CURRENT_BINARY_DIR}/${_file}_stubs.c
        COMMAND native-spec2def --ms -a=${SPEC2DEF_ARCH} -n=${_dllname} -d=${CMAKE_CURRENT_BINARY_DIR}/${_file}.def -s=${CMAKE_CURRENT_BINARY_DIR}/${_file}_stubs.c ${CMAKE_CURRENT_SOURCE_DIR}/${_spec_file}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_spec_file} native-spec2def)

    if(__add_importlib)
        generate_import_lib(lib${_file} ${_dllname} ${_spec_file})
    endif()
endfunction()

macro(macro_mc FLAG FILE)
    set(COMMAND_MC ${CMAKE_MC_COMPILER} ${FLAG} -r ${REACTOS_BINARY_DIR}/include/reactos -h ${REACTOS_BINARY_DIR}/include/reactos ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.mc)
endmacro()

#pseh workaround
set(PSEH_LIB "pseh")

if((ARCH STREQUAL "amd64"))
    set(CMAKE_ASM16_COMPILER wcl.exe)
else()
    set(CMAKE_ASM16_COMPILER wcl.exe)
endif()

function(CreateBootSectorTarget _target_name _asm_file _binary_file _base_address)
    set(_object_file ${_binary_file}.obj)
    set(_temp_file ${_binary_file}.tmp)

    add_custom_command(
        OUTPUT ${_temp_file}
        COMMAND ${CMAKE_C_COMPILER} -i${REACTOS_SOURCE_DIR}/include/asm -i${REACTOS_BINARY_DIR}/include/asm -d__ASM__ -d_USE_ML -ep -c ${_asm_file} > ${_temp_file}
        DEPENDS ${_asm_file})

    add_custom_command(
        OUTPUT ${_object_file}
        COMMAND ${CMAKE_ASM16_COMPILER} -fo=${_object_file} -c ${_temp_file}
        DEPENDS ${_temp_file})

    add_custom_command(
        OUTPUT ${_binary_file}
        COMMAND native-obj2bin ${_object_file} ${_binary_file} ${_base_address}
        DEPENDS ${_object_file} native-obj2bin)

    set_source_files_properties(${_object_file} ${_temp_file} ${_binary_file} PROPERTIES GENERATED TRUE)

    add_custom_target(${_target_name} ALL DEPENDS ${_binary_file})
endfunction()

function(allow_warnings __module)
endfunction()

macro(add_asm_files _target)
        list(APPEND ${_target} ${ARGN})
endmacro()
