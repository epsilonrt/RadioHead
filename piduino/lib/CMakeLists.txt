# This file is part of the radiohead Project.
#
# The radiohead Project is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The radiohead Project is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the radiohead Project.  If not, see <http://www.gnu.org/licenses/>.

# radiohead CMakeLists.txt
cmake_minimum_required(VERSION 2.8.11)

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()
add_definitions (-DRH_PLATFORM=RH_PLATFORM_PIDUINO) 

WriteGitVersionFile(${CMAKE_CURRENT_BINARY_DIR}/version.h)
  
set (src_radiohead
  ${RADIOHEAD_SRC_DIR}/RHGenericDriver.cpp
  ${RADIOHEAD_SRC_DIR}/RHGenericSPI.cpp
  ${RADIOHEAD_SRC_DIR}/RHHardwareSPI.cpp
  ${RADIOHEAD_SRC_DIR}/RHSPIDriver.cpp
  ${RADIOHEAD_SRC_DIR}/RH_RF22.cpp
  ${RADIOHEAD_SRC_DIR}/RH_RF24.cpp
  ${RADIOHEAD_SRC_DIR}/RH_RF69.cpp
  ${RADIOHEAD_SRC_DIR}/RH_RF95.cpp
  ${RADIOHEAD_SRC_DIR}/RHNRFSPIDriver.cpp
  ${RADIOHEAD_SRC_DIR}/RH_CC110.cpp
  ${RADIOHEAD_SRC_DIR}/RH_MRF89.cpp
  ${RADIOHEAD_SRC_DIR}/RH_NRF24.cpp
  ${RADIOHEAD_SRC_DIR}/RH_NRF905.cpp
  ${RADIOHEAD_SRC_DIR}/RHCRC.cpp
  ${RADIOHEAD_SRC_DIR}/RH_Serial.cpp
  ${RADIOHEAD_SRC_DIR}/RH_E32.cpp
  ${RADIOHEAD_SRC_DIR}/RHDatagram.cpp
  ${RADIOHEAD_SRC_DIR}/RHReliableDatagram.cpp
  ${RADIOHEAD_SRC_DIR}/RHRouter.cpp
  ${RADIOHEAD_SRC_DIR}/RHMesh.cpp
#  ${RADIOHEAD_SRC_DIR}/RH_TCP.cpp
  ${RADIOHEAD_SRC_DIR}/RHutil/RHGpioLed.cpp
)

set (hdr_radiohead 
  ${RADIOHEAD_INC_DIR}/RadioHead.h
  ${RADIOHEAD_INC_DIR}/RHGenericDriver.h
  ${RADIOHEAD_INC_DIR}/RHGenericSPI.h
  ${RADIOHEAD_INC_DIR}/RHHardwareSPI.h
  ${RADIOHEAD_INC_DIR}/RHSPIDriver.h
  ${RADIOHEAD_INC_DIR}/RH_RF22.h
  ${RADIOHEAD_INC_DIR}/RH_RF24.h
  ${RADIOHEAD_INC_DIR}/RH_RF69.h
  ${RADIOHEAD_INC_DIR}/RH_RF95.h
  ${RADIOHEAD_INC_DIR}/RHNRFSPIDriver.h
  ${RADIOHEAD_INC_DIR}/RH_CC110.h
  ${RADIOHEAD_INC_DIR}/RH_MRF89.h
  ${RADIOHEAD_INC_DIR}/RH_NRF24.h
  ${RADIOHEAD_INC_DIR}/RH_NRF905.h
  ${RADIOHEAD_INC_DIR}/RHCRC.h
  ${RADIOHEAD_INC_DIR}/RH_Serial.h
  ${RADIOHEAD_INC_DIR}/RH_E32.h
  ${RADIOHEAD_INC_DIR}/RHDatagram.h
  ${RADIOHEAD_INC_DIR}/RHReliableDatagram.h
  ${RADIOHEAD_INC_DIR}/RHRouter.h
  ${RADIOHEAD_INC_DIR}/RHMesh.h
  ${RADIOHEAD_INC_DIR}/RHTcpProtocol.h
  ${RADIOHEAD_INC_DIR}/RH_TCP.h
)

set (hdr_radiohead_util 
  ${RADIOHEAD_INC_DIR}/RHutil/RHLed.h
  ${RADIOHEAD_INC_DIR}/RHutil/RHGpioLed.h
)

