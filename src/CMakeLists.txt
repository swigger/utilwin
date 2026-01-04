cmake_minimum_required (VERSION 3.11.0)
project (utilwin)

# message(STATUS "xx src is ${CMAKE_CURRENT_SOURCE_DIR}")

# set(CMAKE_DEBUG_POSTFIX "d")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include"
 "${CMAKE_CURRENT_SOURCE_DIR}/__make"
 "${CMAKE_CURRENT_SOURCE_DIR}/../inc_win"
 )

file(GLOB HFILES "${CMAKE_CURRENT_SOURCE_DIR}" */*.h)
file(GLOB HFILES_e "${CMAKE_CURRENT_SOURCE_DIR}" */__*.h)
list(REMOVE_ITEM HFILES ${HFILES_e})

file(GLOB CSRC "${CMAKE_CURRENT_SOURCE_DIR}" */*.c)
add_library(utilwin STATIC ${CSRC})


install (TARGETS utilwin 
	ARCHIVE DESTINATION lib
)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	install ( FILES ${HFILES} DESTINATION include)
endif()

macro(configure_link_flags)
	set(MSVC_C_CXX_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_MINSIZEREL
		CMAKE_C_FLAGS_RELEASE
		CMAKE_C_FLAGS_RELWITHDEBINFO
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_MINSIZEREL
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_RELWITHDEBINFO
		)
	# message(STATUS "VCPKG: dynamic link")
	foreach(flag ${MSVC_C_CXX_FLAGS})
		if(${flag} MATCHES "/MT")
			string(REGEX REPLACE "/MT" "/MD" ${flag} "${${flag}}")
		endif()
	endforeach()
	set(VCPKG_CRT_LINKAGE "dynamic")
	set(VCPKG_LIBRARY_LINKAGE "dynamic")
endmacro()

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	configure_link_flags()
endif()
