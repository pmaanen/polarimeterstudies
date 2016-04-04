#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(polarimeterStudies)


if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9")
    message(FATAL_ERROR "Insufficient gcc version")
  endif()
endif()

set(CMAKE_MACOSX_RPATH 1)
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
#set(DISABLED_WARNINGS "-Wno-shadow -Wno-c++11-extensions")

#Get rid of warning messages introduced by boost
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DISABLED_WARNINGS} -std=c++11")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DISABLED_WARNINGS} -g -std=c++11 -D MYDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${DISABLED_WARNINGS} -std=c++11")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${DISABLED_WARNINGS} -std=c++11")
#----------------------------------------------------------------------------
# set everything needed for the root dictonary and create the
# dictionary
set(Analysis_HEADERS ${PROJECT_SOURCE_DIR}/include/hit.hh)
set(Analysis_LINKDEF ${PROJECT_SOURCE_DIR}/include/AnalysisLinkDef.hh )
set(Analysis_DICT ${CMAKE_BINARY_DIR}/AnalysisDict)

ROOT_GENERATE_DICTIONARY(${Analysis_DICT} ${Analysis_HEADERS} LINKDEF ${Analysis_LINKDEF})
# add the dictionary to the list of source files
#SET(sources ${sources} ${Analysis_DICT})
#SET(Analysis_SRCS ${Analysis_DICT})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(polarimeterstudies main.cc ${sources} ${headers})
target_link_libraries(polarimeterstudies ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES} Analysis)

add_executable(dceventgenerator DCElasticFileWriter.cc ${sources} ${headers})
target_link_libraries(dceventgenerator ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES} Analysis)

add_library(Analysis SHARED ${Analysis_DICT})
target_link_libraries(Analysis ${ROOT_LIBRARIES})

add_dependencies(polarimeterstudies Analysis)
#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build gammagamma. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
foreach(_script ${scripts})
  configure_file(
    ${PROJECT_SOURCE_DIR}/scripts/${_script}	
    ${PROJECT_BINARY_DIR}/scripts/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS polarimeterstudies DESTINATION bin)
