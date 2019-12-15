# Copyright © 2018-2019 Pascal JEAN, All rights reserved.
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

# doc CMakeLists.txt

cmake_minimum_required(VERSION 2.8.11)

set (RADIOHEAD_WITH_DOXYGEN_DOC 0 CACHE BOOL "Enable build of the documentation")

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

if("${CMAKE_PROJECT_NAME}" STREQUAL "Project")
  set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
endif()

if (RADIOHEAD_WITH_DOXYGEN_DOC)

  find_package(Doxygen REQUIRED)
  set(DOXYGEN_FILE ${CMAKE_BINARY_DIR}/Doxyfile)

  set(DOXYGEN_INPUT_LIST 
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
  
  if (RADIOHEAD_WITH_ENCRYPTION)
    list(APPEND DOXYGEN_INPUT_LIST ${RADIOHEAD_INC_DIR}/RHEncryptedDriver.h)
  endif()

  unset(DOXYGEN_INPUT)
  foreach(item ${DOXYGEN_INPUT_LIST})
    string(APPEND DOXYGEN_INPUT " ${item} ")
  endforeach()

  #message("DOXYGEN_INPUT=${DOXYGEN_INPUT}")

  configure_file(Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)

  add_custom_target(html
    COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_FILE} > /dev/null
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
    DEPENDS DoxygenLayout.xml Doxyfile.in ${DOXYGEN_INPUT_LIST}
    )
  
  add_custom_target(doc)
  add_dependencies(doc html)
  
  #message("RADIOHEAD_INSTALL_DOC_DIR=${RADIOHEAD_INSTALL_DOC_DIR}")
  #message("INSTALL_DATA_DIR=${INSTALL_DATA_DIR}")

  # API documentation
  install(DIRECTORY ${CMAKE_BINARY_DIR}/html/
          DESTINATION ${RADIOHEAD_INSTALL_DOC_DIR}/api-manual COMPONENT doc)
  
  # manpages
  install(DIRECTORY ${CMAKE_BINARY_DIR}/man/
          DESTINATION ${INSTALL_DATA_DIR}/man 
          COMPONENT doc
          FILES_MATCHING PATTERN "*.3")

  # Examples
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/examples/
          DESTINATION ${RADIOHEAD_INSTALL_EXAMPLES_DIR}
          COMPONENT doc
          FILES_MATCHING PATTERN "*.c" PATTERN "*.h" PATTERN "*.cpp" PATTERN "*.mk" PATTERN "*.txt" PATTERN "*.project"  PATTERN "*.workspace" PATTERN "CMakeLists.txt" PATTERN "README*"
          PATTERN ".clang/" EXCLUDE
          PATTERN "Debug/" EXCLUDE
          PATTERN "Release/" EXCLUDE
          PATTERN ".codelite/" EXCLUDE
          PATTERN "cmake-build*/" EXCLUDE)

endif(RADIOHEAD_WITH_DOXYGEN_DOC)