file(GLOB src_crypto  ${CRYPTO_SRC_DIR}/*.cpp)
file(GLOB hdr_crypto  ${CRYPTO_INC_DIR}/*.h)
file(GLOB hdr_crypto_utility  ${CRYPTO_INC_DIR}/utility/*.h)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

set(hdr_public ${hdr_radiohead} ${hdr_crypto} ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set (libsrc ${src_radiohead} ${src_crypto})

include_directories(BEFORE
  ${PIDUINO_INCLUDE_DIRS} 
  ${RADIOHEAD_INC_DIR} 
  ${CMAKE_CURRENT_BINARY_DIR}
  )

add_library(radiohead-shared SHARED ${libsrc})
target_compile_definitions(radiohead-shared PUBLIC RADIOHEAD_WITH_CONFIG_H)

set_target_properties(radiohead-shared PROPERTIES 
  OUTPUT_NAME radiohead 
  CLEAN_DIRECT_OUTPUT 1 
  VERSION ${RADIOHEAD_VERSION} 
  SOVERSION ${RADIOHEAD_VERSION_MAJOR}
  PUBLIC_HEADER "${hdr_public}"
  )

if(RADIOHEAD_WITH_STATIC)
  add_library(radiohead-static STATIC ${libsrc})
  target_compile_definitions(radiohead-static PUBLIC RADIOHEAD_WITH_CONFIG_H)
  set_target_properties(radiohead-static PROPERTIES
    OUTPUT_NAME radiohead 
    CLEAN_DIRECT_OUTPUT 1 
    VERSION ${RADIOHEAD_VERSION} 
  )
endif(RADIOHEAD_WITH_STATIC)

# CMake Package ----------------------------------------------------------------
# The interesting stuff goes here
# ===============================

# Add all targets to the build-tree export set
if (RADIOHEAD_WITH_STATIC)
  set (exported_targets radiohead-shared radiohead-static)
else(RADIOHEAD_WITH_STATIC)
  set (exported_targets radiohead-shared)
endif(RADIOHEAD_WITH_STATIC)

export(TARGETS ${exported_targets}
  FILE "${PROJECT_BINARY_DIR}/radiohead.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE radiohead)

# Create the radiohead-config.cmake and radiohead-config-version files
file(RELATIVE_PATH REL_LIB_DIR "${RADIOHEAD_INSTALL_CMAKE_DIR}" 
  "${INSTALL_LIB_DIR}")
file(RELATIVE_PATH REL_INCLUDE_DIR "${RADIOHEAD_INSTALL_CMAKE_DIR}"
  "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${RADIOHEAD_INC_DIR}" "${PROJECT_BINARY_DIR}")
set(CONF_LIB_DIRS "${PROJECT_BINARY_DIR}/lib/radiohead")
configure_file(radiohead-config.cmake.in
  "${PROJECT_BINARY_DIR}/radiohead-config.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${RADIOHEAD_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "\${RADIOHEAD_CMAKE_DIR}/${REL_LIB_DIR}")
configure_file(radiohead-config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/radiohead-config.cmake" @ONLY)

# ... for both
configure_file(radiohead-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/radiohead-config-version.cmake" @ONLY)

# Install the radiohead-config.cmake and radiohead-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/radiohead-config.cmake"
  "${PROJECT_BINARY_DIR}/radiohead-config-version.cmake"
  DESTINATION "${RADIOHEAD_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT radiohead DESTINATION
  "${RADIOHEAD_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the radiohead licence files
install(FILES
  "${RADIOHEAD_TOP_DIR}/LICENSE"
  DESTINATION "${RADIOHEAD_INSTALL_DATA_DIR}" COMPONENT lib)

install(FILES
  "${RADIOHEAD_TOP_DIR}/README.md"
  DESTINATION "${RADIOHEAD_INSTALL_DOC_DIR}" COMPONENT lib)

install (TARGETS radiohead-shared
  # IMPORTANT: Add the radiohead library to the "export-set"
  EXPORT radiohead
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}" COMPONENT dev
  )

set_target_properties(radiohead-shared PROPERTIES 
  PUBLIC_HEADER "${hdr_radiohead_util}")
install (TARGETS radiohead-shared
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/RHutil" COMPONENT dev)

set_target_properties(radiohead-shared PROPERTIES 
  PUBLIC_HEADER "${hdr_crypto_utility}")
install (TARGETS radiohead-shared
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/utility" COMPONENT dev)

if(RADIOHEAD_WITH_STATIC)
  install (TARGETS radiohead-static 
    EXPORT radiohead
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  )
endif(RADIOHEAD_WITH_STATIC)


## pkg-config ------------------------------------------------------------------
foreach(item ${RADIOHEAD_CFLAGS_OTHER})
  string(APPEND RADIOHEAD_CFLAGS_PKGCONFIG "${item} ")
endforeach(item RADIOHEAD_CFLAGS_OTHER)

foreach(item ${RADIOHEAD_LDFLAGS_OTHER})
  string(APPEND RADIOHEAD_LDFLAGS_PKGCONFIG "${item} ")
endforeach(item RADIOHEAD_LDFLAGS_OTHER)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/radiohead.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/radiohead.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/radiohead.pc 
  DESTINATION "${INSTALL_LIB_DIR}/pkgconfig"
  COMPONENT dev)
