
# === CMake lists for the Abacus - (12/08/2015) ===

cmake_minimum_required(VERSION 2.8)
project(Abacus)


# === Build path ===

set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR} CACHE PATH "Build directory" FORCE)
set(PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(PROJECT_SOURCES_DIR "${PROJECT_SOURCE_DIR}/sources")
set(PROJECT_TEST_DIR "${PROJECT_SOURCE_DIR}/test")

option(BUILD_UI_APP "Include build of UI application for the calculator (required wxWidgets)" ON)


# === Global files ===

file(GLOB HeadersAll "${PROJECT_INCLUDE_DIR}/Abacus/*.*")
file(GLOB SourcesAll "${PROJECT_SOURCES_DIR}/*.*")
file(GLOB SourcesPrecPkgAll "${PROJECT_SOURCES_DIR}/precpkg/*.*")
file(GLOB SourcesUIAll "${PROJECT_SOURCES_DIR}/ui/*.*")

set(FilesAllLib ${HeadersAll} ${SourcesAll} ${SourcesPrecPkgAll})
set(FilesAllApp ${SourcesUIAll})


# === Include directories ===

include_directories("${PROJECT_INCLUDE_DIR}")


# === Binaries ===

add_library(abacuslib STATIC ${FilesAllLib})
set_target_properties(abacuslib PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")

add_executable(test1 "${PROJECT_TEST_DIR}/test1.cpp")
set_target_properties(test1 PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")
target_link_libraries(test1 abacuslib)

if(BUILD_UI_APP)
	if(WIN32)
		add_executable(abacus WIN32 ${FilesAllApp} "${PROJECT_SOURCES_DIR}/ui/Resources.rc")
	else(WIN32)
		add_executable(abacus ${FilesAllApp})
	endif(WIN32)
	
	set_target_properties(abacus PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")
	target_link_libraries(abacus abacuslib)
	
	# wxWidgets Libs
	find_package(wxWidgets REQUIRED adv base core stc)
	if(wxWidgets_FOUND)
		message("Found wxWidgets Library")
		include_directories(${wxWidgets_INCLUDE_DIRS})
		target_link_libraries(abacus ${wxWidgets_LIBRARIES})
	else(wxWidgets_FOUND)
		message(SEND_ERROR "Missing wxWidgets Library")
	endif(wxWidgets_FOUND)
endif(BUILD_UI_APP)


