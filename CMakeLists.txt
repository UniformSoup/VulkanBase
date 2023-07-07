cmake_minimum_required(VERSION 3.2)
project(VulkanBase VERSION 1.0.0)

find_package(Vulkan REQUIRED)

add_subdirectory(src)

target_link_libraries(VulkanBase PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(VulkanBase PUBLIC ${Vulkan_INCLUDE_DIRS} "./include/VulkanBase")