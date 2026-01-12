#ifndef FINALDAYONEARTH_CAMERA_H
#define FINALDAYONEARTH_CAMERA_H

#include "glm/glm.hpp"

enum Direction {
    LEFT,
    RIGHT,
    UP,
    DOWN,
    FORWARDS,
    BACKWARDS
};

struct Plane {
    glm::vec3 normal;
    float distance;

    Plane(glm::vec3 normal, glm::vec3 point);
};

struct Frustum {
    Plane forwards;
    Plane backwards;
    Plane left;
    Plane right;
    Plane top;
    Plane bottom;
};

class Shader;

class ShapeRenderer;

class Camera {
public:
    // Settings
    float aspectRatio;

    float zNear = 0.1;
    float zFar = 1000;
    float zoom = 1.0f;

    // Unit Vectors
    glm::vec3 forward{0, 0, 1};
    glm::vec3 up{0, 1, 0};
    glm::vec3 right{1, 0, 0};

    Camera(float x, float y, float z, float aspectRatio);

    glm::mat4x4 getCameraMatrix();

    glm::mat4x4 getProjectionMatrix();

    float getPitch() const;

    void setPitch(float pitch);

    float getYaw() const;

    void setYaw(float yaw);

    float getRoll() const;

    void setRoll(float roll);

    const glm::vec3 &getLocation() const;

    void setLocation(const glm::vec3 &location);

    void rotate(double mouseDeltaX, double mouseDeltaY);

    void move(Direction direction, float movement);

    float getFovY();

    void setZoom(float nZoom);

    Frustum buildFrustum();

    glm::vec3 getDirection() const;

    void drawFrustum(ShapeRenderer &shapeRenderer, Shader &basicShader);

private:
    glm::vec3 location;

    //All are measured counter-clockwise from their corresponding axes
    double pitch = 0.0f;
    double yaw = 0.0f;
    double roll = 0.0f;

    float fovY = 105;

    float getFovX();
};


#endif //FINALDAYONEARTH_CAMERA_H
