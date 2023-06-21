cmake_minimum_required(VERSION 3.10)
project(Knots\ and\ Crosses VERSION 1.0)

# Set output dir
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)

# Set compiler flags
set(CMAKE_CXX_STANDARD_LIBRARIES "")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Qn -Wall -Wpedantic -Wextra -O3 -flto -fuse-linker-plugin -static -fno-stack-protector -fno-rtti -fno-exceptions -fdiagnostics-color=always -std=gnu++23")

add_compile_definitions(
	NDEBUG
)


# Add the executable target
add_executable(Knots_and_Crosses
	"${CMAKE_SOURCE_DIR}/src/Knc.cpp"
	"${CMAKE_SOURCE_DIR}/src/Settings.cpp"
	"${CMAKE_SOURCE_DIR}/src/ShaderMgmt.cpp"
	"${CMAKE_SOURCE_DIR}/glad/src/glad.c"
)

# Boxer by Aaron Jacobs https://github.com/aaronmjacobs/Boxer
# Sources for Boxer
if(APPLE)
	target_sources(Knots_and_Crosses PRIVATE "${CMAKE_SOURCE_DIR}/boxer/src/boxer_mac.mm")
elseif(WIN32)
	target_sources(Knots_and_Crosses PRIVATE "${CMAKE_SOURCE_DIR}/boxer/src/boxer_win.cpp")
elseif(LINUX)
	target_sources(Knots_and_Crosses PRIVATE "${CMAKE_SOURCE_DIR}/boxer/src/boxer_linux.cpp")
endif()

# Include dir for Boxer
target_include_directories(Knots_and_Crosses PRIVATE "${CMAKE_SOURCE_DIR}/boxer/include")

# Platform-specific dependencies for Boxer
if (APPLE)
   find_library(COCOA_LIBRARY Cocoa)
   target_link_libraries(Knots_and_Crosses PRIVATE ${COCOA_LIBRARY})
elseif (LINUX)
   find_package(PkgConfig REQUIRED)
   pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
   target_link_libraries(Knots_and_Crosses PRIVATE ${GTK3_LIBRARIES})
   target_include_directories(Knots_and_Crosses PRIVATE ${GTK3_INCLUDE_DIRS})
endif ()

# Include dirs
target_include_directories(Knots_and_Crosses PRIVATE
	"${CMAKE_SOURCE_DIR}/include"
	"${CMAKE_SOURCE_DIR}/glad/include"
	"${CMAKE_SOURCE_DIR}/glfw/out/include"
)

# Set target properties
target_link_options(Knots_and_Crosses PRIVATE 
	"-Qn"
	"-flto"
	"-fuse-linker-plugin"
)

# Link libraries
target_link_libraries(Knots_and_Crosses PRIVATE
	"${CMAKE_SOURCE_DIR}/glfw/out/lib/libglfw3.a"
)

if(WIN32)
	target_link_libraries(Knots_and_Crosses PRIVATE
		"gdi32"
		"opengl32"
	)
endif()

# Copies rsrc folder
add_custom_target(copy_rsrc_folder ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/src/rsrc
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rsrc
	DEPENDS Knots_and_Crosses
)