#include "glm/ext/matrix_transform.hpp"
#include <iostream>
#include "Camera.h"
#include "../../../Final Day On Earth/src/Rendering/shapeRenderer.h"

Camera::Camera(float x, float y, float z, float aspectRatio) : location(x, y, z), aspectRatio(aspectRatio) {
}

void Camera::move(Direction direction, float movement) {
    if (direction == RIGHT)
        location += movement * right;
    if (direction == LEFT)
        location += -movement * right;
    if (direction == UP)
        location += movement * up;
    if (direction == DOWN)
        location += -movement * up;
    if (direction == FORWARDS)
        location += movement * forward;
    if (direction == BACKWARDS)
        location += -movement * forward;
}

void Camera::rotate(double mouseDeltaX, double mouseDeltaY) {
    yaw = fmod((yaw - mouseDeltaX), 360.0);
    pitch = fmod((pitch + mouseDeltaY), 360.0);

    double y = glm::radians(yaw);
    double p = glm::radians(pitch);
    double r = glm::radians(roll);

    forward = glm::normalize(glm::vec3(-sin(y) * cos(p), sin(p), cos(y) * cos(p))); // Find based on pitch and yaw

    glm::vec3 Rtemp = glm::normalize(glm::vec3(cos(y), 0, sin(y)));

    glm::vec3 Utemp = glm::normalize(glm::cross(forward, Rtemp));

    right = glm::normalize(Rtemp * (float) cos(r) + Utemp * (float) sin(r)); // roll around the forward vector
    up = glm::normalize(Rtemp * (float) -sin(r) + Utemp * (float) cos(r));
}

glm::mat4 Camera::getCameraMatrix() {
    //Assumptions : Axes are perpendicular and equal magnitude (to eachother)
    // The new point can be described by three vectors along the camera's x, y, and z axes
    // These vectors can be described by those axes times the original point's corresponding coordinate (treated as magnitude)
    // If you add these vectors together (add their components together), then you will have the rotated point


    //SINCE THIS IS A CAMERA, ROTATE ALL POINTS OPPOSITE TO THE CAMERA'S ROTATION, THIS MEANS THE MATRIX WILL BE
    //TRANSPOSED AND THE TRANSLATION WILL BE NEGATED  234`

    // Clip space looks down the negative z axis
    glm::vec3 front = -forward;

    // Dot product is equivalent to the result of a transformation matrix multiplied by the rotation matrix

    //STORED IN COLUMN MAJOR FOR OPENGL

    /*std::cout << "Right: " << right.x << " " << right.y << " " << right.z << std::endl;
    std::cout << "Up: " << up.x << " " << up.y << " " << up.z << std::endl;
    std::cout << "Forward: " << forward.x << " " << forward.y << " " << forward.z << std::endl;
    std::cout << "Location: " << location.x << " " << location.y << " " << location.z << std::endl;*/

    glm::mat4 Result;
    Result[0][0] = right.x;
    Result[1][0] = right.y;
    Result[2][0] = right.z;
    Result[3][0] = -dot(right, location);
    Result[0][1] = up.x;
    Result[1][1] = up.y;
    Result[2][1] = up.z;
    Result[3][1] = -dot(up, location);
    Result[0][2] = front.x;
    Result[1][2] = front.y;
    Result[2][2] = front.z;
    Result[3][2] = -dot(front, location);
    Result[3][3] = 1.0f;

    return Result;
}


float Camera::getPitch() const {
    return pitch;
}

void Camera::setPitch(float pitch) {
    Camera::pitch = pitch;
}

float Camera::getYaw() const {
    return yaw;
}

void Camera::setYaw(float yaw) {
    Camera::yaw = yaw;
}

float Camera::getRoll() const {
    return roll;
}

void Camera::setRoll(float roll) {
    Camera::roll = roll;
}

const glm::vec3 &Camera::getLocation() const {
    return location;
}

void Camera::setLocation(const glm::vec3 &l) {
    Camera::location = l;
}

glm::vec3 Camera::getDirection() const {
    return forward;
}

Frustum Camera::buildFrustum() {
    // Distance is the dot product of the normal and any point on the plane
    // This will project any point on the plane to the same location, as the normal is perpendicular to the plane

    // Find a vector along the plane and cross it with a unit vector is perpendicular with it to find its normal

    // The vectors along left, right, up, and down planes can be found from their intersections with the far Z plane
    // Add forwards and vertical/horizontal components based on fov and unit vectors
    float halfVerticalSide = std::tanf(glm::radians(getFovY() * 0.5f)) * zFar;
    float halfHorizontalSide = aspectRatio * halfVerticalSide;
    glm::vec3 farPlanePoint = zFar * forward;

    return {
            Plane{glm::normalize(forward), location + zNear * forward},
            Plane{glm::normalize(-forward), location + farPlanePoint},
            Plane{glm::normalize(glm::cross(up, farPlanePoint - right * halfHorizontalSide)), location},
            Plane{glm::normalize(glm::cross(farPlanePoint + right * halfHorizontalSide, up)), location},
            Plane{glm::normalize(glm::cross(right, farPlanePoint + up * halfVerticalSide)), location},
            Plane{glm::normalize(glm::cross(farPlanePoint - up * halfVerticalSide, right)), location}
    };
}

