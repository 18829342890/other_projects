# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\train_data_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\train_data_autogen.dir\\ParseCache.txt"
  "train_data_autogen"
  )
endif()
