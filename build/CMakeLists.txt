# Tencent is pleased to support the open source community by making xLua available.
# Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
# Licensed under the MIT License (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
# http://opensource.org/licenses/MIT
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

cmake_minimum_required(VERSION 2.8)

if ( WIN32 AND NOT CYGWIN AND NOT ( CMAKE_SYSTEM_NAME STREQUAL "WindowsStore" ) AND NOT ANDROID)
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT" CACHE STRING "")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd" CACHE STRING "")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT" CACHE STRING "")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd" CACHE STRING "")
endif ()

project(XLua)

if ( IOS )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fembed-bitcode")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fembed-bitcode")
endif ()

option ( UINT_ESPECIALLY "using custom ulong" OFF )
option ( USING_LUAJIT "using luajit" OFF )
option ( GC64 "using gc64" OFF )
option ( LUAC_COMPATIBLE_FORMAT "compatible format" OFF )

find_path(XLUA_PROJECT_DIR NAMES SConstruct
    PATHS 
    ${CMAKE_SOURCE_DIR}
    NO_DEFAULT_PATH
    )

MARK_AS_ADVANCED(XLUA_PROJECT_DIR)

#
set (CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "10.0" CACHE string "iOS Deployment Target")
set (CMAKE_OSX_ARCHITECTURES "arm64" CACHE string "Build architecture for iOS")

#数据读取
set (READER_SRC 
	lua-reader/source/LUAMain.cpp
	lua-reader/source/LUAReader.cpp
	lua-reader/source/AnyLog.cpp
	lua-reader/source/Parser.cpp
	lua-reader/source/Reader.cpp
	lua-reader/source/stdafx.cpp
)
set_property(
	SOURCE ${READER_SRC}
	APPEND
	PROPERTY COMPILE_DEFINITIONS
	LUA_LIB
)

set(SYSTEM_DIR
	lua-reader/source/boost-1.60.0/system/src/error_code.cpp
)

