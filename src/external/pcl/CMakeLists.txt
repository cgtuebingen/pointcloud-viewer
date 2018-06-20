find_package(Boost 1.58 REQUIRED)

add_library(pcl STATIC
  include/pcl/pcl_macros.h
  include/pcl/io/boost.h
  include/pcl/io/ply/byte_order.h
  include/pcl/io/ply/io_operators.h
  include/pcl/io/ply/ply.h
  include/pcl/io/ply/ply_parser.h
  src/pcl/io/ply/ply_parser.cpp
)

target_include_directories(pcl PUBLIC include ${BOOST_INCLUDE_DIRS})
