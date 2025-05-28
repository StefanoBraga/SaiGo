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

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
	# set input and output files
	set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
	set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)

	# request to configure the file
	configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

	add_custom_target(docs
			COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Generating documentation with Doxygen"
			VERBATIM )
endif()

# Find dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

set(RELEASE_SOURCE_FILES
		${SOURCE_DIR}/main.c
		${SOURCE_DIR}/stone_logic.c
		${SOURCE_DIR}/groups.c
		${SOURCE_DIR}/board_coord.c
)

add_executable(${NAME} ${RELEASE_SOURCE_FILES})
target_compile_options(${NAME} PRIVATE -Wall -Werror)
target_link_libraries(${NAME} PRIVATE PkgConfig::GTK4)
