#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <array>
#include <boxer/boxer.h>
#include "config.hpp"
#include "Settings.hpp"
#include "ShaderMgmt.hpp"

#define UNUSED(expr) (void)(expr);

enum struct WININITSTATUS {
	OK,
	GLFW_WIN_FAIL,
	GLAD_LOAD_FAIL,
	SHADER_FAIL
};

enum struct MarkType : u8 {
	X = 1,
	O = 2
};

enum struct MarkStatus {
	BLOCKED,
	OK,
	WIN
};

struct Board {
	std::array<u8, 9> buffer;
	inline u8* data() {
		return buffer.data();
	}
	inline void clear() {
		buffer.fill(0u);
	}
	inline void init() {
		clear();
	}
	void print() {
		u8 idx = 0;
		for(u8 val : buffer) {
			switch(val) {
				case 1:
					std::cout << 'X';
					break;
				case 2:
					std::cout << 'O';
					break;
				default:
					std::cout << '#';
					break;
			}
			if(++idx == 3u) {
				idx = 0;
				std::cout << '\n';
			}
		}
	}
	static inline consteval u8 ctIdx(u8 x, u8 y) {
		return y * 3 + x;
	}
	inline u8& at(u8 x, u8 y) {
		return buffer[y * 3 + x];
	}
	MarkStatus markPos(u8 x, u8 y, MarkType markType) {
		const u8 idx = (y * 3) + x;
		u8* cur = &buffer[idx];
		u8 type = *cur;
		if(type != 0u)
			return MarkStatus::BLOCKED;
		type = static_cast<u8>(markType);
		*cur = type;
		switch(idx) {
			case ctIdx(0, 0):
				if(cur[1] == type && cur[2] == type)
					return MarkStatus::WIN;
				if(cur[3] == type && cur[6] == type)
					return MarkStatus::WIN;
				if(cur[4] == type && cur[8] == type)
					return MarkStatus::WIN;
				break;
			case ctIdx(1, 0):
				if(cur[-1] == type && cur[1] == type)
					return MarkStatus::WIN;
				if(cur[3] == type && cur[6] == type)
					return MarkStatus::WIN;
				break;
			case ctIdx(2, 0):
				if(cur[-1] == type && cur[-2] == type)
					return MarkStatus::WIN;
				if(cur[3] == type && cur[6] == type)
					return MarkStatus::WIN;
				if(cur[2] == type && cur[4] == type)
					return MarkStatus::WIN;
				break;
			case ctIdx(0, 1):
				if(cur[1] == type && cur[2] == type)
					return MarkStatus::WIN;
				if(cur[-3] == type && cur[3] == type)
					return MarkStatus::WIN;
				break;
			case ctIdx(1, 1):
				if(cur[-1] == type && cur[1] == type)
					return MarkStatus::WIN;
				if(cur[-3] == type && cur[3] == type)
					return MarkStatus::WIN;
				if(cur[-4] == type && cur[4] == type)
					return MarkStatus::WIN;
				if(cur[-2] == type && cur[2] == type)
					return MarkStatus::WIN;
				break;
			case ctIdx(2, 1):
				if(cur[-2] == type && cur[-1] == type)
					return MarkStatus::WIN;
				if(cur[-3] == type && cur[3] == type)
					return MarkStatus::WIN;
				break;
			case ctIdx(0, 2):
				if(cur[1] == type && cur[2] == type)
					return MarkStatus::WIN;
				if(cur[-3] == type && cur[-6] == type)
					return MarkStatus::WIN;
				if(cur[-2] == type && cur[-4] == type)
					return MarkStatus::WIN;
				break;
			case ctIdx(1, 2):
				if(cur[-1] == type && cur[1] == type)
					return MarkStatus::WIN;
				if(cur[-3] == type && cur[-6] == type)
					return MarkStatus::WIN;
				break;
			case ctIdx(2, 2):
				if(cur[-1] == type && cur[-2] == type)
					return MarkStatus::WIN;
				if(cur[-3] == type && cur[-6] == type)
					return MarkStatus::WIN;
				if(cur[-4] == type && cur[-8] == type)
					return MarkStatus::WIN;
			default:
				break;
		}
		return MarkStatus::OK;
	}
};

Board board;
MarkType curMarkType = MarkType::X;

