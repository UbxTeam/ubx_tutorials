CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(cblock_tutorial_auto)

set(CMAKE_CXX_FLAGS "-Wall -Werror -fvisibility=hidden")
set(CMAKE_CXX_COMPILER clang++ )
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake") ## temporary resources, for UBX

# Global
set(INSTALL_LIB_TYPES_DIR lib/ubx/types CACHE PATH "Installation directory for types libraries")
set(INSTALL_LIB_BLOCKS_DIR lib/ubx/blocks CACHE PATH "Installation directory for blocks libraries")
set(INSTALL_BIN_APPS_DIR bin CACHE PATH "Installation directory for bin")
set(INSTALL_INCLUDE_DIR include/ubx CACHE PATH "Installation directory for header files (types)")
set(INSTALL_CMAKE_DIR  share/ubx/cmake CACHE PATH  "Installation directory for CMake files") # default ${DEF_INSTALL_CMAKE_DIR}

# Make relative paths absolute
foreach(p LIB_TYPES LIB_BLOCKS BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

##
# Add uninstall target.
##
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
    
add_custom_target(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
)

##
# Generate config package
##
set(CONF_INCLUDE_DIRS "${INSTALL_INCLUDE_DIR}")
set(CONF_CMAKE_DIR "${INSTALL_CMAKE_DIR}")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cblock-tutorial-auto-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/cblock-tutorial-auto-config.cmake" @ONLY
)

set (UBX_ROOT $ENV{UBX_ROOT} )
if (NOT UBX_ROOT)
 message( FATAL_ERROR "UBX_ROOT is not defined. CMake install willl abort." )
endif()

find_package(Ubx REQUIRED)

include_directories(
  ${UBX_INCLUDE_DIR}
)

set(GEN_HEXARR ${UBX_ROOT}/tools/file2carr.lua)

file(GLOB types
  ${CMAKE_CURRENT_SOURCE_DIR}/types/*.h
)


set(outfiles "")
foreach( _type ${types} )
  string(REPLACE ".h" ".h.hexarr" _outfile ${_type})
  add_custom_command(
  OUTPUT ${_outfile}
  COMMAND ${GEN_HEXARR}
  ARGS ${_type}  > ${_outfile}
  )
  list(APPEND outfiles ${_outfile})
endforeach(_type)
add_custom_target( gen_hexarr ALL DEPENDS ${outfiles} )

add_library(cblock_tutorial_auto_types SHARED types/cblock_tutorial_auto_types.c)
set_target_properties(cblock_tutorial_auto_types PROPERTIES PREFIX "") 
set_property(TARGET cblock_tutorial_auto_types PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
target_link_libraries(cblock_tutorial_auto_types ${UBX_LIBRARIES})
add_dependencies(cblock_tutorial_auto_types gen_hexarr)

### Install libraries
set_target_properties(cblock_tutorial_auto_types PROPERTIES PUBLIC_HEADER "${outfiles};${types}" )

install(TARGETS cblock_tutorial_auto_types 
  EXPORT cblock-tutorial-auto-types-targets
  LIBRARY DESTINATION "${INSTALL_LIB_TYPES_DIR}" COMPONENT cblock_tutorial_auto_types
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/types" COMPONENT dev
)

# Install the cblock_tutorial_auto_types-config.cmake 
install(FILES
  "${PROJECT_BINARY_DIR}/cmake/cblock-tutorial-auto-config.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT cblock-tutorial-auto-types-targets DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)
  
# Compile library sendermod
add_library(sendermod SHARED ../cblock_tutorial_auto/src/sender.c
../cblock_tutorial_auto/modules/sendermod_module.c
)
set_target_properties(sendermod PROPERTIES PREFIX "")
target_link_libraries(sendermod ${UBX_LIBRARIES})
add_dependencies(sendermod gen_hexarr)

# Install sendermod
install(TARGETS sendermod DESTINATION ${INSTALL_LIB_BLOCKS_DIR} EXPORT sendermod-block)
set_property(TARGET sendermod PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
install(EXPORT sendermod-block DESTINATION ${INSTALL_CMAKE_DIR})

# Compile library receivermod
add_library(receivermod SHARED ../cblock_tutorial_auto/src/receiver.c
../cblock_tutorial_auto/modules/receivermod_module.c
)
set_target_properties(receivermod PROPERTIES PREFIX "")
target_link_libraries(receivermod ${UBX_LIBRARIES})
add_dependencies(receivermod gen_hexarr)

# Install receivermod
install(TARGETS receivermod DESTINATION ${INSTALL_LIB_BLOCKS_DIR} EXPORT receivermod-block)
set_property(TARGET receivermod PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
install(EXPORT receivermod-block DESTINATION ${INSTALL_CMAKE_DIR})

