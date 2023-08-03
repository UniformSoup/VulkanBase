#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace VulkanBase
{
    class Camera
    {
        glm::mat4 m_projection{1.f}, m_view{1.f};

        public:
        Camera(float const& fov, float const& aspect, float const& near, float const& far)
        {
            m_projection = glm::perspective(fov, aspect, near, far);
            //m_projection[1][1] *= -1.f;
        }

        void setViewDirection(glm::vec3 const& position, glm::vec3 const& direction, glm::vec3 const& up = {0.f, 1.f, 0.f})
        {
            m_view = glm::lookAt(position, position + direction, up);
        }

        void setViewTarget(glm::vec3 const& position, glm::vec3 const& target, glm::vec3 const& up = {0.f, 1.f, 0.f})
        {
            m_view = glm::lookAt(position, target, up);
        }

        glm::mat4 const& projection() const
        {
            return m_projection;
        }

        glm::mat4 const& view() const
        {
            return m_view;
        }
    };
}