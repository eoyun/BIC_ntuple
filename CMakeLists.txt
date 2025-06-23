cmake_minimum_required(VERSION 3.10)
project(BinaryToRoot)

set(CMAKE_CXX_STANDARD 17)

find_package(ROOT REQUIRED COMPONENTS Core RIO Tree)
include(${ROOT_USE_FILE})  # ROOT 환경 설정 포함

# 헤더와 라이브러리 경로
include_directories(include)

# 소스 빌드
add_subdirectory(src)

# main 파일을 실행 파일로 빌드
add_executable(BinaryToRoot main.cpp)
target_link_libraries(BinaryToRoot test 
	getFile
	DAQTypeAParser
	DAQTypeBParser
	${ROOT_LIBRARIES})

# 설치 설정
install(TARGETS BinaryToRoot DESTINATION bin)
