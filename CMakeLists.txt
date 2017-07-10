# Global prerequisites.
cmake_minimum_required(VERSION 3.7.1)



# Project main configs.
project(tf-serving-client-cpp)



# Protobuf dependency.
include(FindProtobuf)
find_package(Protobuf REQUIRED)

# gRPC plugin dependency.
include(ProtoGenGrpc.cmake)

# OpenCV dependency.
find_package(OpenCV
        HINTS /usr/local/opt/opencv3
        REQUIRED core imgcodecs)
include_directories(${OpenCV_INCLUDE_DIRS})



# Keeping the same files system structure in generated sources.
set(PROTOBUF_GENERATE_CPP_APPEND_PATH OFF)

# Including generated files as sources for project.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Files to generate usual sources from proto.
protobuf_generate_cpp(PROTO_SOURCES PROTO_HEADERS
        tensorflow/core/example/example.proto
        tensorflow/core/example/feature.proto
        tensorflow/core/framework/attr_value.proto
        tensorflow/core/framework/function.proto
        tensorflow/core/framework/graph.proto
        tensorflow/core/framework/node_def.proto
        tensorflow/core/framework/op_def.proto
        tensorflow/core/framework/resource_handle.proto
        tensorflow/core/framework/tensor.proto
        tensorflow/core/framework/tensor_shape.proto
        tensorflow/core/framework/types.proto
        tensorflow/core/framework/versions.proto
        tensorflow/core/protobuf/meta_graph.proto
        tensorflow/core/protobuf/saver.proto
        tensorflow_serving/apis/classification.proto
        tensorflow_serving/apis/get_model_metadata.proto
        tensorflow_serving/apis/inference.proto
        tensorflow_serving/apis/input.proto
        tensorflow_serving/apis/model.proto
        tensorflow_serving/apis/predict.proto
        tensorflow_serving/apis/regression.proto
        tensorflow_serving/apis/prediction_service.proto)

# File to generate source from proto with gRPC plugin.
protobuf_generate_grpc_cpp(PROTO_GRPC_SOURCES PROTO_GRPC_HEADERS
        tensorflow_serving/apis/prediction_service.proto)



# Sources to build the project from.
include_directories(include)
set(SOURCES main.cpp)



# A target to generate an executable binary.
add_executable(${CMAKE_PROJECT_NAME}
        ${PROTO_HEADERS} ${PROTO_SOURCES}
        ${PROTO_GRPC_HEADERS} ${PROTO_GRPC_SOURCES}
        ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${PROTOBUF_LIBRARIES} grpc++ ${OpenCV_LIBS})

# Configs for target.
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)