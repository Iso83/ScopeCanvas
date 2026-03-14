#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "diagram/DiagramBasics.h"
#include "windows/NodeDiagramWindow.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace {
int RunStudioApp() {
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

	DiagramBasics basics;
	basics.seedDefaultDemo();

	Renderer renderer;
	int w = 0;
	int h = 0;
	glfwGetFramebufferSize(window, &w, &h);
	renderer.init(w, h);

	GraphView viewA{ 1, glm::vec2(0.0f, 0.0f), 1.0f, 0 };
	GraphView viewB{ 2, glm::vec2(200.0f, -80.0f), 0.9f, 0 };

	NodeDiagramWindow diagramA(window, &renderer, &basics, &viewA, "Diagram View A");
	NodeDiagramWindow diagramB(window, &renderer, &basics, &viewB, "Diagram View B");

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
				if (ImGui::MenuItem("Load Basic Demo")) {
					basics.seedDefaultDemo();
				}
				if (ImGui::MenuItem("Load SHA256 Demo")) {
					basics.seedSha256Demo();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) {
					glfwSetWindowShouldClose(window, true);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit")) {
				const bool canUndo = basics.engine().commands().canUndo();
				const bool canRedo = basics.engine().commands().canRedo();
				if (ImGui::MenuItem("Undo", "Ctrl+Z", false, canUndo)) {
					(void)basics.engine().commands().undo();
				}
				if (ImGui::MenuItem("Redo", "Ctrl+Y / Ctrl+Shift+Z", false, canRedo)) {
					(void)basics.engine().commands().redo();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Duplicate Selected", "Ctrl+D")) {
					basics.duplicateSelected();
				}
				if (ImGui::MenuItem("Delete Selected", "Delete")) {
					basics.deleteSelected();
				}
				if (ImGui::MenuItem("Rearrange Connectors", "R")) {
					basics.alignSelectedConnectors();
				}
				if (ImGui::MenuItem("Add Input Connector")) {
					basics.addInputConnectorToSelection();
				}
				if (ImGui::MenuItem("Add Output Connector")) {
					basics.addOutputConnectorToSelection();
				}
				if (ImGui::MenuItem("Delete Last Connector")) {
					basics.removeLastConnectorFromSelection();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Create")) {
				if (ImGui::MenuItem("Number", "1")) {
					basics.createNode("Number", viewA.cameraPosition + glm::vec2(-140.0f, 0.0f));
				}
				if (ImGui::MenuItem("Add", "2")) {
					basics.createNode("Add", viewA.cameraPosition);
				}
				if (ImGui::MenuItem("Multiply", "3")) {
					basics.createNode("Multiply", viewA.cameraPosition + glm::vec2(140.0f, 0.0f));
				}
				if (ImGui::MenuItem("Output", "4")) {
					basics.createNode("Output", viewA.cameraPosition + glm::vec2(280.0f, 0.0f));
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Group")) {
				if (ImGui::MenuItem("Create Group from Selection")) {
					basics.createGroupFromSelection(false);
				}
				if (ImGui::MenuItem("Create Collapsed Group from Selection")) {
					basics.createGroupFromSelection(true);
				}
				if (ImGui::MenuItem("Toggle Collapse on Selected Group(s)", "C")) {
					basics.toggleSelectedGroupsCollapsed();
				}
				if (ImGui::MenuItem("Toggle Bit Container Scope")) {
					basics.toggleBitContainersCollapsed();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View")) {
				ImGui::MenuItem("Show Grid", "G", &basics.gridSettings().enabled);
				ImGui::MenuItem("Snap to Grid", "Shift+G", &basics.gridSettings().snapEnabled);
				ImGui::MenuItem("Curved Edge Overlay", nullptr, &basics.viewSettings().curvedEdgeOverlay);
				ImGui::MenuItem("SHA Node Styling", nullptr, &basics.viewSettings().shaNodeStyling);
				ImGui::MenuItem("Connector State Colors", nullptr, &basics.viewSettings().connectorStateColors);

				if (ImGui::BeginMenu("Grid Size")) {
					if (ImGui::MenuItem("16")) {
						basics.gridSettings().cellSize = 16.0f;
					}
					if (ImGui::MenuItem("32")) {
						basics.gridSettings().cellSize = 32.0f;
					}
					if (ImGui::MenuItem("64")) {
						basics.gridSettings().cellSize = 64.0f;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			ImGui::Text("Selected: %zu", basics.selectedNodeCount());
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
}

#ifdef _WIN32
#include <windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	return RunStudioApp();
}
#else
int main() {
	return RunStudioApp();
}
#endif
