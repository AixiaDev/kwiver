###
# Finds the python binaries, libs, include, and site-packages paths
#
# The purpose of this file is to export variables that will be used in
# kwiver/CMake/utils/kwiver-utils-python.cmake and
# kwiver/sprokit/conf/sprokit-macro-python.cmake (the latter will eventually be
# consolidated into the former)
#
# User options defined in this file:
#
#    KWIVER_PYTHON_VERSION
#      The major python version to target (either 2 or 3)
#
#
# Calls find_packages to on python interpreter/libraries which defines:
#
#    PYTHON_EXECUTABLE
#    PYTHON_INCLUDE_DIR
#    PYTHON_LIBRARY
#    PYTHON_LIBRARY_DEBUG
#
# Exported variables used by python utility functions are:
#
#    PYTHON_VERSION
#      the major/minor python version
#
#    PYTHON_ABI_FLAGS
#      Python abstract binary interface flags (used internally for defining
#      subsequent variables, but settable by the user as an advanced setting)
#
#    python_site_packages
#      Location where python packages are installed relative to your python
#      install directory. For example:
#        Windows system install: Lib\site-packages
#        Debian system install: lib/python2.7/dist-packages
#        Debian virtualenv install: lib/python3.5/site-packages
#
#    python_sitename
#      The basename of the python_site_packages directory. This is either
#      site-packages (in most cases) or dist-packages (if your python was
#      configured by a debian package manager). If you are using a python
#      virtualenv (you should be) then this will be site-packages
#
#    kwiver_python_subdir
#      basename of the python lib folder (that contains site-packages).
#      Depends on the python major/minor version and the ABI flags
#      (e.g. python2.7, python3.5m)
#
#    kwiver_python_output_path
#      The location in the build tree to copy/symlink python modules Depends on
#      the value of `kwiver_python_subdir`.
#      (e.g. build/lib/python2.7, build/lib/python3.5m)
#
#    sprokit_python_output_path
#      Similar to `kwiver_python_output_path`. Used by sprokit to define extra
#      python output paths. This may be removed in the future.
#      (e.g. build/lib)
#
#    boost_python_library
#      Set to either python or python3. Needed for find_package(Boost)


set(KWIVER_PYTHON_VERSION "2" CACHE STRING "Python version to use: 3 or 2")
set_property(CACHE KWIVER_PYTHON_VERSION PROPERTY STRINGS "3" "2")



###
# Logic to determine if we have changed python versions
# When this happens, we need to re-find the bin, include, and libs
if (NOT __prev_kwiver_pyversion STREQUAL KWIVER_PYTHON_VERSION)
  # but dont clobber initial settings in the instance they are specified via
  # commandline (e.g  cmake -DPYTHON_EXECUTABLE=/my/special/python)
  if (__prev_kwiver_pyversion)
    message(STATUS "The Python version changed; refinding the interpreter")
    unset(PYTHON_EXECUTABLE CACHE)
    unset(PYTHON_INCLUDE_DIR CACHE)
    unset(PYTHON_LIBRARY CACHE)
    unset(PYTHON_LIBRARY_DEBUG CACHE)
    unset(PYTHON_ABIFLAGS CACHE)
  endif()
endif()



###
# Python interpreter and libraries
#
if (KWIVER_PYTHON_VERSION STREQUAL "3")
  # note, 3.4 is a minimum version
  find_package(PythonInterp 3.4 REQUIRED)
  find_package(PythonLibs 3.4 REQUIRED)
else()
  find_package(PythonInterp 2.7 REQUIRED)
  find_package(PythonLibs 2.7 REQUIRED)
endif()
include_directories(SYSTEM ${PYTHON_INCLUDE_DIR})


###
# Private helper function that runs a python command and populates an outvar
# with the result of stdout
function(_pycmd outvar cmd)
  execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -c "${cmd}"
    RESULT_VARIABLE _exitcode
    OUTPUT_VARIABLE _output)
  if(NOT ${_exitcode} EQUAL 0)
    message(ERROR "Failed when running python code: \"\"\"
${cmd}\"\"\"")
    message(FATAL_ERROR "Python command failed with error code: ${_exitcode}")
  endif()
  # Remove supurflous newlines (artifacts of print)
  string(STRIP "${_output}" _output)
  set(${outvar} "${_output}" PARENT_SCOPE)
endfunction()


