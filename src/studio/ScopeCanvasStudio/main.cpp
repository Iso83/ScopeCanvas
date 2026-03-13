#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Engine/CanvasEngine.h"
#include "windows/NodeDiagramWindow.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

int main() {
	if (!glfwInit()) {
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(1280, 800, "ScopeCanvas Studio", nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	glfwSwapInterval(1);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	CanvasEngine engine;
	Node *node1 = engine.graph().createNodeOfType("Number", { -280.0f, -40.0f }, { 180.0f, 100.0f });
	Node *node2 = engine.graph().createNodeOfType("Add", { 0.0f, 90.0f }, { 220.0f, 120.0f });
	Node *node3 = engine.graph().createNodeOfType("Output", { 260.0f, -150.0f }, { 200.0f, 110.0f });
	if (node1 != nullptr && node2 != nullptr) {
		engine.graph().createEdge(node1->id, node1->connectors[0].id, node2->id, node2->connectors[0].id);
	}
	if (node2 != nullptr && node3 != nullptr) {
		engine.graph().createEdge(node2->id, node2->connectors.back().id, node3->id, node3->connectors[0].id);
	}
	engine.graph().syncIdCounters();

	Renderer renderer;
	int w = 0;
	int h = 0;
	glfwGetFramebufferSize(window, &w, &h);
	renderer.init(w, h);

	GraphView viewA{ 1, glm::vec2(0.0f, 0.0f), 1.0f, 0 };
	GraphView viewB{ 2, glm::vec2(200.0f, -80.0f), 0.9f, 0 };

	NodeDiagramWindow diagramA(window, &renderer, &engine.graph(), &viewA, "Diagram View A");
	NodeDiagramWindow diagramB(window, &renderer, &engine.graph(), &viewB, "Diagram View B");

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit")) {
					glfwSetWindowShouldClose(window, true);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		diagramA.Draw();
		diagramB.Draw();

		ImGui::Render();

		int display_w = 0;
		int display_h = 0;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow *backup = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup);
		}

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
