set(NCPROJECT_NAME "ncGo")
set(NCPROJECT_EXE_NAME "ncgo")
set(NCPROJECT_VENDOR "Alisher Kenzhebayev")
set(NCPROJECT_DESCRIPTION "MCTS Go implementation for NAIL122")

set(NCPROJECT_SOURCES
	main.h
	main.cpp
	SpritePool.h
	SpritePool.cpp
	mcts.h
	mcts.cpp
	GridPool.h
	GridPool.cpp
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Don't edit beyond this line
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
	return()
endif()

cmake_minimum_required(VERSION 3.13)
project(${NCPROJECT_NAME})

find_path(NCPROJECT_DIR cmake/project_main.cmake
	PATHS ${nCine_DIR} $ENV{NCINE_DIR}
	PATH_SUFFIXES project ../project ../../../project ../../../share/ncine/project
	NO_CMAKE_FIND_ROOT_PATH # For Emscripten
	DOC "Path to the nCine template project scripts")

if(IS_DIRECTORY ${NCPROJECT_DIR})
	include(${NCPROJECT_DIR}/cmake/project_main.cmake)
else()
	message(FATAL_ERROR "Directory \"${NCPROJECT_DIR}\" does not exist")
endif()