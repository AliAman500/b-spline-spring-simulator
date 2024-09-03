#include "curves.h"
#include "GLFW/glfw3.h"

std::vector<glm::vec3> splitLine(const glm::vec3& p1, const glm::vec3& p2, int numDivisions) {
    std::vector<glm::vec3> divisionPoints;

    glm::vec3 step = (p2 - p1) / static_cast<float>(numDivisions);

    for (int i = 1; i <= numDivisions - 1; ++i) {
        glm::vec3 divisionPoint = p1 + static_cast<float>(i) * step;
		divisionPoints.push_back(divisionPoint);
    }

	return divisionPoints;
}

void plotPoint(const glm::vec3& p, const glm::vec3& color) {
	glBegin(GL_POINTS);
	glColor3f(color.r, color.g, color.b);
	glVertex3f(p.x, p.y, p.z);
	glEnd();
}

void calculateMidpoints(const glm::vec3& p1, const glm::vec3& p2, std::vector<glm::vec3>& splinePoints, bool debug) {
	std::vector<glm::vec3> b = splitLine(p1, p2, 3);
	splinePoints.insert(splinePoints.end(), b.begin(), b.end());

	glPointSize(4);
    for (int i = 0; i < splinePoints.size(); i++)
        if(debug) plotPoint(splinePoints[i], glm::vec3(1.0f, 0.65f, 0));
}

void bezier(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4, bool debug) {
	glLineWidth(2);

	if(debug) {
		glBegin(GL_LINE_STRIP);
		glColor3f(0.5f, 0.03f, 0.3f);
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p2.x, p2.y, p2.z);
		glVertex3f(p3.x, p3.y, p3.z);
		glVertex3f(p4.x, p4.y, p4.z);
		glEnd();
	}
	glBegin(GL_LINE_STRIP);
	glColor3f(1.8f, 0.2f, 1.0f);
	for (float t = 0.0; t <= 1.0; t += 0.01) {
		float x = pow((1 - t), 3) * p1.x + 3 * pow((1 - t), 2) * t * p2.x + 3 * (1 - t) * pow(t, 2) * p3.x + pow(t, 3) * p4.x;
		float y = pow((1 - t), 3) * p1.y + 3 * pow((1 - t), 2) * t * p2.y + 3 * (1 - t) * pow(t, 2) * p3.y + pow(t, 3) * p4.y;
		float z = pow((1 - t), 3) * p1.z + 3 * pow((1 - t), 2) * t * p2.z + 3 * (1 - t) * pow(t, 2) * p3.z + pow(t, 3) * p4.z;
		glVertex3f(x, y, z);
	}
	glEnd();
	glPointSize(4);
	if(debug) {
		plotPoint(p1, glm::vec3(0, 1, 1));
		plotPoint(p2, glm::vec3(0, 1, 1));
		plotPoint(p3, glm::vec3(0, 1, 1));
		plotPoint(p4, glm::vec3(0, 1, 1));
	}
}

void bSpline(const std::vector<glm::vec3>& points, bool debug) {
	std::vector<glm::vec3> splinePoints;
	std::vector<glm::vec3> bezierPoints;

	if(debug) {
		glBegin(GL_LINE_STRIP);
		glColor3f(0, 0.5f, 0);
		for(int i = 0; i < points.size(); i++) {
			glVertex3f(points[i].x, points[i].y, points[i].z);
		}
		glEnd();
	}
	for (size_t i = 0; i < points.size() - 1; i++) {
        const glm::vec3& p1 = points[i];
        const glm::vec3& p2 = points[i + 1];
		calculateMidpoints(p1, p2, splinePoints, debug);
    }

	for(int i = 1; i < splinePoints.size() - 2; i += 2) {
		glm::vec3 bezierPoint = splitLine(splinePoints[i], splinePoints[i+1], 2)[0];
		bezierPoints.push_back(bezierPoint);
		bezierPoints.push_back(splinePoints[i+1]);
		bezierPoints.push_back(splinePoints[i+2]);
	}

	bezierPoints.pop_back();
	bezierPoints.pop_back();

	if(debug) {
		glPointSize(4);
		for(int i = 0; i < bezierPoints.size(); i++) {
			plotPoint(bezierPoints[i], glm::vec3(1, 1, 1));
		}
	}
	for(int i = 0; i < bezierPoints.size() - 3; i += 3) {
		bezier(bezierPoints[i], bezierPoints[i+1], bezierPoints[i+2], bezierPoints[i+3], debug);
	}
}