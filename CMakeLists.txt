cmake_minimum_required(VERSION 3.10)
project(SaiGo)

set(NAME saigo)

# Properties
set(C_STANDARD 17)

# Configure project paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

include_directories(./src/include)

# Find dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

set(RELEASE_SOURCE_FILES
	${SOURCE_DIR}/main.c
	${SOURCE_DIR}/stone_logic.c
)

add_executable(${NAME} ${RELEASE_SOURCE_FILES})
target_compile_options(${NAME} PRIVATE -Wall -Werror)
target_link_libraries(${NAME} PRIVATE PkgConfig::GTK4)
