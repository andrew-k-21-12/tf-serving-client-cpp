# Global prerequisites.
cmake_minimum_required(VERSION 3.5)



# Project main configs.
project(tf-serving-client-cpp)



# Protobuf dependency.
include(FindProtobuf)
find_package(Protobuf REQUIRED)

# Protobuf compiler dependency.
include(CompileProto.cmake)

# OpenCV dependency.
find_package(OpenCV
        HINTS /usr/local/opt/opencv3
        REQUIRED core imgcodecs)
include_directories(${OpenCV_INCLUDE_DIRS})



# Protobuf sources of the TensorFlow Serving to be compiled without a gRPC plugin.
file(GLOB_RECURSE TF_PROTOS proto/*.proto)

# Compiling CPP sources from proto files.
compile_proto(0 "${CMAKE_SOURCE_DIR}/proto" "${CMAKE_SOURCE_DIR}/compiled" PB_SOURCES PB_HEADERS ${TF_PROTOS})

# Compiling CPP sources with gRPC plugin.
compile_proto(1 "${CMAKE_SOURCE_DIR}/proto" "${CMAKE_SOURCE_DIR}/compiled" PB_GRPC_SOURCES PB_GRPC_HEADERS
        proto/tensorflow_serving/apis/prediction_service.proto)

# Including compiled files.
include_directories(compiled)



# Sources to build the project from.
include_directories(include)
set(SOURCES main.cpp)



# A target to generate an executable binary.
add_executable(${CMAKE_PROJECT_NAME}
        ${PB_HEADERS} ${PB_SOURCES}
        ${PB_GRPC_HEADERS} ${PB_GRPC_SOURCES}
        ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${PROTOBUF_LIBRARIES} grpc++ ${OpenCV_LIBS})

# Configs for target.
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)