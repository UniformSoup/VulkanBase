add_library(
    VulkanBase
    Device.cpp
    Pipeline.cpp
    SwapChain.cpp
    Model.cpp
)

target_link_libraries(VulkanBase PUBLIC ${Vulkan_LIBRARIES} glm::glm)
target_include_directories(VulkanBase PUBLIC ${Vulkan_INCLUDE_DIRS} "../include/VulkanBase")