add_library(libboost_system-vc140-mt-s-1_60 STATIC ${SYSTEM_DIR})
	target_include_directories(libboost_system-vc140-mt-s-1_60
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

list (APPEND THIRDPART_LIB libboost_system-vc140-mt-s-1_60)

set(FILE_SYSTEM_DIR
	lua-reader/source/boost-1.60.0/filesystem/src/windows_file_codecvt.cpp
	lua-reader/source/boost-1.60.0/filesystem/src/utf8_codecvt_facet.cpp
	lua-reader/source/boost-1.60.0/filesystem/src/unique_path.cpp
	lua-reader/source/boost-1.60.0/filesystem/src/portability.cpp
	lua-reader/source/boost-1.60.0/filesystem/src/path_traits.cpp
	lua-reader/source/boost-1.60.0/filesystem/src/path.cpp
	lua-reader/source/boost-1.60.0/filesystem/src/operations.cpp
	lua-reader/source/boost-1.60.0/filesystem/src/codecvt_error_category.cpp
)

add_library(libboost_filesystem-vc140-mt-s-1_60 STATIC ${FILE_SYSTEM_DIR})
	target_include_directories(libboost_filesystem-vc140-mt-s-1_60
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)


list (APPEND THIRDPART_LIB libboost_filesystem-vc140-mt-s-1_60)

#########BOOST库目录
list(APPEND THIRDPART_INC lua-reader/include/boost-1.60.0)

set(protobuf_STAT_SRCS
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/compiler/importer.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/compiler/parser.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/descriptor.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/descriptor.pb.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/descriptor_database.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/dynamic_message.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/extension_set.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/extension_set_heavy.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/generated_message_reflection.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/generated_message_util.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/io/coded_stream.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/io/gzip_stream.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/io/printer.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/io/strtod.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/io/tokenizer.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/io/zero_copy_stream.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/io/zero_copy_stream_impl.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/io/zero_copy_stream_impl_lite.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/message.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/message_lite.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/reflection_ops.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/repeated_field.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/service.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/stubs/common.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/stubs/once.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/stubs/stringprintf.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/stubs/structurally_valid.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/stubs/strutil.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/stubs/substitute.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/text_format.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/unknown_field_set.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/wire_format.cc
  lua-reader/source/protobuf-2.6.1/src/google/protobuf/wire_format_lite.cc
)

if (MSVC)
  set(protobuf_STAT_SRCS
    ${protobuf_STAT_SRCS}
    lua-reader/source/protobuf-2.6.1/src/google/protobuf/stubs/atomicops_internals_x86_msvc.cc
  )
  add_compile_options(/wd4800)
else()
  set(protobuf_STAT_SRCS
    ${protobuf_STAT_SRCS}
    lua-reader/source/protobuf-2.6.1/src/google/protobuf/stubs/atomicops_internals_x86_gcc.cc
  )
endif()

add_library(protobuf STATIC ${protobuf_STAT_SRCS})

target_include_directories(protobuf
  PUBLIC
    lua-reader/source/protobuf-2.6.1/src
  PRIVATE
    lua-reader/source/protobuf-2.6.1/tc_custom
)

list(APPEND THIRDPART_INC lua-reader/include/protobuf-2.6.1)
list (APPEND THIRDPART_LIB protobuf)

set (THIRDPART_SRC ${THIRDPART_SRC} ${READER_SRC})
#数据读取

#Pbc
  set (PBC_SRC
    pbc/src/source/alloc.c
    pbc/src/source/array.c
    pbc/src/source/bootstrap.c
    pbc/src/source/context.c
    pbc/src/source/decode.c
    pbc/src/source/map.c
    pbc/src/source/pattern.c
    pbc/src/source/proto.c
    pbc/src/source/register.c
    pbc/src/source/rmessage.c
    pbc/src/source/stringpool.c
    pbc/src/source/varint.c
    pbc/src/source/wmessage.c
    pbc/src/source/pbc-lua53.c
    )
  set_property(
    SOURCE ${PBC_SRC}
    APPEND
    PROPERTY COMPILE_DEFINITIONS
    LUA_LIB
  )
  list(APPEND THIRDPART_INC pbc/src/include)
  set (THIRDPART_SRC ${THIRDPART_SRC} ${PBC_SRC})
#Pbc

#Gbk
  set (GBK_SRC 
    lua-gbk/source/lgbk.c
    lua-gbk/source/lpinyin.c
    )
  set_property(
      SOURCE ${GBK_SRC}
      APPEND
      PROPERTY COMPILE_DEFINITIONS
      LUA_LIB 
    )
    list(APPEND THIRDPART_INC lua-gbk/include)
    set (THIRDPART_SRC ${THIRDPART_SRC} ${GBK_SRC})
#Gbk

#JSON
set (RAPIDJSON_SRC lua-rapidjson/source/rapidjson.cpp)
set_property(
	SOURCE ${RAPIDJSON_SRC}
	APPEND
	PROPERTY COMPILE_DEFINITIONS
	LUA_LIB
)
list(APPEND THIRDPART_INC  lua-rapidjson/include)
set (THIRDPART_SRC ${THIRDPART_SRC} ${RAPIDJSON_SRC})
#JSON

#lpeg
set (LPEG_SRC
    lpeg/lpcap.c
    lpeg/lpcode.c
    lpeg/lpprint.c
    lpeg/lptree.c
    lpeg/lpvm.c
)
set_property(
    SOURCE ${LPEG_SRC}
    APPEND
    PROPERTY COMPILE_DEFINITIONS
    LUA_LIB
)
list(APPEND THIRDPART_INC  lpeg)
set (THIRDPART_SRC ${THIRDPART_SRC} ${LPEG_SRC})
#lpeg

set(LUA_SRC_PATH lua-5.3.5/src)
set(LUAJIT_SRC_PATH luajit-2.1.0b3/src)

if (USING_LUAJIT)
	
    if (APPLE AND NOT IOS AND NOT ANDROID)
	    include_directories(
		  ${CMAKE_SOURCE_DIR}
		  lua-5.1.5/src
		  lua-5.1.5/etc
		  ${CMAKE_CURRENT_BINARY_DIR}
		  ${THIRDPART_INC}
	    )
	    set ( LUA_CORE lua-5.1.5/src/lapi.c lua-5.1.5/src/lcode.c lua-5.1.5/src/ldebug.c lua-5.1.5/src/ldo.c lua-5.1.5/src/ldump.c lua-5.1.5/src/lfunc.c 
          lua-5.1.5/src/lgc.c lua-5.1.5/src/llex.c lua-5.1.5/src/lmem.c lua-5.1.5/src/lobject.c lua-5.1.5/src/lopcodes.c lua-5.1.5/src/lparser.c lua-5.1.5/src/lstate.c 
          lua-5.1.5/src/lstring.c lua-5.1.5/src/ltable.c lua-5.1.5/src/ltm.c lua-5.1.5/src/lundump.c lua-5.1.5/src/lvm.c lua-5.1.5/src/lzio.c lua-5.1.5/src/lauxlib.c 
          lua-5.1.5/src/lbaselib.c lua-5.1.5/src/ldblib.c lua-5.1.5/src/liolib.c lua-5.1.5/src/lmathlib.c lua-5.1.5/src/loslib.c lua-5.1.5/src/ltablib.c 
          lua-5.1.5/src/lstrlib.c lua-5.1.5/src/linit.c lua-5.1.5/src/loadlib.c)
	else ()
	    include_directories(
		  ${CMAKE_SOURCE_DIR}
		  ${LUAJIT_SRC_PATH}
		  ${CMAKE_CURRENT_BINARY_DIR}
		  ${THIRDPART_INC}
	    )

	    set ( LUA_CORE )
	    set_property( SOURCE xlua.c APPEND PROPERTY COMPILE_DEFINITIONS USING_LUAJIT )
    endif ()
	set ( LUA_LIB )
else ()
	set ( LUA_IDSIZE 120 CACHE NUMBER "gives the maximum size for the description of the source." )

	configure_file ( ${LUA_SRC_PATH}/luaconf.h.in ${CMAKE_CURRENT_BINARY_DIR}/luaconf.h )

	include_directories(
	    ${CMAKE_SOURCE_DIR}
		${LUA_SRC_PATH}
		${CMAKE_CURRENT_BINARY_DIR}
		${THIRDPART_INC}
	)

	set ( LUA_CORE ${LUA_SRC_PATH}/lapi.c ${LUA_SRC_PATH}/lcode.c ${LUA_SRC_PATH}/lctype.c ${LUA_SRC_PATH}/ldebug.c ${LUA_SRC_PATH}/ldo.c ${LUA_SRC_PATH}/ldump.c
	  ${LUA_SRC_PATH}/lfunc.c ${LUA_SRC_PATH}/lgc.c ${LUA_SRC_PATH}/llex.c ${LUA_SRC_PATH}/lmem.c ${LUA_SRC_PATH}/lobject.c ${LUA_SRC_PATH}/lopcodes.c ${LUA_SRC_PATH}/lparser.c
	  ${LUA_SRC_PATH}/lstate.c ${LUA_SRC_PATH}/lstring.c ${LUA_SRC_PATH}/ltable.c ${LUA_SRC_PATH}/ltm.c ${LUA_SRC_PATH}/lundump.c ${LUA_SRC_PATH}/lvm.c ${LUA_SRC_PATH}/lzio.c )
	set ( LUA_LIB ${LUA_SRC_PATH}/lauxlib.c ${LUA_SRC_PATH}/lbaselib.c ${LUA_SRC_PATH}/lbitlib.c ${LUA_SRC_PATH}/lcorolib.c ${LUA_SRC_PATH}/ldblib.c
	  ${LUA_SRC_PATH}/liolib.c ${LUA_SRC_PATH}/lmathlib.c ${LUA_SRC_PATH}/loslib.c ${LUA_SRC_PATH}/lstrlib.c ${LUA_SRC_PATH}/ltablib.c ${LUA_SRC_PATH}/linit.c
	  ${LUA_SRC_PATH}/lutf8lib.c ${LUA_SRC_PATH}/loadlib.c )
endif ()

set ( LUA_SOCKET
    luasocket/auxiliar.c
    luasocket/buffer.c
    luasocket/except.c
    luasocket/inet.c
    luasocket/io.c
    luasocket/luasocket.c
    luasocket/mime.c
    luasocket/options.c
    luasocket/select.c
    luasocket/tcp.c
    luasocket/timeout.c
    luasocket/udp.c 
)

if ( WIN32 AND NOT CYGWIN )
    list (APPEND LUA_SOCKET luasocket/wsocket.c)
else ( )
    list (APPEND LUA_SOCKET luasocket/usocket.c)
endif ( )

set ( XLUA_CORE
    i64lib.c
    perflib.c
    xlua.c
)

if (NOT USING_LUAJIT)
    list (APPEND XLUA_CORE memory_leak_checker.c)
endif ( )

macro(source_group_by_dir proj_dir source_files)
    if(MSVC OR APPLE)
        get_filename_component(sgbd_cur_dir ${proj_dir} ABSOLUTE)
        foreach(sgbd_file ${${source_files}})
			get_filename_component(sgbd_abs_file ${sgbd_file} ABSOLUTE)
            file(RELATIVE_PATH sgbd_fpath ${sgbd_cur_dir} ${sgbd_abs_file})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    endif(MSVC OR APPLE)
endmacro(source_group_by_dir)

source_group_by_dir(${CMAKE_CURRENT_SOURCE_DIR} LUA_SOCKET)
source_group_by_dir(${CMAKE_CURRENT_SOURCE_DIR} LUA_CORE)
source_group_by_dir(${CMAKE_CURRENT_SOURCE_DIR} LUA_LIB)
source_group_by_dir(${CMAKE_CURRENT_SOURCE_DIR} XLUA_CORE)

if (APPLE)
    if (IOS)
        set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)")
        add_library(xlua STATIC
           ${LUA_CORE}
           ${LUA_LIB}
           ${LUA_SOCKET}
           ${XLUA_CORE}
           ${THIRDPART_SRC}
        )
		set_xcode_property (xlua IPHONEOS_DEPLOYMENT_TARGET "7.0" "all")
    else ()
        set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_64_BIT)")
        add_library(xlua MODULE
            ${LUA_CORE}
            ${LUA_LIB}
            ${LUA_SOCKET}
            ${XLUA_CORE}
            ${THIRDPART_SRC}
        )
        set_target_properties ( xlua PROPERTIES BUNDLE TRUE )
        #set_target_properties ( xlua PROPERTIES FRAMEWORK TRUE )
        #set_target_properties ( xlua PROPERTIES MACOSX_RPATH TRUE )
    endif ()
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Switch")
    add_library(xlua STATIC
        ${LUA_CORE}
        ${LUA_LIB}
        ${XLUA_CORE}
        ${THIRDPART_SRC}
    )
    target_compile_options(xlua PRIVATE -m64 -mcpu=cortex-a57+fp+simd+crypto+crc -fno-common -fno-short-enums -ffunction-sections -fdata-sections -fPIC -fms-extensions)
