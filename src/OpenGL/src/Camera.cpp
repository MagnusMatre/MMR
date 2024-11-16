#include "Camera.h"

#include <iostream>

#include "Renderer.h"

Camera::Camera(GLFWwindow* window, glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(m_SPEED), MouseSensitivity(m_SENSITIVITY), Zoom(m_ZOOM)
{
    m_window = window;
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(m_SPEED), MouseSensitivity(m_SENSITIVITY), Zoom(m_ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(GLFWwindow* window, glm::vec3 position, glm::quat orientation) : Position(position), Orientation(orientation), MovementSpeed(10.0f), MouseSensitivity(0.1f), Zoom(45.0f), m_window(window)
{
	m_window = window;
	Position = position;
	Orientation = orientation;
    //yawQuat = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //pitchQuat = glm::angleAxis(glm::radians(0.0f), GetRight());
	updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio)
{
    return glm::perspective(glm::radians(Zoom), aspectRatio, 0.1f, 100.0f);
}

void Camera::ProcessKeyboard(float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
    //TRANSPOSITION
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
        Position += Front * velocity;
    }
    else if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
        Position -= Front * velocity;
    }
    else if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
        Position -= Right * velocity;
    }
    else if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
        Position += Right * velocity;
    }

    else if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        Position += Up * velocity;
    }
    else if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        Position -= Up * velocity;
    }
    
	// WITH QUATERNIONS
   /* if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
        Position += GetFront() * velocity;
    }
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
        Position -= GetFront() * velocity;
    }
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
        Position -= GetRight() * velocity;
    }
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
        Position += GetRight() * velocity;
    }
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        Position += GetUp() * velocity;
    }
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        Position -= GetUp() * velocity;
    }*/

    // ZOOM
    if (glfwGetKey(m_window, GLFW_KEY_X) == GLFW_PRESS) {
        Zoom -= 1.0f;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
    }
    else if (glfwGetKey(m_window, GLFW_KEY_Z) == GLFW_PRESS) {
        Zoom += 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    float xoffset = 0;
    float yoffset = 0;


    // ROTATION

    if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS) {
        Pitch += 0.02 * Zoom;
    }
    else if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        Pitch -= 0.02 * Zoom;
    }
    else if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        Yaw -= 0.02 * Zoom;
    }
    else if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        Yaw += 0.02 * Zoom;
    }

    // With quaternions
 //   if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS) {
 //       Pitch += 0.5f;
 //   }
 //   else if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
 //       Pitch -= 0.5f;
 //   }
 //   if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
 //       Yaw += 0.5f;
 //   }
 //   else if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
 //       Yaw -= 0.5f;
 //   }

 //   // Get correct orientation matrix
	//Orientation = glm::angleAxis(glm::radians(Yaw), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(glm::radians(Pitch), glm::vec3(1.0f, 0.0f, 0.0f));

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (Pitch > 89.0f){
        Pitch = 89.0f;
    }
    if (Pitch < -89.0f){
        Pitch = -89.0f;
    }

    updateCameraVectors();

}

//void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
//{
//    xoffset *= MouseSensitivity;
//    yoffset *= MouseSensitivity;
//
//    Yaw += xoffset;
//    Pitch += yoffset;
//
//    // make sure that when pitch is out of bounds, screen doesn't get flipped
//    if (constrainPitch)
//    {
//        if (Pitch > 89.0f)
//            Pitch = 89.0f;
//        if (Pitch < -89.0f)
//            Pitch = -89.0f;
//    }
//
//    // update Front, Right and Up Vectors using the updated Euler angles
//    updateCameraVectors();
//}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector

    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
     
    Up = glm::normalize(glm::cross(Right, Front));

   /* Front = Orientation * glm::vec3(0.0f, 0.0f, -1.0f);
    Right = Orientation * glm::vec3(1.0f, 0.0f, 0.0f);
    Up = Orientation * glm::vec3(0.0f, 1.0f, 0.0f);*/
}


glm::vec3 Camera::GetFront() {
    return Orientation * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 Camera::GetRight() {
    return Orientation * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 Camera::GetUp() {
    return Orientation * glm::vec3(0.0f, 1.0f, 0.0f);
}
