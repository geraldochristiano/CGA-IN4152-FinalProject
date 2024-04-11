#pragma once
#include "abstraction.h"
#include <glm/vec3.hpp>


// Abstract base class for all lights
struct Light{ 
public:
    glm::vec3 lightColor{ 1,1,1 }; // Default white light color
    glm::vec3 lightIntensities{ 0, 0.5, 1.0 }; // Ambient, diffuse, specular intensities
    
protected:
    Light() = default;
    Light(const glm::vec3& color) : lightColor(color) {}
    Light(const glm::vec3& color, const glm::vec3& intensity) :
        lightColor(color), 
        lightIntensities(intensity){}
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

    PointLight(const glm::vec3& pos) : Light(), position(pos) {}
    PointLight(const glm::vec3& pos, const glm::vec3& color) : Light(color), position(pos) {}
    PointLight(const glm::vec3& pos, const glm::vec3& color, const glm::vec3& intensity) : 
        Light(color, intensity), position(pos) {}
};

struct SpotLight : Light {
public:
    glm::vec3 direction;
    float cutoffAngle;
    glm::vec3 position;

    SpotLight(const glm::vec3& pos, const glm::vec3& dir, float cutoff) : Light(), position(pos), direction(dir), cutoffAngle(cutoff) {}
    SpotLight(const glm::vec3& pos, const glm::vec3& dir, float cutoff, const glm::vec3& color) : Light(color), position(pos), direction(dir), cutoffAngle(cutoff) {}
    SpotLight(const glm::vec3& pos, const glm::vec3& dir, float cutoff, const glm::vec3& color, const glm::vec3& intensity) : 
        Light(color, intensity), position(pos), direction(dir), cutoffAngle(cutoff) {}
};





