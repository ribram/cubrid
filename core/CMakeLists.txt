#
# Copyright (C) 2016 Search Solution Corporation. All rights reserved.
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#
include (GenerateExportHeader)

set(CORE_SOURCE_FILES
  ${CORE_DIR}/cubrid_getopt_long.c
)
SET_SOURCE_FILES_PROPERTIES(
  ${CORE_SOURCE_FILES}
  PROPERTIES LANGUAGE CXX
)
set(CORE_SOURCE_HEADERS
 ${CORE_DIR}/cubrid_getopt.h 
)

add_library(cubridcore SHARED
  ${CORE_SOURCE_FILES}
  ${CORE_SOURCE_HEADERS}
)
generate_export_header(cubridcore
  #EXPORT_FILE_NAME ${CMAKE_EXPORTS_DIR}/cubridcore_export.h
)
target_include_directories(cubridcore PUBLIC
  ${CORE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

function(_cubridcore_link depender_target prv_or_pub)
  target_link_libraries(${depender_target} ${prv_or_pub} cubridcore)
  target_include_directories(${depender_target} ${CUBRIDCORE_INCLUDE_DIR})
endfunction(_cubridcore_link)
