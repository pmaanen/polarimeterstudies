#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(polarimeterStudies)

set(CMAKE_CXX_STANDARD 14)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9")
    message(FATAL_ERROR "Insufficient gcc version")
  endif()
endif()

set(CMAKE_MACOSX_RPATH 1)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/")

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Setup Boost include directories and compile definitions
#
find_package(Boost 1.36 COMPONENTS program_options REQUIRED)
#----------------------------------------------------------------------------
# Setup Root include directories and compile definitions
#
find_package(ROOT REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include  
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIR}
                    ${Boost_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(GLOB scripts RELATIVE ${PROJECT_SOURCE_DIR}/scripts/ ${PROJECT_SOURCE_DIR}/scripts/*)

#----------------------------------------------------------------------------
# set everything needed for the root dictonary and create the
# dictionary
set(Analysis_HEADERS ${PROJECT_SOURCE_DIR}/include/JediClasses.hh)
set(Analysis_LINKDEF ${PROJECT_SOURCE_DIR}/include/AnalysisLinkDef.hh )
ROOT_GENERATE_DICTIONARY(libAnalysis ${Analysis_HEADERS} LINKDEF ${Analysis_LINKDEF})
add_library(Analysis SHARED libAnalysis.cxx)
target_link_libraries(Analysis ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(polarimeterstudies main.cc ${sources} ${headers})
target_link_libraries(polarimeterstudies ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES} Analysis)
#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build polarimeterstudies. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
file(COPY ${PROJECT_SOURCE_DIR}/scripts DESTINATION ${PROJECT_BINARY_DIR} )

#----------------------------------------------------------------------------
# Install the project into {CMAKE_INSTALL_PREFIX}
#
install(TARGETS polarimeterstudies Analysis 
		RUNTIME DESTINATION .
		LIBRARY DESTINATION . )

install (FILES "${PROJECT_BINARY_DIR}/libAnalysis_rdict.pcm"        
         DESTINATION .)

install(DIRECTORY scripts/ DESTINATION scripts/ MESSAGE_NEVER) 