namespace Window {
	Vec2u16 size;
	Shaders::MainShaderProgram mainShaderProgram;
	static void processInput(GLFWwindow *window) {
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		if(glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
			board.clear();
			glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 9, GL_RED_INTEGER, GL_UNSIGNED_BYTE, board.data());
		}
	}
	static void render(GLFWwindow* window);

	static void onMouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		UNUSED(mods);
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			double mouseX;
			double mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			mouseX = (15*(2*mouseX-size.x+(0.7*static_cast<double>(size.y))))/(7*size.y);
			mouseY = ((mouseY/(float)size.y)-0.15f)*3.f/0.7f;
			if(mouseX > 0 && mouseX < 3 && mouseY > 0 && mouseY < 3) {
				switch(board.markPos(static_cast<u8>(mouseX), 2-static_cast<u8>(mouseY), curMarkType)) {
					case MarkStatus::WIN:
						glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 9, GL_RED_INTEGER, GL_UNSIGNED_BYTE, board.data());
						render(window);
						boxer::show(curMarkType == MarkType::X ? "X wins!" : "O wins!", "Congratulations", boxer::Style::Info);
						curMarkType = (rand() & 1) ? MarkType::O : MarkType::X;
						board.clear();
						glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 9, GL_RED_INTEGER, GL_UNSIGNED_BYTE, board.data());
						break;
					case MarkStatus::OK:
						curMarkType = (curMarkType == MarkType::X) ? MarkType::O : MarkType::X;
						glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 9, GL_RED_INTEGER, GL_UNSIGNED_BYTE, board.data());
						break;
					case MarkStatus::BLOCKED:
						break;
				}
			}
		}
	}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	static void onFrameBufferSizeChanged(GLFWwindow* window, int width, int height) {
		UNUSED(window);
		glViewport(0, 0, width, height);
		glUniform2f(mainShaderProgram.sizeUniformId, static_cast<float>(width), static_cast<float>(height));
		size = {static_cast<u16>(width), static_cast<u16>(height)};
	}

	static void render(GLFWwindow* window) {
		glClearColor(1.f, 1.f, 1.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
	}
	static bool mainLoop(GLFWwindow* window) {
		processInput(window);
		render(window);
		glfwWaitEvents();
		return glfwWindowShouldClose(window) == 0;
	}

	static WININITSTATUS init() {
		//srand(time(NULL));
		glfwInit();
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
		GLFWwindow* win = glfwCreateWindow(Consts::WindowSize.x, Consts::WindowSize.y, "Knots and Crosses", nullptr, nullptr);
		if (win == nullptr) {
			glfwTerminate();
			return WININITSTATUS::GLFW_WIN_FAIL;
		}
		glfwMakeContextCurrent(win);
		glfwSetFramebufferSizeCallback(win, onFrameBufferSizeChanged);
		glfwSetMouseButtonCallback(win, onMouseButton);
		if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0)
			return WININITSTATUS::GLAD_LOAD_FAIL;
		if(mainShaderProgram.init() != Shaders::ShaderStatus::OK)
			return WININITSTATUS::SHADER_FAIL;
		glUseProgram(mainShaderProgram.id);
		{
			int width;
			int height;
			glfwGetFramebufferSize(win, &width, &height);
			glUniform2f(mainShaderProgram.sizeUniformId, static_cast<float>(width), static_cast<float>(height));
			size = {static_cast<u16>(width), static_cast<u16>(height)};
		}
		glUniform1i(mainShaderProgram.boardUniformId, 0);
		GLuint boardTexture;
		glGenTextures(1, &boardTexture);
		glBindTexture(GL_TEXTURE_1D, boardTexture);
		board.init();
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R8I, 9, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, board.data());
		GLuint emptyVAO;
		glGenVertexArrays(1, &emptyVAO);
		glBindVertexArray(emptyVAO);
		if(Settings::loadSettings() != Settings::Status::OK)
			boxer::show("Failed to load options.", "Warning", boxer::Style::Warning);
		glfwShowWindow(win);
		while(mainLoop(win));
		glfwTerminate();
		if(Settings::saveSettings() != Settings::Status::OK)
			boxer::show("Failed to save options.", "Warning", boxer::Style::Warning);
		return WININITSTATUS::OK;
	}
};

int main() {
	switch(Window::init()) {
		case WININITSTATUS::GLFW_WIN_FAIL:
			boxer::show("Failed to create GLFW window.", "Error", boxer::Style::Error);
			return -1;
		case WININITSTATUS::GLAD_LOAD_FAIL:
			boxer::show("Failed to load glad.", "Error", boxer::Style::Error);
			return -1;
		case WININITSTATUS::SHADER_FAIL:
			boxer::show("Failed to load shaders.", "Error", boxer::Style::Error);
			return -1;
		default:
			return 0;
	}
}