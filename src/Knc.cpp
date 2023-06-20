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
		const u8 idx = y * 3 + x;
		u8* cur = &buffer[idx];
		const u8 type = *cur;
		if(type != 0u)
			return MarkStatus::BLOCKED;
		*cur = static_cast<u8>(markType);
		/*
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
				if(cur[-3] == type && cur[3] == type)
					return MarkStatus::WIN;
				break;
			case ctIdx(2, 0):
				if(cur[-1] == type && cur[-2] == type)
					return MarkStatus::WIN;
				if(cur[3] == type && cur[6] == type)
					return MarkStatus::WIN;
				if(cur[4] == type && cur[8] == type)
					return MarkStatus::WIN;
		}
		*/
		return MarkStatus::OK;
	}
};

Board board;

namespace Window {
	Shaders::MainShaderProgram mainShaderProgram;
	static void processInput(GLFWwindow *window) {
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	static void onFrameBufferSizeChanged(GLFWwindow* window, int width, int height) {
		UNUSED(window);
		glViewport(0, 0, width, height);
		glUniform2f(mainShaderProgram.sizeUniformId, static_cast<float>(width), static_cast<float>(height));
	}

	static bool mainLoop(GLFWwindow* window) {
		processInput(window);
		glClearColor(1.f, 1.f, 1.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		glfwPollEvents();
		return glfwWindowShouldClose(window) == 0;
	}

	static WININITSTATUS init() {
		srand(99);
		glfwInit();
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
		GLFWwindow* win = glfwCreateWindow(Consts::WindowSize.x, Consts::WindowSize.y, "Knots and Crosses", nullptr, nullptr);
		if (win == nullptr) {
			glfwTerminate();
			return WININITSTATUS::GLFW_WIN_FAIL;
		}
		glfwMakeContextCurrent(win);
		glfwSetFramebufferSizeCallback(win, onFrameBufferSizeChanged);
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
		}
		glUniform1i(mainShaderProgram.boardUniformId, 0);
		GLuint boardTexture;
		glGenTextures(1, &boardTexture);
		glBindTexture(GL_TEXTURE_2D, boardTexture);
		board.init();
		if(true) //This breaks?????
			for(int x = 0; x < 3; ++x)
				for(int y = 0; y < 3; ++y)
					board.markPos(x, y, (rand() & 1) ? MarkType::X : MarkType::O);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8I, 3, 3, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, board.data());
		board.print(); //This works fine now?? why.
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