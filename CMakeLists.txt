cmake_minimum_required(VERSION 3.19)

project(wyuEngine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

# Build Options.
option(IMGUI "Build imgui" ON)

include(CMakeDependentOption)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

if(IMGUI)
	add_compile_definitions(IMGUI_ENABLE=1)
else()
	add_compile_definitions(IMGUI_ENABLE=0)
endif()

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}/bin")
set(BINARY_ROOT_DIR "${CMAKE_INSTALL_PREFIX}/")

add_subdirectory(engine)
#add_subdirectory(game)
add_subdirectory(3Dtest)

#add_subdirectory(CHAIN&CHANGE)
add_subdirectory(test)
#add_subdirectory(touchNumbers)
#add_subdirectory(touchNumbersClient)