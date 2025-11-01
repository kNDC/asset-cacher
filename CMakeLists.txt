cmake_minimum_required(VERSION 3.23)

project(AssetCacher CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AC_VERSION_MAJOR "1" CACHE STRING "Major version")
set(AC_VERSION_MINOR "0" CACHE STRING "Minor version")
set(AC_VERSION_PATCH "0" CACHE STRING "Maintenance version")
set(AC_VERSION "${AC_VERSION_MAJOR}.${AC_VERSION_MINOR}.${AC_VERSION_PATCH}")

message("AssetCacher version: ${AC_VERSION}")
message("Host system: ${CMAKE_HOST_SYSTEM}")
message("C++ compiler Id: ${CMAKE_CXX_COMPILER_ID}")
message("C++ compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
message("CMake version: ${CMAKE_VERSION}")

message(STATUS "Flags for Debug: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Flags for Release: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "Flags for MinSizeRel: ${CMAKE_CXX_FLAGS_MINSIZEREL}")

# To maximise portability, non-Debug versions are linked statically to 
# the standard library.
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU") # MinGW
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -static")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -s -static")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -s -static")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang") # Clang
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -static")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -s -static")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -s -static")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC") # MSVC
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /LTCG /MT")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /GL /LTCG  /MT")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /GL /LTCG  /MT")
else()
	message(WARNING "Unsupported compiler detected!")
endif()

set(SRC_DIR "./src")
set(FILES_CACHER "${SRC_DIR}/asset_cacher.h" "${SRC_DIR}/asset_cacher.cpp")
set(FILES_CONFIG "${SRC_DIR}/json.h" "${SRC_DIR}/config.h")
set(FILES_CONSOLE "${SRC_DIR}/console_progress_bar.h")
set(FILES_MAIN "${SRC_DIR}/main.cpp")
set(FILES_MISC "${SRC_DIR}/augmented_fstream.h" "${SRC_DIR}/binary_io.h" "${SRC_DIR}/io_traits.h" 
	"${SRC_DIR}/container_utils.h")
set(FILES_W3D "${SRC_DIR}/w3d.h" "${SRC_DIR}/w3d.cpp")

source_group("Main" FILES FILES_MAIN)
source_group("Cacher" FILES FILES_CACHER)
source_group("Config" FILES FILES_CONFIG)
source_group("Console" FILES FILES_CONSOLE)
source_group("Misc" FILES FILES_MISC)
source_group("W3D" FILES FILES_W3D)

# Laying out the output folder structure
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
message("Compilation flags: " "${CMAKE_CXX_FLAGS}")

add_executable("AssetCacher" ${FILES_MAIN} ${FILES_CACHER} ${FILES_CONFIG} 
	${FILES_CONSOLE} ${FILES_MISC} ${FILES_W3D} ${SYSTEM_LIBS})