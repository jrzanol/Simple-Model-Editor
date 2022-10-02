// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CCamera.h"

CCamera::CCamera()
{
    m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    m_Yaw = YAW;
    m_Pitch = PITCH;
    m_FirstMouse = true;
    m_Zoom = 45.f;

    UpdateCameraVectors();
}

CCamera::~CCamera()
{
}

void CCamera::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        ProcessKeyboard(Camera_Movement::FORWARD, g_DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        ProcessKeyboard(Camera_Movement::BACKWARD, g_DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        ProcessKeyboard(Camera_Movement::LEFT, g_DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        ProcessKeyboard(Camera_Movement::RIGHT, g_DeltaTime);
}

void CCamera::ProcessMouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
            m_FirstMouse = false;
        else if (action == GLFW_RELEASE)
            m_FirstMouse = true;
    }
}

void CCamera::ProcessMouseEvent(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (m_FirstMouse)
    {
        m_LastX = xpos;
        m_LastY = ypos;
    }

    float xoffset = xpos - m_LastX;
    float yoffset = m_LastY - ypos; // reversed since y-coordinates go from bottom to top

    m_LastX = xpos;
    m_LastY = ypos;

    ProcessMouseMovement(xoffset, yoffset);
}

void CCamera::ProcessMouseScroll(GLFWwindow* window, double _xoffset, double _yoffset)
{
    float yoffset = static_cast<float>(_yoffset);

    m_Zoom -= (float)yoffset;
    if (m_Zoom < 1.0f)
        m_Zoom = 1.0f;
    if (m_Zoom > 45.0f)
        m_Zoom = 45.0f;
}

glm::mat4 CCamera::GetViewMatrix()
{
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void CCamera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= SENSITIVITY;
    yoffset *= SENSITIVITY;

    m_Yaw += xoffset;
    m_Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

void CCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = (7.5f * deltaTime);

    if (direction == Camera_Movement::FORWARD)
        m_Position += m_Front * velocity;
    if (direction == Camera_Movement::BACKWARD)
        m_Position -= m_Front * velocity;
    if (direction == Camera_Movement::LEFT)
        m_Position -= m_Right * velocity;
    if (direction == Camera_Movement::RIGHT)
        m_Position += m_Right * velocity;
}

void CCamera::UpdateCameraVectors()
{
    // Calculate the new Front vector.
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    m_Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

