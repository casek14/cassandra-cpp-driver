cmake_minimum_required(VERSION 2.6.4)

#-----------
# Includes
#-----------
include(FindPackageHandleStandardArgs)

#-----------
# Policies
#-----------

macro(CassPolicies)
  # TODO: Figure out Mac OS X rpath
  if(POLICY CMP0042)
    cmake_policy(SET CMP0042 OLD)
  endif()

  # Force OLD style of project versioning variables
  if(POLICY CMP0048)
    cmake_policy(SET CMP0048 OLD)
  endif()
endmacro()

#------------------------
# CassCheckPlatform
#
# Check to ensure the platform is valid for the driver
#
#------------------------
macro(CassCheckPlatform)
  # Ensure Windows platform is supported
  if(WIN32)
    if(CMAKE_SYSTEM_VERSION GREATER 5.2 OR
       CMAKE_SYSTEM_VERSION EQUAL 5.2)
      add_definitions(-D_WIN32_WINNT=0x502)
    else()
      string(REPLACE "." "" WINDOWS_VERSION ${CMAKE_SYSTEM_VERSION})
      string(REGEX REPLACE "([0-9])" "0\\1" WINDOWS_VERSION ${WINDOWS_VERSION})
      message(FATAL_ERROR "Unable to build driver: Unsupported Windows platform 0x${WINDOWS_VERSION}")
    endif()
  endif()
endmacro()

#---------------
# Dependencies
#---------------