void Camera::drawFrustum(ShapeRenderer &shapeRenderer, Shader &basicShader) {
    Frustum frustum = buildFrustum();


    float halfVerticalSide = std::tanf(glm::radians(getFovY() * 0.5f)) * 1.0f;
    float halfHorizontalSide = aspectRatio * halfVerticalSide;
    glm::vec3 farPlanePoint = zFar * forward * 1.0f;

    glm::vec3 nearPlanePoint = zNear * forward * 1.0f;

    basicShader.setVec3("color", glm::vec3(1.0, 0.0, 0.0));

    shapeRenderer.drawLine(
            getLocation() + farPlanePoint + right * halfHorizontalSide * zFar + up * halfVerticalSide * zFar,
            getLocation() + farPlanePoint - right * halfHorizontalSide * zFar + up * halfVerticalSide * zFar);

    shapeRenderer.drawLine(
            getLocation() + farPlanePoint + right * halfHorizontalSide * zFar - up * halfVerticalSide * zFar,
            getLocation() + farPlanePoint - right * halfHorizontalSide * zFar - up * halfVerticalSide * zFar);

    shapeRenderer.drawLine(
            getLocation() + farPlanePoint + right * halfHorizontalSide * zFar - up * halfVerticalSide * zFar,
            getLocation() + farPlanePoint + right * halfHorizontalSide * zFar + up * halfVerticalSide * zFar);

    shapeRenderer.drawLine(
            getLocation() + farPlanePoint - right * halfHorizontalSide * zFar - up * halfVerticalSide * zFar,
            getLocation() + farPlanePoint - right * halfHorizontalSide * zFar + up * halfVerticalSide * zFar);


    //----------------------------------------------------------------------------------------------------------------------------------------------------

    shapeRenderer.drawLine(
            getLocation() + nearPlanePoint + right * halfHorizontalSide * zNear + up * halfVerticalSide * zNear,
            getLocation() + nearPlanePoint - right * halfHorizontalSide * zNear + up * halfVerticalSide * zNear);

    shapeRenderer.drawLine(
            getLocation() + nearPlanePoint + right * halfHorizontalSide * zNear - up * halfVerticalSide * zNear,
            getLocation() + nearPlanePoint - right * halfHorizontalSide * zNear - up * halfVerticalSide * zNear);

    shapeRenderer.drawLine(
            getLocation() + nearPlanePoint + right * halfHorizontalSide * zNear - up * halfVerticalSide * zNear,
            getLocation() + nearPlanePoint + right * halfHorizontalSide * zNear + up * halfVerticalSide * zNear);

    shapeRenderer.drawLine(
            getLocation() + nearPlanePoint - right * halfHorizontalSide * zNear - up * halfVerticalSide * zNear,
            getLocation() + nearPlanePoint - right * halfHorizontalSide * zNear + up * halfVerticalSide * zNear);

    //----------------------------------------------------------------------------------------------------------------------------------------------------

    shapeRenderer.drawLine(
            getLocation() + farPlanePoint + right * halfHorizontalSide * zFar + up * halfVerticalSide * zFar,
            getLocation() + nearPlanePoint + right * halfHorizontalSide * zNear + up * halfVerticalSide * zNear);

    shapeRenderer.drawLine(
            getLocation() + farPlanePoint - right * halfHorizontalSide * zFar + up * halfVerticalSide * zFar,
            getLocation() + nearPlanePoint - right * halfHorizontalSide * zNear + up * halfVerticalSide * zNear);

    shapeRenderer.drawLine(
            getLocation() + farPlanePoint + right * halfHorizontalSide * zFar - up * halfVerticalSide * zFar,
            getLocation() + nearPlanePoint + right * halfHorizontalSide * zNear - up * halfVerticalSide * zNear);

    shapeRenderer.drawLine(
            getLocation() + farPlanePoint - right * halfHorizontalSide * zFar - up * halfVerticalSide * zFar,
            getLocation() + nearPlanePoint - right * halfHorizontalSide * zNear - up * halfVerticalSide * zNear);

    // NORMALS

    basicShader.setVec3("color", glm::vec3(0.0, 1.0, 0.0));

    shapeRenderer.drawLine(
            getLocation() + forward * 10.0f - right * halfHorizontalSide * 10.0f,
            getLocation() + forward * 10.0f - right * halfHorizontalSide * 10.0f + frustum.left.normal * 5.0f);

    shapeRenderer.drawLine(
            getLocation() + forward * 10.0f + right * halfHorizontalSide * 10.0f,
            getLocation() + forward * 10.0f + right * halfHorizontalSide * 10.0f + frustum.right.normal * 5.0f);

    shapeRenderer.drawLine(
            getLocation() + forward * 10.0f + up * halfVerticalSide * 10.0f,
            getLocation() + forward * 10.0f + up * halfVerticalSide * 10.0f + frustum.top.normal * 5.0f);

    shapeRenderer.drawLine(
            getLocation() + forward * 10.0f - up * halfVerticalSide * 10.0f,
            getLocation() + forward * 10.0f - up * halfVerticalSide * 10.0f + frustum.bottom.normal * 5.0f);

    shapeRenderer.drawLine(getLocation() + nearPlanePoint, getLocation() + nearPlanePoint + frustum.forwards.normal * 5.0f);

    shapeRenderer.drawLine(getLocation() + farPlanePoint, getLocation() + farPlanePoint + frustum.backwards.normal * 5.0f);

}

glm::mat4x4 Camera::getProjectionMatrix() {

    float PI = 3.141592653;
    float S = 1.0f / (std::tan(PI * getFovY() / 360.0f));

    glm::mat4 Result;

    Result[0] = {S, 0, 0, 0};
    Result[1] = {0, S, 0, 0};
    Result[2] = {0,0,-zFar / (zFar - zNear),-1};
    Result[3] = {0,0,-zFar * zNear / (zFar - zNear),0};

    return Result;
}

float Camera::getFovY() {
    return fovY / zoom;
}

void Camera::setZoom(float nZoom) {
    zoom = nZoom;
}

float Camera::getFovX() {
    return 90.0f;
}

Plane::Plane(glm::vec3 normal, glm::vec3 point) : normal(normal), distance(glm::dot(point, normal)) {
}
