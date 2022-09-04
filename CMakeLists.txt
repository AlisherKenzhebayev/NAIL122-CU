set(NCPROJECT_NAME "AStarPong")
set(NCPROJECT_EXE_NAME "astarpong")
set(NCPROJECT_VENDOR "Alisher Kenzhebayev")

set(NCPROJECT_SOURCES
	pong.h
	pong.cpp
	aStar.h
	aStar.cpp
	gameState.h
	gameState.cpp
)

set(NCPROJECT_ANDROID_ASSETS
	data/DroidSans32_256.fnt
	data/out.wav
	data/tick.wav
	android/DroidSans32_256.webp
	android/sticks_256.webp
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

function(callback_end)
	if(NOT EMSCRIPTEN AND NOT ANDROID)
		include(ncpong_lua)
	endif()
endfunction()

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
