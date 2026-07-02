cmake_minimum_required(VERSION 3.13)

project(GRUTinizer
  VERSION 1.2.0
  DESCRIPTION "A generic unpacker and analysis package for gamma-ray spectroscopy"
  HOMEPAGE_URL "https://github.com/pcbend/GRUTinizer"
  LANGUAGES CXX
)

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})
include(GNUInstallDirs)

if(NOT ROOT_CXX_STANDARD)
  set(ROOT_CXX_STANDARD 11)
endif()

set(CMAKE_CXX_STANDARD ${ROOT_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(GRUTINIZER_BUILD_UTILITIES "Build utility executables from util/*.cxx" ON)
option(GRUTINIZER_BUILD_PLUGINS "Build histogram and filter plugin libraries" ON)
option(GRUTINIZER_ENABLE_HTTP "Link ROOT RHTTP support for live histogram publishing" ON)
option(GRUTINIZER_ENABLE_ROOT_PYTHON "Build/link ROOT Python support and grutinizer-python" OFF)
option(GRUTINIZER_ENABLE_LEGACY_ROOT_GUI "Link legacy ROOT GUI/Proof libraries" OFF)

if(GRUTINIZER_ENABLE_ROOT_PYTHON)
  find_library(GRUTINIZER_ROOT_PYTHON_LIBRARY
    NAMES ROOTTPython PyROOT
    PATHS ${ROOT_LIBRARY_DIR} ${ROOT_LIBRARY_DIRS} ${ROOT_DIR}/lib
  )
endif()

add_library(grutinizer_common INTERFACE)
target_include_directories(grutinizer_common INTERFACE
  ${PROJECT_SOURCE_DIR}/include
  ${ROOT_INCLUDE_DIRS}
)
target_compile_definitions(grutinizer_common INTERFACE
  $<$<PLATFORM_ID:Darwin>:OS_DARWIN>
  $<$<PLATFORM_ID:Darwin>:HAVE_ZLIB>
)
target_compile_options(grutinizer_common INTERFACE
  -Wall
  -Wextra
  -pedantic
  -Wno-unused-parameter
)
target_link_libraries(grutinizer_common INTERFACE
  ${ROOT_LIBRARIES}
  Spectrum
  Minuit
  MathMore
)
if(GRUTINIZER_ENABLE_ROOT_PYTHON AND GRUTINIZER_ROOT_PYTHON_LIBRARY)
  target_link_libraries(grutinizer_common INTERFACE ${GRUTINIZER_ROOT_PYTHON_LIBRARY})
endif()

if(GRUTINIZER_ENABLE_HTTP)
  if(TARGET ROOT::RHTTP)
    target_link_libraries(grutinizer_common INTERFACE ROOT::RHTTP)
  else()
    find_library(GRUTINIZER_RHTTP_LIBRARY RHTTP
      PATHS ${ROOT_LIBRARY_DIR} ${ROOT_LIBRARY_DIRS} ${ROOT_DIR}/lib
    )
    if(GRUTINIZER_RHTTP_LIBRARY)
      target_link_libraries(grutinizer_common INTERFACE ${GRUTINIZER_RHTTP_LIBRARY})
    endif()
  endif()
endif()

if(GRUTINIZER_ENABLE_LEGACY_ROOT_GUI)
  foreach(root_component Gui GuiHtml XMLParser XMLIO Proof)
    if(TARGET ROOT::${root_component})
      target_link_libraries(grutinizer_common INTERFACE ROOT::${root_component})
    endif()
  endforeach()
endif()

if(APPLE)
  if(GRUTINIZER_ENABLE_LEGACY_ROOT_GUI)
    if(EXISTS /opt/X11/include)
      target_include_directories(grutinizer_common INTERFACE /opt/X11/include)
    endif()
    find_library(GRUTINIZER_X11_LIBRARY X11 PATHS /opt/X11/lib)
    find_library(GRUTINIZER_XPM_LIBRARY Xpm PATHS /opt/X11/lib)
    if(GRUTINIZER_X11_LIBRARY)
      target_link_libraries(grutinizer_common INTERFACE ${GRUTINIZER_X11_LIBRARY})
    endif()
    if(GRUTINIZER_XPM_LIBRARY)
      target_link_libraries(grutinizer_common INTERFACE ${GRUTINIZER_XPM_LIBRARY})
    endif()
  endif()
elseif(UNIX)
  target_link_options(grutinizer_common INTERFACE -Wl,--no-as-needed)
endif()

include(cmake/GrutinizerTargets.cmake)

add_custom_command(
  OUTPUT ${PROJECT_SOURCE_DIR}/include/GVersion.h
  COMMAND ${PROJECT_SOURCE_DIR}/util/gen_version.sh
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  DEPENDS
    ${PROJECT_SOURCE_DIR}/.git/HEAD
    ${PROJECT_SOURCE_DIR}/.git/index
    ${PROJECT_SOURCE_DIR}/util/gen_version.sh
  COMMENT "Generating GVersion.h"
)
add_custom_target(grutinizer_version DEPENDS ${PROJECT_SOURCE_DIR}/include/GVersion.h)

add_subdirectory(libraries)

add_executable(grutinizer src/grutinizer.cxx)
add_dependencies(grutinizer grutinizer_version)
target_link_libraries(grutinizer PRIVATE ${GRUTINIZER_LIBRARY_TARGETS})

if(GRUTINIZER_BUILD_UTILITIES)
  set(grutinizer_utility_excludes gadd.cxx)
  if(NOT GRUTINIZER_ENABLE_ROOT_PYTHON OR NOT GRUTINIZER_ROOT_PYTHON_LIBRARY)
    list(APPEND grutinizer_utility_excludes grutinizer-python.cxx)
  endif()
  grutinizer_add_executables_from_directory(
    DIRECTORY ${PROJECT_SOURCE_DIR}/util
    LINK_LIBRARIES ${GRUTINIZER_LIBRARY_TARGETS}
    EXCLUDE ${grutinizer_utility_excludes}
  )
endif()

if(GRUTINIZER_BUILD_PLUGINS)
  grutinizer_add_plugins_from_directory(
    DIRECTORY ${PROJECT_SOURCE_DIR}/histos
    LINK_LIBRARIES ${GRUTINIZER_LIBRARY_TARGETS}
  )
  grutinizer_add_plugins_from_directory(
    DIRECTORY ${PROJECT_SOURCE_DIR}/filters
    LINK_LIBRARIES ${GRUTINIZER_LIBRARY_TARGETS}
  )
endif()

add_custom_target(grutinizer_runtime ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory
          ${CMAKE_BINARY_DIR}/include
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${PROJECT_SOURCE_DIR}/include/GVersion.h
          ${CMAKE_BINARY_DIR}/include/GVersion.h
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${PROJECT_SOURCE_DIR}/.grutrc
          ${CMAKE_BINARY_DIR}/.grutrc
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${PROJECT_SOURCE_DIR}/.grut.mimes
          ${CMAKE_BINARY_DIR}/.grut.mimes
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${PROJECT_SOURCE_DIR}/util/grutinizer-config
          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/grutinizer-config
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${PROJECT_SOURCE_DIR}/icons
          ${CMAKE_BINARY_DIR}/icons
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${PROJECT_SOURCE_DIR}/config
          ${CMAKE_BINARY_DIR}/config
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${PROJECT_SOURCE_DIR}/pygui
          ${CMAKE_BINARY_DIR}/pygui
  DEPENDS ${PROJECT_SOURCE_DIR}/include/GVersion.h
  COMMENT "Copying GRUTinizer runtime files"
)
add_dependencies(grutinizer_runtime grutinizer_version)
add_dependencies(grutinizer grutinizer_runtime)

install(TARGETS grutinizer ${GRUTINIZER_LIBRARY_TARGETS}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
install(DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY icons config pygui DESTINATION .)
install(FILES .grutrc .grut.mimes util/grutinizer-config DESTINATION .)

string(ASCII 27 Esc)
set(GRUT_COLOR_CYAN "${Esc}[36m")
set(GRUT_COLOR_YELLOW "${Esc}[33m")
set(GRUT_COLOR_RESET "${Esc}[0m")

add_custom_command(
  TARGET grutinizer
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "-----------------------------------"
  COMMAND ${CMAKE_COMMAND} -E echo "${GRUT_COLOR_CYAN} GRUTinizer build successfully, ${GRUT_COLOR_YELLOW}woohoo! ${GRUT_COLOR_RESET}"
  COMMAND ${CMAKE_COMMAND} -E echo "-----------------------------------"
)
