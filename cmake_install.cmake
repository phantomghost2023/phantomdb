# Install script for directory: D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/PhantomDB")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/Debug/phantomdb.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/Release/phantomdb.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/MinSizeRel/phantomdb.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/RelWithDebInfo/phantomdb.exe")
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/core/cmake_install.cmake")
  include("D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/storage/cmake_install.cmake")
  include("D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/query/cmake_install.cmake")
  include("D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/transaction/cmake_install.cmake")
  include("D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/api/cmake_install.cmake")
  include("D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/tools/cmake_install.cmake")
  include("D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/testing/cmake_install.cmake")
  include("D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/audit/cmake_install.cmake")
  include("D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/security/cmake_install.cmake")
  include("D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/plugin/cmake_install.cmake")
  include("D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/import_export/cmake_install.cmake")
  include("D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/distributed/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
