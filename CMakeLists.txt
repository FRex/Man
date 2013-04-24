cmake_minimum_required(VERSION 2.8)

project(Man)

# Set options
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
set(SFML_STATIC_LIBS FALSE CACHE BOOL "Choose whether SFML is linked statically or not.")
set(MAN_STATIC_STD_LIBS FALSE CACHE BOOL "Use statically linked standard/runtime libraries? This option must match the one used for SFML.")
    
# Make sure that the runtime library gets link statically
if(MAN_STATIC_STD_LIBS)
	if(NOT SFML_STATIC_LIBS)
		message("\n-> If you check MAN_STATIC_STD_LIBS, you also need to check SFML_STATIC_LIBRARIES.")
		message("-> It would lead to multiple runtime environments which result in undefined behavior.\n")
	elseif(WIN32 AND MSVC)
		# Change all MSVC compiler flags to /MT
		foreach(flag CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
			if(${flag} MATCHES "/MD")
			string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
			endif()
		endforeach()
	elseif(CMAKE_COMPILER_IS_GNUCXX)
		# Note: Doesn't work for TDM compiler, since it's compiling the runtime libs statically by default
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
	endif()
endif()

# Make sure that FindSFML.cmake searches for the static libraries
if(SFML_STATIC_LIBS)
	set(SFML_STATIC_LIBRARIES TRUE)
endif()

# Find SFML
find_package(SFML 2 COMPONENTS graphics window system)

# Output an error if SFML wasn't found
if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
else()
	set(SFML_ROOT "" CACHE PATH "SFML top-level directory")
	message("\n-> SFML directory not found. Set SFML_ROOT to SFML's top-level path (containing \"include\" and \"lib\" directories).")
	message("-> Make sure the SFML libraries with the same configuration (Release/Debug, Static/Dynamic) exist.\n")
endif()

# Add the source files
set(SRC_DIR "src")
set(INC_DIR "src")

set(
	SOURCES
	${INC_DIR}/EEToString.h
	${INC_DIR}/ErrState.h
	${INC_DIR}/GameState.h
	${INC_DIR}/MenuState.h
	${INC_DIR}/PacEngine.h
	${INC_DIR}/PacEntity.h
	${INC_DIR}/PauseState.h
	${INC_DIR}/QuitState.h
	${INC_DIR}/ScoreState.h
	${INC_DIR}/SpecialState.h
	${INC_DIR}/State.h
	${INC_DIR}/StateManager.h
	${SRC_DIR}/GameState.cpp
	${SRC_DIR}/mainPacman.cpp
	${SRC_DIR}/MenuState.cpp
	${SRC_DIR}/PacEngine.cpp
	${SRC_DIR}/ScoreState.cpp
	${SRC_DIR}/StateManager.cpp
)

# Tell CMake to build a executable 
add_executable(Man ${SOURCES})

# Link SFML
target_link_libraries(Man ${SFML_LIBRARIES})

# Install executable
install(TARGETS Man
		RUNTIME DESTINATION .)

# Install game data
install(DIRECTORY bin/
		DESTINATION .)

# On Windows install required audio libraries
#if(WIN32)
#	install(FILES ${SFML_ROOT}/bin/openal32.dll ${SFML_ROOT}/bin/libsndfile-1.dll
#			DESTINATION .)
#endif()