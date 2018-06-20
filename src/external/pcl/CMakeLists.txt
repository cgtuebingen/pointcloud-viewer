add_library(pcl STATIC
  include/pcl/io/ply/byte_order.h
  include/pcl/io/ply/io_operators.h
  include/pcl/io/ply/ply.h
  include/pcl/io/ply/ply_parser.h
  src/pcl/io/ply/ply_parser.cpp
)

target_include_directories(pcl PUBLIC include)