#------------------------
# CassUseLibuv
#
# Add includes and  libraries required for using libuv.
#
# Input: CASS_INCLUDES and CASS_LIBS
# Output: CASS_INCLUDES and CASS_LIBS
#------------------------
macro(CassUseLibuv)
  # Setup the paths and hints for libuv
  set(_LIBUV_ROOT_PATHS "${PROJECT_SOURCE_DIR}/lib/libuv/")
  set(_LIBUV_ROOT_HINTS ${LIBUV_ROOT_DIR} $ENV{LIBUV_ROOT_DIR})
  if(NOT WIN32)
    set(_LIBUV_ROOT_PATHS "${_LIBUV_ROOT_PATHS}" "/usr/" "/usr/local/")
  endif()
  set(_LIBUV_ROOT_HINTS_AND_PATHS HINTS
    HINTS ${_LIBUV_ROOT_HINTS}
    PATHS ${_LIBUV_ROOT_PATHS})

  # Ensure libuv was found
  find_path(LIBUV_INCLUDE_DIR
    NAMES uv.h
    HINTS ${_LIBUV_INCLUDEDIR} ${_LIBUV_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES include)
  find_library(LIBUV_LIBRARY
    NAMES uv libuv
    HINTS ${_LIBUV_LIBDIR} ${_LIBUV_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES lib)
  find_package_handle_standard_args(Libuv "Could NOT find libuv, try to set the path to the libuv root folder in the system variable LIBUV_ROOT_DIR"
    LIBUV_LIBRARY
    LIBUV_INCLUDE_DIR)

  # Assign libuv include and libraries
  set(CASS_INCLUDES ${CASS_INCLUDES} ${LIBUV_INCLUDE_DIR})
  set(CASS_LIBS ${CASS_LIBS} ${LIBUV_LIBRARY})
endmacro()

#------------------------
# Optional Dependencies
#------------------------

# Minimum supported version of Boost
set(CASS_MINIMUM_BOOST_VERSION 1.55.0)

#------------------------
# CassUseBoost
#
# Add includes, libraries, define flags required for using Boost.
#
# Input: CASS_USE_STATIC_LIBS, CASS_USE_BOOST_ATOMIC, CASS_INCLUDES, CASS_LIBS
# Output: CASS_INCLUDES and CASS_LIBS
#------------------------
macro(CassUseBoost)

  # Allow for boost directory to be specified on the command line
  set(ENV{BOOST_ROOT} "${PROJECT_SOURCE_DIR}/lib/boost/")
  if(BOOST_ROOT_DIR)
    set(ENV{BOOST_ROOT} ${BOOST_ROOT_DIR})
  endif()

  # Ensure Boost auto linking is disabled (defaults to auto linking on Windows)
  if(WIN32)
    add_definitions(-DBOOST_ALL_NO_LIB)
  endif()

  # Determine if shared or static boost libraries should be used
  if(CASS_USE_STATIC_LIBS)
    set(Boost_USE_STATIC_LIBS ON)
  else()
    set(Boost_USE_STATIC_LIBS OFF)
    add_definitions(-DBOOST_ALL_DYN_LINK)
  endif()
  set(Boost_USE_STATIC_RUNTIME OFF)
  set(Boost_USE_MULTITHREADED ON)
  add_definitions(-DBOOST_THREAD_USES_MOVE)

  # Ensure the driver components exist (optional)
  if(CASS_USE_BOOST_ATOMIC)
    find_package(Boost ${CASS_MINIMUM_BOOST_VERSION})
    if(NOT Boost_INCLUDE_DIR)
      message(FATAL_ERROR "Boost headers required to build driver because of -DCASS_USE_BOOST_ATOMIC=On")
    endif()

    # Assign Boost include and libraries
    set(CASS_INCLUDES ${CASS_INCLUDES} ${Boost_INCLUDE_DIRS})
    set(CASS_LIBS ${CASS_LIBS} ${Boost_LIBRARIES})
  endif()

  # Determine if Boost components are available for test executables
  if(CASS_BUILD_UNIT_TESTS OR CASS_BUILD_INTEGRATION_TESTS)
    find_package(Boost ${CASS_MINIMUM_BOOST_VERSION} COMPONENTS chrono date_time filesystem log log_setup system regex thread unit_test_framework)
    if(NOT Boost_FOUND)
      message(FATAL_ERROR "Boost [chrono, date_time, filesystem, log, log_setup, system, regex, thread, and unit_test_framework] are required to build tests")
    endif()

    # Assign Boost include and libraries
    set(CASS_INCLUDES ${CASS_INCLUDES} ${Boost_INCLUDE_DIRS})
    set(CASS_LIBS ${CASS_LIBS} ${Boost_LIBRARIES})
  endif()

  # Determine if additional Boost definitions are required for driver/executables
  set(CASS_FOUND_BOOST_VERSION "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
  if((CASS_FOUND_BOOST_VERSION VERSION_GREATER "1.56.0") OR (CASS_FOUND_BOOST_VERSION VERSION_EQUAL "1.56.0"))
    if(NOT WIN32)
      # Handle explicit initialization warning in atomic/details/casts
      add_definitions(-Wno-missing-field-initializers)
    endif()
  else()
    # Handle redefinition warning of BOOST_NO_CXX11_RVALUE_REFERENCES
    add_definitions(-DBOOST_NO_CXX11_RVALUE_REFERENCES)
  endif()
endmacro()

#------------------------
# CassUseSparseHash
#
# Add includes required for using SparseHash.
#
# Input: CASS_INCLUDES
# Output: CASS_INCLUDES
#------------------------
macro(CassUseSparseHash)
  # Setup the paths and hints for sparsehash
  set(_SPARSEHASH_ROOT_PATHS "${PROJECT_SOURCE_DIR}/lib/sparsehash/")
  set(_SPARSEHASH_ROOT_HINTS ${SPARSEHASH_ROOT_DIR} $ENV{SPARSEHASH_ROOT_DIR})
  if(NOT WIN32)
    set(_SPARSEHASH_ROOT_PATHS ${_SPARSEHASH_ROOT_PATHS} "/usr/" "/usr/local/")
  endif()
  set(_SPARSEHASH_ROOT_HINTS_AND_PATHS
    HINTS ${_SPARSEHASH_ROOT_HINTS}
    PATHS ${_SPARSEHASH_ROOT_PATHS})

  # Ensure sparsehash headers were found
  find_path(SPARSEHASH_INCLUDE_DIR
    NAMES google/dense_hash_map
    HINTS ${_SPARSEHASH_INCLUDE_DIR} ${_SPARSEHASH_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES include)
  find_package_handle_standard_args(SparseJash "Could NOT find sparsehash, try to set the path to the sparsehash root folder in the system variable SPARSEHASH_ROOT_DIR"
    SPARSEHASH_INCLUDE_DIR)

  set(CASS_INCLUDES ${CASS_INCLUDES} ${SPARSEHASH_INCLUDE_DIR})

  if (SPARSEHASH_INCLUDE_DIR)
    add_definitions("-DCASS_USE_SPARSEHASH")
  endif()
endmacro()

#------------------------
# CassUseOpenSSL
#
# Add includes and libraries required for using OpenSSL.
#
# Input: CASS_INCLUDES and CASS_LIBS
# Output: CASS_INCLUDES and CASS_LIBS
#------------------------
macro(CassUseOpenSSL)
  # Setup the paths and hints for OpenSSL
  set(_OPENSSL_ROOT_PATHS "${PROJECT_SOURCE_DIR}/lib/openssl/")
  set(_OPENSSL_ROOT_HINTS ${OPENSSL_ROOT_DIR} $ENV{OPENSSL_ROOT_DIR})
  set(_OPENSSL_ROOT_HINTS_AND_PATHS
    HINTS ${_OPENSSL_ROOT_HINTS}
  PATHS ${_OPENSSL_ROOT_PATHS})

  # Discover OpenSSL and assign OpenSSL include and libraries
  find_package(OpenSSL REQUIRED)

  set(CASS_INCLUDES ${CASS_INCLUDES} ${OPENSSL_INCLUDE_DIR})
  set(CASS_LIBS ${CASS_LIBS} ${OPENSSL_LIBRARIES})
endmacro()

#------------------------
# CassUseTcmalloc
#
# Add libraries required for using tcmalloc.
#
# Input: CASS_LIBS
# Output: CASS_LIBS
#------------------------
macro(CassUseTcmalloc)
  # Setup the paths and hints for gperftools
  set(_GPERFTOOLS_ROOT_PATHS "${PROJECT_SOURCE_DIR}/lib/gperftools/")
  set(_GPERFTOOLS_ROOT_HINTS ${GPERFTOOLS_ROOT_DIR} $ENV{GPERFTOOLS_ROOT_DIR})
  if(NOT WIN32)
    set(_GPERFTOOLS_ROOT_PATHS ${_GPERFTOOLS_ROOT_PATHS} "/usr/" "/usr/local/")
  endif()
  set(_GPERFTOOLS_ROOT_HINTS_AND_PATHS
    HINTS ${_GPERFTOOLS_ROOT_HINTS}
    PATHS ${_GPERFTOOLS_ROOT_PATHS})

  # Ensure gperftools (tcmalloc) was found
  find_library(GPERFLIBRARY_LIBRARY
    NAMES tcmalloc
    HINTS ${_GPERFTOOLS_LIBDIR} ${_GPERFTOOLS_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES lib)
  find_package_handle_standard_args(Gperftools "Could NOT find gperftools, try to set the path to the gperftools root folder in the system variable GPERFTOOLS_ROOT_DIR"
    GPERFLIBRARY_LIBRARY)

  # Assign gperftools (tcmalloc) library
  set(CASS_LIBS ${CASS_LIBS} ${GPERFLIBRARY_LIBRARY})
endmacro()

#------------------------
# CassUseZlib
#
# Add includes and libraries required for using tcmalloc.
#
# Input: CASS_INCLUDES and CASS_LIBS
# Output: CASS_INCLUDES and CASS_LIBS
#------------------------
macro(CassUseZlib)
  # Setup the root directory for zlib
  set(ZLIB_ROOT "${PROJECT_SOURCE_DIR}/lib/zlib/")
  set(ZLIB_ROOT ${ZLIB_ROOT} ${ZLIB_ROOT_DIR} $ENV{ZLIB_ROOT_DIR})

  # Ensure zlib was found (assign zlib include/libraries or present warning)
  find_package(ZLIB)
  if(ZLIB_FOUND)
    # Determine if the static library needs to be used for Windows
    if(WIN32 AND CASS_USE_STATIC_LIBS)
      string(REPLACE "zlib.lib" "zlibstatic.lib" ZLIB_LIBRARIES "${ZLIB_LIBRARIES}")
    endif()

    # Assign zlib properties
    set(CASS_INCLUDES ${CASS_INCLUDES} ${ZLIB_INCLUDE_DIRS})
    set(CASS_LIBS ${CASS_LIBS} ${ZLIB_LIBRARIES})
  else()
    message(WARNING "Could not find zlib, try to set the path to zlib root folder in the system variable ZLIB_ROOT_DIR")
    message(WARNING "zlib libraries will not be linked into build")
  endif()
endmacro()

#-------------------
# Compiler Flags
#-------------------

#------------------------
# CassSetCompilerFlags
#
# Detect compiler version and set compiler flags and defines
#
# Input: CASS_USE_STD_ATOMIC, CASS_USE_BOOST_ATOMIC, CASS_MULTICORE_COMPILATION
# and CASS_USE_STATIC_LIBS
# Output: CASS_USE_STD_ATOMIC, CASS_DRIVER_CXX_FLAGS, CASS_TEST_CXX_FLAGS and
# CASS_EXAMPLE_C_FLAGS
#------------------------
macro(CassSetCompilerFlags)
  # Force OLD style of implicitly dereferencing variables
  if(POLICY CMP0054)
    cmake_policy(SET CMP0054 OLD)
  endif()

  # Determine if all GNU extensions should be enabled
  if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE")
  endif()

  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
     "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # CMAKE_CXX_COMPILER variables do not exist in 2.6.4 (min version)
    # Parse the -dumpversion argument into the variable not already set
    if("${CMAKE_CXX_COMPILER_VERSION}" STREQUAL "")
      execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION)
    endif()
  endif()

  # Determine if std::atomic can be used for GCC, Clang, or MSVC
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
      # Version determined from: https://gcc.gnu.org/wiki/Atomic/GCCMM
      if(CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "4.7" OR
         CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.7")
        set(CASS_USE_STD_ATOMIC ON)
      endif()
    endif()
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # Version determined from: http://clang.llvm.org/cxx_status.html
    # 3.2 includes the full C++11 memory model, but 3.1 had atomic
    # support.
    if(CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "3.1" OR
       CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "3.1")
     set(CASS_USE_STD_ATOMIC ON)
    endif()
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Version determined from https://msdn.microsoft.com/en-us/library/hh874894
    # VS2012+/VS 11.0+/WindowsSDK v8.0+
    if(MSVC_VERSION GREATER 1700 OR
       MSVC_VERSION EQUAL 1700)
      set(CASS_USE_STD_ATOMIC ON)
    endif()
  endif()

  # Enable specific cass::Atomic implementation
  if(CASS_USE_BOOST_ATOMIC)
    message(STATUS "Using boost::atomic implementation for atomic operations")
    add_definitions(-DCASS_USE_BOOST_ATOMIC)
  elseif(CASS_USE_STD_ATOMIC)
    message(STATUS "Using std::atomic implementation for atomic operations")
    add_definitions(-DCASS_USE_STD_ATOMIC)
  endif()

  # Assign compiler specific flags
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Determine if multicore compilation should be enabled
    if(CASS_MULTICORE_COMPILATION)
      # Default multicore compilation with effective processors (see https://msdn.microsoft.com/en-us/library/bb385193.aspx)
      add_definitions("/MP")
    endif()

    # Remove/Replace linker flags in the event they are present
    string(REPLACE "/INCREMENTAL" "/INCREMENTAL:NO" CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
    string(REPLACE "/INCREMENTAL" "/INCREMENTAL:NO" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    # Create specific linker flags
    set(WINDOWS_LINKER_FLAGS "/INCREMENTAL:NO /LTCG /NODEFAULTLIB:LIBCMT.LIB /NODEFAULTLIB:LIBCMTD.LIB")
    if(CASS_USE_STATIC_LIBS)
      set(PROJECT_CXX_LINKER_FLAGS "${WINDOWS_LINKER_FLAGS}")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${WINDOWS_LINKER_FLAGS}")
    endif()

    # On Visual C++ -pedantic flag is not used,
    # -fPIC is not used on Windows platform (all DLLs are
    # relocable), -Wall generates about 30k stupid warnings
    # that can hide useful ones.
    # Create specific warning disable compiler flags
    # TODO(mpenick): Fix these "possible loss of data" warnings
    add_definitions(/wd4244)
    add_definitions(/wd4267)
    add_definitions(/wd4800) # Performance warning due to automatic compiler casting from int to bool

    # Add preprocessor definitions for proper compilation
    add_definitions(-D_WIN32_WINNT=0x0501)      # Required for winsock (pre Windows XP wspiapi.h only)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)  # Remove warnings for not using safe functions (TODO: Fix codebase to be more secure for Visual Studio)
    add_definitions(-DNOMINMAX)                 # Does not define min/max macros

    # Create the project, example, and test flags
    set(CASS_DRIVER_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CASS_DRIVER_CXX_FLAGS} ${WARNING_COMPILER_FLAGS}")
    set(CASS_EXAMPLE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_COMPILER_FLAGS}")
    set(CASS_TEST_CXX_FLAGS "${CASS_DRIVER_CXX_FLAGS} ${WARNING_COMPILER_FLAGS}")

    # Assign additional library requirements for Windows
    set(CASS_LIBS ${CASS_LIBS} iphlpapi psapi wsock32 crypt32 ws2_32 userenv)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # GCC specific compiler options
    # I disabled long-long warning because boost generates about 50 such warnings
    set(WARNING_COMPILER_FLAGS "-Wall -pedantic -Wextra -Wno-long-long -Wno-unused-parameter -Wno-variadic-macros")

    if(CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "4.8" OR
       CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.8")
      set(WARNING_COMPILER_FLAGS "${WARNING_COMPILER_FLAGS} -Wno-unused-local-typedefs")
    endif()

    # OpenSSL is deprecated on later versions of Mac OS X. The long-term solution
    # is to provide a CommonCryto implementation.
    if (APPLE AND CASS_USE_OPENSSL)
      set(CASS_DRIVER_CXX_FLAGS "${CASS_DRIVER_CXX_FLAGS} -Wno-deprecated-declarations")
      set(CASS_TEST_CXX_FLAGS "${CASS_TEST_CXX_FLAGS} -Wno-deprecated-declarations")
    endif()

    # Enable C++11 support to use std::atomic
    if(CASS_USE_STD_ATOMIC)
      set(CASS_DRIVER_CXX_FLAGS "${CASS_DRIVER_CXX_FLAGS} -std=c++11")
      set(CASS_TEST_CXX_FLAGS "${CASS_TEST_CXX_FLAGS} -std=c++11")
    endif()

    set(CASS_DRIVER_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CASS_DRIVER_CXX_FLAGS} ${WARNING_COMPILER_FLAGS} -Werror")
    set(CASS_TEST_CXX_FLAGS "${CASS_TEST_CXX_FLAGS} ${WARNING_COMPILER_FLAGS}")
    set(CASS_EXAMPLE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_COMPILER_FLAGS}")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # Clang/Intel specific compiler options
    # I disabled long-long warning because boost generates about 50 such warnings
    set(WARNING_COMPILER_FLAGS "-Wall -pedantic -Wextra -Wno-long-long -Wno-unused-parameter")
    set(WARNING_COMPILER_FLAGS "${WARNING_COMPILER_FLAGS} -Wno-variadic-macros -Wno-zero-length-array")
    set(WARNING_COMPILER_FLAGS "${WARNING_COMPILER_FLAGS} -Wno-unused-local-typedef -Wno-unknown-warning-option")

    # OpenSSL is deprecated on later versions of Mac OS X. The long-term solution
    # is to provide a CommonCryto implementation.
    if (APPLE AND CASS_USE_OPENSSL)
      set(CASS_DRIVER_CXX_FLAGS "${CASS_DRIVER_CXX_FLAGS} -Wno-deprecated-declarations")
      set(CASS_TEST_CXX_FLAGS "${CASS_TEST_CXX_FLAGS} -Wno-deprecated-declarations")
    endif()

    # Enable C++11 support to use std::atomic
    if(CASS_USE_STD_ATOMIC)
      set(CASS_DRIVER_CXX_FLAGS "${CASS_DRIVER_CXX_FLAGS} -std=c++11")
      set(CASS_TEST_CXX_FLAGS "${CASS_TEST_CXX_FLAGS} -std=c++11")
    endif()

    set(CASS_DRIVER_CXX_FLAGS " ${CMAKE_CXX_FLAGS} ${CASS_DRIVER_CXX_FLAGS} ${WARNING_COMPILER_FLAGS} -Werror")
    set(CASS_TEST_CXX_FLAGS "${CASS_TEST_CXX_FLAGS} ${WARNING_COMPILER_FLAGS}")
    set(CASS_EXAMPLE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 ${WARNING_COMPILER_FLAGS}")
  else()
    message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
  endif()
endmacro()

#-------------------
# Internal Includes and Source Files
#-------------------

#------------------------
# CassAddIncludes
#
# Add internal includes
#
# Input: CASS_INCLUDES
# Output: CASS_INCLUDES
#------------------------
macro(CassAddIncludes)
  set(INCLUDES ${CASS_INCLUDES} ${CASS_SOURCE_DIR}/include)
  set(INCLUDES ${INCLUDES} ${CASS_SOURCE_DIR}/src)
  set(INCLUDES ${INCLUDES} ${CASS_SOURCE_DIR}/src/ssl)
  set(CASS_INCLUDES ${INCLUDES} ${CASS_SOURCE_DIR}/src/third_party/rapidjson)
endmacro()

#------------------------
# CassFindSourceFiles
#
# Gather the header and source files
#
# Input: CASS_SOURCE_DIR, CASS_USE_BOOST_ATOMIC, CASS_USE_STD_ATOMIC and
# CASS_USE_OPENSSL
# Output: CASS_ALL_SOURCE_FILES
#------------------------
macro(CassFindSourceFiles)
  file(GLOB API_HEADER_FILES ${CASS_SOURCE_DIR}/include/*.h)
  file(GLOB INC_FILES ${CASS_SOURCE_DIR}/src/*.hpp)
  file(GLOB SRC_FILES ${CASS_SOURCE_DIR}/src/*.cpp)

  # Determine atomic library to include
  if(CASS_USE_BOOST_ATOMIC)
    set(INC_FILES ${INC_FILES}
      ${CASS_SOURCE_DIR}/src/atomic/atomic_boost.hpp)
  elseif(CASS_USE_STD_ATOMIC)
    set(INC_FILES ${INC_FILES}
      ${CASS_SOURCE_DIR}/src/atomic/atomic_std.hpp)
  else()
    set(INC_FILES ${INC_FILES}
      ${CASS_SOURCE_DIR}/src/atomic/atomic_intrinsics.hpp)
    if(WIN32)
      set(INC_FILES ${INC_FILES}
        ${CASS_SOURCE_DIR}/src/atomic/atomic_intrinsics_msvc.hpp)
    else()
      set(INC_FILES ${INC_FILES}
        ${CASS_SOURCE_DIR}/src/atomic/atomic_intrinsics_gcc.hpp)
    endif()
  endif()

  set(SRC_FILES ${SRC_FILES}
    ${CASS_SOURCE_DIR}/src/third_party/hdr_histogram/hdr_histogram.cpp)

  set(SRC_FILES ${SRC_FILES}
    ${CASS_SOURCE_DIR}/src/third_party/curl/hostcheck.cpp)

  # Determine if OpenSSL should be compiled in (or not)
  if(CASS_USE_OPENSSL)
    set(INC_FILES ${INC_FILES}
      ${CASS_SOURCE_DIR}/src/ssl/ssl_openssl_impl.hpp
      ${CASS_SOURCE_DIR}/src/ssl/ring_buffer_bio.hpp)
    set(SRC_FILES ${SRC_FILES}
      ${CASS_SOURCE_DIR}/src/ssl/ssl_openssl_impl.cpp
      ${CASS_SOURCE_DIR}/src/ssl/ring_buffer_bio.cpp)
    add_definitions(-DCASS_USE_OPENSSL)
  else()
    set(INC_FILES ${INC_FILES}
      ${CASS_SOURCE_DIR}/src/ssl/ssl_no_impl.hpp)
    set(SRC_FILES ${SRC_FILES}
      ${CASS_SOURCE_DIR}/src/ssl/ssl_no_impl.cpp)
  endif()

  set(CASS_ALL_SOURCE_FILES ${SRC_FILES} ${API_HEADER_FILES} ${INC_FILES})

  # Shorten the source file pathing for log messages
  foreach(SRC_FILE ${SRC_FILES})
    string(REPLACE "${CMAKE_SOURCE_DIR}/" "" LOG_FILE_ ${SRC_FILE})
    set_source_files_properties(${SRC_FILE} PROPERTIES COMPILE_FLAGS -DLOG_FILE_=\\\"${LOG_FILE_}\\\")
  endforeach()
endmacro()
