# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/parth_kakirde/esp/esp-idf/components/bootloader/subproject"
  "/home/parth_kakirde/Projects/DTI-PROJECT/R/build/bootloader"
  "/home/parth_kakirde/Projects/DTI-PROJECT/R/build/bootloader-prefix"
  "/home/parth_kakirde/Projects/DTI-PROJECT/R/build/bootloader-prefix/tmp"
  "/home/parth_kakirde/Projects/DTI-PROJECT/R/build/bootloader-prefix/src/bootloader-stamp"
  "/home/parth_kakirde/Projects/DTI-PROJECT/R/build/bootloader-prefix/src"
  "/home/parth_kakirde/Projects/DTI-PROJECT/R/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/parth_kakirde/Projects/DTI-PROJECT/R/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/parth_kakirde/Projects/DTI-PROJECT/R/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
