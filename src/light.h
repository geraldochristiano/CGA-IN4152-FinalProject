#pragma once
#include "abstraction.h"
#include <glm/vec3.hpp>


// Abstract base class for all lights
struct Light {
public:
    glm::vec3 lightColor{ 1,1,1 }; // Default white light color
    glm::vec3 lightIntensities{ 0, 0.5, 1.0 }; // Ambient, diffuse, specular intensities

protected:
    Light() = default;
    Light(const glm::vec3& color) : lightColor(color) {}
    Light(const glm::vec3& color, const glm::vec3& intensity) :
        lightColor(color),
        lightIntensities(intensity) {}
};

struct DirectionalLight : Light {
public:
    glm::vec3 direction;

    DirectionalLight(const glm::vec3& dir) : Light(), direction(dir) {}
    DirectionalLight(const glm::vec3& dir, const glm::vec3& color) : Light(color), direction(dir) {}
    DirectionalLight(const glm::vec3& dir, const glm::vec3& color, const glm::vec3& intensity) :
        Light(color, intensity), direction(dir) {}
};


struct PointLight : Light {
public:
    glm::vec3 position;
    float attenuationLinearCoefficient;
    float attenuationQuadraticCoefficient;

    PointLight(const glm::vec3& pos, float maxDistance) :
        Light(), position(pos) {
        setAttenuationCoefficient(maxDistance);
    }
    PointLight(const glm::vec3& pos, const glm::vec3& color, float maxDistance) :
        Light(color), position(pos) {
        setAttenuationCoefficient(maxDistance);
    }
    PointLight(const glm::vec3& pos, const glm::vec3& color, const glm::vec3& intensity, float maxDistance) :
        Light(color, intensity), position(pos) {
        setAttenuationCoefficient(maxDistance);
    }
private:
    // from https://learnopengl.com/Lighting/Light-casters
    void setAttenuationCoefficient(float maxDistance) {
        if (maxDistance <= 7.0) {
            attenuationLinearCoefficient = 0.7;
            attenuationQuadraticCoefficient = 1.8;
        }
        else if (maxDistance <= 13) {
            attenuationLinearCoefficient = 0.35;
            attenuationQuadraticCoefficient = 0.44;
        }
        else if (maxDistance <= 20) {
            attenuationLinearCoefficient = 0.22;
            attenuationQuadraticCoefficient = 0.20;
        }
        else if (maxDistance <= 32) {
            attenuationLinearCoefficient = 0.14;
            attenuationQuadraticCoefficient = 0.07;
        }
        else if (maxDistance <= 50) {
            attenuationLinearCoefficient = 0.09;
            attenuationQuadraticCoefficient = 0.032;
        }
        else if (maxDistance <= 65) {
            attenuationLinearCoefficient = 0.07;
            attenuationQuadraticCoefficient = 0.017;
        }
        else if (maxDistance <= 100) {
            attenuationLinearCoefficient = 0.045;
            attenuationQuadraticCoefficient = 0.0075;
        }
        else if (maxDistance <= 160) {
            attenuationLinearCoefficient = 0.027;
            attenuationQuadraticCoefficient = 0.0028;
        }
        else if (maxDistance <= 200) {
            attenuationLinearCoefficient = 0.022;
            attenuationQuadraticCoefficient = 0.0019;
        }
        else if (maxDistance <= 325) {
            attenuationLinearCoefficient = 0.014;
            attenuationQuadraticCoefficient = 0.0007;
        }
        else if (maxDistance <= 600) {
            attenuationLinearCoefficient = 0.007;
            attenuationQuadraticCoefficient = 0.0002;
        }
        else if (maxDistance <= 3250) {
            attenuationLinearCoefficient = 0.0014;
            attenuationQuadraticCoefficient = 0.000007;
        }
        else {
            attenuationLinearCoefficient = 0.0001;
            attenuationQuadraticCoefficient = 0.0000001;
        }
    }
};

struct SpotLight : Light {
public:
    glm::vec3 direction;
    float cutoffAngle;
    glm::vec3 position;
    float attenuationLinearCoefficient;
    float attenuationQuadraticCoefficient;

    SpotLight(const glm::vec3& pos, const glm::vec3& dir, float cutoff, float maxDistance) :
        Light(), position(pos), direction(dir), cutoffAngle(cutoff) {
        setAttenuationCoefficient(maxDistance);
    }
    SpotLight(const glm::vec3& pos, const glm::vec3& dir, float cutoff, const glm::vec3& color, float maxDistance) :
        Light(color), position(pos), direction(dir), cutoffAngle(cutoff) {
        setAttenuationCoefficient(maxDistance);
    }
    SpotLight(const glm::vec3& pos, const glm::vec3& dir, float cutoff, const glm::vec3& color, const glm::vec3& intensity, float maxDistance) :
        Light(color, intensity), position(pos), direction(dir), cutoffAngle(cutoff) {
        setAttenuationCoefficient(maxDistance);
    }

private:
    // from https://learnopengl.com/Lighting/Light-casters
    void setAttenuationCoefficient(float maxDistance) {
        if (maxDistance <= 7.0) {
            attenuationLinearCoefficient = 0.7;
            attenuationQuadraticCoefficient = 1.8;
        }
        else if (maxDistance <= 13) {
            attenuationLinearCoefficient = 0.35;
            attenuationQuadraticCoefficient = 0.44;
        }
        else if (maxDistance <= 20) {
            attenuationLinearCoefficient = 0.22;
            attenuationQuadraticCoefficient = 0.20;
        }
        else if (maxDistance <= 32) {
            attenuationLinearCoefficient = 0.14;
            attenuationQuadraticCoefficient = 0.07;
        }
        else if (maxDistance <= 50) {
            attenuationLinearCoefficient = 0.09;
            attenuationQuadraticCoefficient = 0.032;
        }
        else if (maxDistance <= 65) {
            attenuationLinearCoefficient = 0.07;
            attenuationQuadraticCoefficient = 0.017;
        }
        else if (maxDistance <= 100) {
            attenuationLinearCoefficient = 0.045;
            attenuationQuadraticCoefficient = 0.0075;
        }
        else if (maxDistance <= 160) {
            attenuationLinearCoefficient = 0.027;
            attenuationQuadraticCoefficient = 0.0028;
        }
        else if (maxDistance <= 200) {
            attenuationLinearCoefficient = 0.022;
            attenuationQuadraticCoefficient = 0.0019;
        }
        else if (maxDistance <= 325) {
            attenuationLinearCoefficient = 0.014;
            attenuationQuadraticCoefficient = 0.0007;
        }
        else if (maxDistance <= 600) {
            attenuationLinearCoefficient = 0.007;
            attenuationQuadraticCoefficient = 0.0002;
        }
        else if (maxDistance <= 3250) {
            attenuationLinearCoefficient = 0.0014;
            attenuationQuadraticCoefficient = 0.000007;
        }
        else {
            attenuationLinearCoefficient = 0.0001;
            attenuationQuadraticCoefficient = 0.0000001;
        }
    }
};