cmake_minimum_required(VERSION 3.10)
project(BinaryToRoot)

set(CMAKE_CXX_STANDARD 20)

find_package(ROOT REQUIRED COMPONENTS Core RIO Tree)
include(${ROOT_USE_FILE})

# include 디렉토리
include_directories(include)

# Dictionary를 위한 헤더 목록
set(DICT_HEADERS
    functions.h
)

# LinkDef 파일 경로
set(LINKDEF_FILE
    include/LinkDef.h
)

# Dictionary 파일 이름
set(DICT_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Dict.cxx
)

# Dictionary 생성 (ROOT macro)
ROOT_GENERATE_DICTIONARY(Dict
    ${DICT_HEADERS}
    LINKDEF ${LINKDEF_FILE}
)

# Dictionary 소스와 헤더를 라이브러리로 빌드
add_library(dictlib SHARED ${DICT_SOURCES})
target_link_libraries(dictlib ${ROOT_LIBRARIES})

# 소스 빌드
add_subdirectory(src)

# main 실행 파일 빌드
add_executable(BinaryToRoot main.cpp)
target_link_libraries(BinaryToRoot
    test
    getFile
    DAQTypeAParser
    DAQTypeBParser
    getMap
    dictlib  # <- 반드시 dictionary 링크
    ${ROOT_LIBRARIES}
)

install(TARGETS BinaryToRoot DESTINATION bin)

