add_library(test test.cpp)
add_library(getFile getFile.cpp)
add_library(DAQTypeAParser DAQTypeAParser.cpp)
add_library(DAQTypeBParser DAQTypeBParser.cpp)
target_include_directories(test PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(getFile PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(DAQTypeAParser PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(DAQTypeBParser PUBLIC ${PROJECT_SOURCE_DIR}/include)