###
# Python site-packages
#
# Get canonical directory for python site packages (relative to install
# location).  It varys from system to system.
#
_pycmd(python_site_packages "\
from distutils import sysconfig
print(sysconfig.get_python_lib(prefix=''))
")
message(STATUS "python_site_packages = ${python_site_packages}")
# Current usage determines most of the path in alternate ways.
# All we need to supply is the '*-packages' directory name.
# Customers could be converted to accept a larger part of the path from this function.
get_filename_component(python_sitename ${python_site_packages} NAME)


###
# Python major/minor version
#
# Use the executable to find the major/minor version.
# If you want to change this, then change the executable.
#
_pycmd(PYTHON_VERSION "\
import sys
print(sys.version[0:3])
")


###
# Python ABI Flags
#
# See PEP 3149 - ABI (application binary interface) version tagged .so files
# https://www.python.org/dev/peps/pep-3149/
if (KWIVER_PYTHON_VERSION STREQUAL "3")
  # In python 3, we can determine what the ABI flags are
  _pycmd(_python_abi_flags "\
from distutils import sysconfig
print(sysconfig.get_config_var('ABIFLAGS'))
")
else()
  # Not sure if ABI flags are easilly found (or are even used in python2)
  set(_python_abi_flags, "")
endif()
set(PYTHON_ABIFLAGS "${_python_abi_flags}"
  CACHE STRING "The ABI flags for the version of Python being used")
mark_as_advanced(PYTHON_ABIFLAGS)


###
# Boost-Python linker definitions
#
if (KWIVER_PYTHON_VERSION STREQUAL "3")
  set(boost_python_library python3)
else()
  set(boost_python_library python)
endif ()
# This is to avoid Boost.Python's headers to have __declspec(dllimport) in
# the headers which confuses Visual Studio's linker.
cmake_dependent_option(SPROKIT_HACK_LINK_BOOST_PYTHON_STATIC "Link Boost.Python statically" ON
  WIN32 OFF)
mark_as_advanced(SPROKIT_HACK_LINK_BOOST_PYTHON_STATIC)
if (SPROKIT_HACK_LINK_BOOST_PYTHON_STATIC)
  add_definitions(-DBOOST_PYTHON_STATIC_LIB)
endif ()


###
# Python package build locations
#
# defines paths used to determine where the kwiver/sprokit/vital python
# packages will be generated in the build tree. (TODO: python modules should
# use a setup.py file to install themselves to the right location)
#
set(kwiver_python_subdir "python${PYTHON_VERSION}${PYTHON_ABIFLAGS}")
set(kwiver_python_output_path "${KWIVER_BINARY_DIR}/lib/${kwiver_python_subdir}")

# Currently needs to be separate because sprokit may have CONFIGURATIONS that
# are placed between lib and `kwiver_python_subdir`
set(sprokit_python_output_path "${KWIVER_BINARY_DIR}/lib")


###
# Mark the previous version so we can determine if the user changes the python
# versions
set(__prev_kwiver_pyversion "${KWIVER_PYTHON_VERSION}" CACHE INTERNAL
  "allows us to determine if the user changes python version")


###
# Status string for debugging
set(PYTHON_CONFIG_STATUS "
PYTHON_CONFIG_STATUS

  * KWIVER_PYTHON_VERSION = \"${KWIVER_PYTHON_VERSION}\"

  * PYTHON_EXECUTABLE = \"${PYTHON_EXECUTABLE}\"
  * PYTHON_INCLUDE_DIR = \"${PYTHON_INCLUDE_DIR}\"
  * PYTHON_LIBRARY = \"${PYTHON_LIBRARY}\"
  * PYTHON_LIBRARY_DEBUG = \"${PYTHON_LIBRARY_DEBUG}\"

  * PYTHON_ABIFLAGS = \"${PYTHON_ABIFLAGS}\"
  * PYTHON_VERSION = \"${PYTHON_VERSION}\"

  * python_site_packages = \"${python_site_packages}\"
  * python_sitename = \"${python_sitename}\"

  * kwiver_python_subdir = \"${kwiver_python_subdir}\"
  * kwiver_python_output_path = \"${kwiver_python_output_path}\"
  * sprokit_python_output_path = \"${sprokit_python_output_path}\"
")
#message(FATAL_ERROR ${PYTHON_CONFIG_STATUS})
