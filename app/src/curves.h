#pragma once

#include <stdlib.h>
#include <glm/glm.hpp>

std::vector<glm::vec3> splitLine(const glm::vec3& p1, const glm::vec3& p2, int numDivisions);
void plotPoint(const glm::vec3& p, const glm::vec3& color);
void calculateMidpoints(const glm::vec3& p1, const glm::vec3& p2, std::vector<glm::vec3>& splinePoints, bool debug);
void bezier(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4, bool debug);
void bSpline(const std::vector<glm::vec3>& points, bool debug);