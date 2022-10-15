#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// 屏幕大小
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// 摄像机
Camera camera(glm::vec3(0.0f, 8.0f, 23.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;
glm::vec3 lightPos = glm::vec3(0.0f, 8.0f, 3.0f);

bool triggerMouse = true;

int main()
{
	// 初始化配置
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// 窗口创建
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// 告诉GLFW捕获鼠标
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	

	// glad:加载OpenGL函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 告诉stb_image.h反转加载的图片（只y轴反转）
	stbi_set_flip_vertically_on_load(true);

	// 配置全局OpenGL状态
	glEnable(GL_DEPTH_TEST);

	// 创建并编译shader
	Shader ourShader("shaders/model_loading.vs", "shaders/model_loading.fs");

	// 加载模型
	Model ourModel("asset/models/plane/plane_20_100.obj");

	// 绘制线框模型
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	float _Amplitude = 1.0f;
	float k = 2 * 3.1415926f / 10.0f;
	float speed = std::sqrtf(9.8f / k);
	float speedness = 0.5f;
	static float WaveA[4] = { 0.0f, 1.0f, 0.3f, 3.9f };
	static float WaveB[4] = { 0.0f, 1.0f, 0.25f, 0.9f };
	static float WaveC[4] = { 1.0f, 1.0f, 0.15f, .9f };
	static float WaveD[4] = { 0.5f, 0.5f, 0.5f, 2.5f };

#pragma region ingui
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

#pragma endregion
	// 渲染循环
	while (!glfwWindowShouldClose(window))
	{
#pragma region ingui
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//if (show_demo_window)
		//	ImGui::ShowDemoWindow(&show_demo_window);
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Contral");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("WaveA:");
			ImGui::SliderFloat("WaveAX", &WaveA[0], -2.0f, 2.0f, "ratio = %.3f");
			ImGui::SliderFloat("WaveAY", &WaveA[1], -2.0f, 2.0f, "ratio = %.3f");
			ImGui::SliderFloat("WaveAZ", &WaveA[2], 0.0f, 1.0f, "ratio = %.3f");
			ImGui::SliderFloat("WaveAW", &WaveA[3], 0.0f, 20.0f, "%.1f", ImGuiSliderFlags_Logarithmic);

			ImGui::Text("WaveB:");
			ImGui::SliderFloat("WaveBX", &WaveB[0], -2.0f, 2.0f, "ratio = %.3f");
			ImGui::SliderFloat("WaveBY", &WaveB[1], -2.0f, 2.0f, "ratio = %.3f");
			ImGui::SliderFloat("WaveBZ", &WaveB[2], 0.0f, 1.0f, "ratio = %.3f");
			ImGui::SliderFloat("WaveBW", &WaveB[3], 0.0f, 20.0f, "%.1f", ImGuiSliderFlags_Logarithmic);

			ImGui::Text("WaveC:");
			ImGui::SliderFloat("WaveCX", &WaveC[0], -2.0f, 2.0f, "ratio = %.3f");
			ImGui::SliderFloat("WaveCY", &WaveC[1], -2.0f, 2.0f, "ratio = %.3f");
			ImGui::SliderFloat("WaveCZ", &WaveC[2], 0.0f, 1.0f, "ratio = %.3f");
			ImGui::SliderFloat("WaveCW", &WaveC[3], 0.0f, 20.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
			
			ImGui::Text("WaveD:");
			ImGui::SliderFloat("WaveDX", &WaveD[0], -2.0f, 2.0f, "ratio = %.3f");
			ImGui::SliderFloat("WaveDY", &WaveD[1], -2.0f, 2.0f, "ratio = %.3f");
			ImGui::SliderFloat("WaveDZ", &WaveD[2], 0.0f, 1.0f, "ratio = %.3f");
			ImGui::SliderFloat("WaveDW", &WaveD[3], 0.0f, 20.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
			
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
#pragma endregion
		// 每帧时间
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 输入
		processInput(window);

		// 渲染
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 设置uniforms变量之前设置shader状态为可使用
		ourShader.use();

		// 视口和投影矩阵设置
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);
		ourShader.setFloat("time", currentFrame);
		//ourShader.setFloat("_Amplitude", _Amplitude);
		//ourShader.setFloat("k", k);
		//ourShader.setFloat("speed", speed);
		//ourShader.setFloat("speedness", speedness);
		ourShader.setVec4("WaveA", glm::vec4(WaveA[0], WaveA[1], WaveA[2], WaveA[3]));
		ourShader.setVec4("WaveB", glm::vec4(WaveB[0], WaveB[1], WaveB[2], WaveB[3]));
		ourShader.setVec4("WaveC", glm::vec4(WaveC[0], WaveC[1], WaveC[2], WaveC[3]));
		ourShader.setVec4("WaveD", glm::vec4(WaveD[0], WaveD[1], WaveD[2], WaveD[3]));
		ourShader.setVec3("lightPos", lightPos);
		ourShader.setVec3("viewPos", camera.Position);

		// 渲染加载的模型
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		ourShader.setMat4("model", model);
		ourModel.Draw(ourShader);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// 交换帧缓冲并且轮询IO事件
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	// 中止，并且清除先前分配的GLFW资源
	glfwTerminate();
	return 0;
}

// 键盘事件
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		triggerMouse = !triggerMouse;
	}
}

// 窗口事件回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// 确保视口匹配新的窗口维度
	glViewport(0, 0, width, height);
}

// 鼠标回调事件
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (triggerMouse)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

// 滚轮回调事件
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}