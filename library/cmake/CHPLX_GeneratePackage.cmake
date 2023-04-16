# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

include(CMakePackageConfigHelpers)

# generate cmake packages
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/Chplx/ChplxConfigVersion.cmake"
  VERSION ${CHPLX_LIBRARY_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(
  TARGETS library
  NAMESPACE Chplx::
  FILE "${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/Chplx/ChplxConfig.cmake"
)

# Export ChplxTargets in the install directory
#install(
#  EXPORT library
#  NAMESPACE Chplx::
#  FILE ChplxTargets.cmake
#  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Chplx
#)

