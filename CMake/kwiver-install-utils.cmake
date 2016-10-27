# Installation logic for kwiver CMake utilities
#
# Variables that modify function:
#
#   kwiver_cmake_install_dir
#     - Directory to install files to
#
set(utils_dir "${CMAKE_CURRENT_LIST_DIR}")

install(
  FILES "${utils_dir}/kwiver-utils.cmake"
        "${utils_dir}/kwiver-flags.cmake"
        "${utils_dir}/kwiver-utils.cmake"
        "${utils_dir}/kwiver-configcheck.cmake"
        "${utils_dir}/kwiver-flags-gnu.cmake"
        "${utils_dir}/kwiver-flags-msvc.cmake"
        "${utils_dir}/kwiver-flags-clang.cmake"
        "${utils_dir}/kwiver-configcheck.cmake"
  DESTINATION "${kwiver_cmake_install_dir}"
  )

install(
  DIRECTORY "${utils_dir}/utils"
            "${utils_dir}/tools"
            "${utils_dir}/configcheck"
            "${utils_dir}/templates"
  DESTINATION "${kwiver_cmake_install_dir}"
  )