else ( )
    add_library(xlua SHARED
        ${LUA_CORE}
        ${LUA_LIB}
        ${LUA_SOCKET}
        ${XLUA_CORE}
        ${THIRDPART_SRC}
    )
endif ( )

if ( WIN32 AND NOT CYGWIN )
    target_compile_definitions (xlua PRIVATE LUA_BUILD_AS_DLL)
endif ( )

if (GC64 AND USING_LUAJIT)
    target_compile_definitions (xlua PRIVATE LUAJIT_ENABLE_GC64)
endif ()

if (LUAC_COMPATIBLE_FORMAT AND NOT USING_LUAJIT)
    target_compile_definitions (xlua PRIVATE LUAC_COMPATIBLE_FORMAT)
endif ()

set_property(
	SOURCE ${LUA_SOCKET}
	APPEND
	PROPERTY COMPILE_DEFINITIONS
	LUA_COMPAT_APIINTCASTS LUA_LIB _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS
)

if(UINT_ESPECIALLY)
    ADD_DEFINITIONS(-DUINT_ESPECIALLY)
endif()
	
if ( WIN32 AND NOT CYGWIN )
    if (USING_LUAJIT)
        target_link_libraries(xlua
            ${CMAKE_SOURCE_DIR}/${LUAJIT_SRC_PATH}/lua51.lib
            ws2_32
            ${THIRDPART_LIB} 
        )
    else ()
        target_link_libraries(xlua
            ws2_32
            ${THIRDPART_LIB} 
        )
    endif()
else ( )
    if (USING_LUAJIT AND NOT APPLE)
		target_link_libraries(xlua
			${CMAKE_SOURCE_DIR}/${LUAJIT_SRC_PATH}/libluajit.a
			m
            ${THIRDPART_LIB} 
		)
    else ()
        target_link_libraries(xlua
            m
            ${THIRDPART_LIB} 
        )
    endif()
endif ( )
