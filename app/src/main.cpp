#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"

#include <cstdio>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "mouse.h"
#include "curves.h"
#include <glm/gtc/matrix_transform.hpp>

void drawSquare() {
	glBegin(GL_QUADS);
	glVertex3f(-0.5f, 0.0f, 0.5f); // Bottom-left
	glVertex3f(-0.5f, 0.0f, -0.5f); // Top-left
	glVertex3f(0.5f, 0.0f, -0.5f); // Top-right
	glVertex3f(0.5f, 0.0f, 0.5f); // Bottom-right
	glEnd();
}

int main(int argc, char **argv) {
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	unsigned int width = 720;
	unsigned int height = 530;

	GLFWwindow *window = glfwCreateWindow(width, height, "project", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	Mouse mouse(window);
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	#ifdef LINUX
		ImGui::GetIO().Fonts->AddFontFromFileTTF("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 15.0f);
	#elif WINDOWS
		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:/Windows/Fonts/bahnschrift.ttf", 15.0f);
	#endif
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL2_Init();

	glm::vec2* currentControlPoint;
	bool controlPointSelected = false;
	glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float) width / (float) height, 0.01f, 100.0f);
	glm::vec3 camPos(0, 2, 2);

	glm::vec3 rotation(glm::radians(45.0f), 0, 0);
	glm::vec3 targetRot(rotation);
	glm::vec3 targetPosition(camPos);
	float dist = 3;
	float targetZoom = dist;

	float zoomSmoothing = 0.15f;
	float zoomSensitivity = 0.06f;
	float rotationSensitivity = 0.01f;
	float rotationSmoothness = 0.15f;

	int c = 6;
	float j = 0.02;
	float radius = 0.5f;

	float prevTime = 0;
	bool debug = false;

	while (!glfwWindowShouldClose(window)) {
		float currTime = glfwGetTime();
		float dt = currTime - prevTime;
		prevTime = currTime;

		glfwPollEvents();
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		if (mouse.isButtonDown(GLFW_MOUSE_BUTTON_MIDDLE)) {
			targetRot.x += mouse.getDelta().y * rotationSensitivity;
			targetRot.y -= mouse.getDelta().x * rotationSensitivity;
		}

		rotation.x += (targetRot.x - rotation.x) * rotationSmoothness;
		rotation.y += (targetRot.y - rotation.y) * rotationSmoothness;

		targetZoom -= mouse.getDWheel() * zoomSensitivity;
		dist += (targetZoom - dist) * zoomSmoothing;

		camPos.x = dist * glm::cos(rotation.x) * glm::sin(rotation.y);
		camPos.y = dist * glm::sin(rotation.x);
		camPos.z = dist * glm::cos(rotation.x) * glm::cos(rotation.y);

		glm::mat4 cam = glm::lookAt<float>(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		ImGui::NewFrame();
		
		ImGui::Begin("spring control");
		ImGui::SliderFloat("radius", &radius, 0.1f, 2);
		ImGui::SliderInt("coils", &c, 3, 20);
		ImGui::SliderFloat("compression", &j, 0.025f, 0.2f, "%.3f");
		ImGui::Checkbox("debug", &debug);

		// for spring height of 10, compression is 0.125 and coils are 20

		std::vector<glm::vec3> controlPoints;
		float z = -0.5f;

		for (int i = 0; i < c; i++) {
			controlPoints.push_back({ -radius, z += j, radius });
			controlPoints.push_back({ -radius, z += j, -radius });
			controlPoints.push_back({ radius, z += j, -radius });
			controlPoints.push_back({ radius, z += j, radius });
		}

		ImGui::TextColored({ 1, 1, 0, 1 }, "spring height: %.2f", z);
		ImGui::End();

		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glLoadMatrixf(&projection[0][0]);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glLoadMatrixf(&cam[0][0]);

		glClearColor(0, 0.1f, 0.1f, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		bSpline(controlPoints, debug);
		if(debug) {
			glPointSize(8);
			for(int i = 0; i < controlPoints.size(); i++)
				plotPoint(controlPoints[i], glm::vec3(0, 1, 0));
		}
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		mouse.update();
	}

	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(nullptr);

	glfwTerminate();
	return 0;
}
