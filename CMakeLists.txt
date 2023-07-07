cmake_minimum_required(VERSION 3.2)
project(VulkanBase VERSION 1.0.0)

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

cmake_policy(SET CMP0069 NEW)
add_subdirectory(src)