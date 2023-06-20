#include <glad/glad.h>
#include <fstream>
#include <new>
#include <cstring>
#include <boxer/boxer.h>
#include "ShaderMgmt.hpp"
namespace Shaders {
    IOStatus Shader::loadFromFile(const char* path) {
        std::ifstream shaderFile(path, std::ios::binary | std::ios::in);
        if(!shaderFile.is_open())
            return IOStatus::FAIL;
        shaderFile.seekg(0, std::ios::end);
        if(shaderFile.fail()) {
            shaderFile.close();
            return IOStatus::FAIL;
        }
        std::ios::pos_type sizeOfFile = shaderFile.tellg();
        if(sizeOfFile == -1) {
            shaderFile.close();
            return IOStatus::FAIL;
        }
        shaderFile.seekg(0, std::ios::beg);
        if(shaderFile.fail()) {
            shaderFile.close();
            return IOStatus::FAIL;
        }
        std::ios::pos_type bufferSize = sizeOfFile;
        bufferSize += 1;
        do
            source = new(std::nothrow) char[bufferSize];
        while(source == nullptr);
        shaderFile.read(source, sizeOfFile);
        if(shaderFile.fail() || shaderFile.eof()) {
            shaderFile.close();
            delete[] source;
            return IOStatus::FAIL;
        }
        source[sizeOfFile] = 0;
        shaderFile.close();
        return IOStatus::OK;
    }
    bool Shader::checkForCompileErrors() const {
        GLint success;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (success != GL_FALSE)
            return false;
        GLint len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        if(len == 0)
            return true;
        #define FAILMSG "Failed to compile shader:\n"
        char* buffer;
        do
            buffer = new(std::nothrow) char[len + sizeof(FAILMSG) - 1];
        while(buffer == nullptr);
        glGetShaderInfoLog(id, len, nullptr, buffer + sizeof(FAILMSG) - 1);
        std::memcpy(buffer, FAILMSG, sizeof(FAILMSG) - 1);
        boxer::show(buffer, "Error", boxer::Style::Error);
        delete[] buffer;
        return true;
    }
    ShaderStatus MainVertexShader::load() {
        if(Shader::loadFromFile("./rsrc/shaders/main.vs") != IOStatus::OK)
            return ShaderStatus::FAIL;
        id = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(id, 1, &source, nullptr);
        glCompileShader(id);
        if(checkForCompileErrors()) {
            delete[] source;
            return ShaderStatus::FAIL;
        }
        return ShaderStatus::OK;
    }
    void MainVertexShader::free() const {
        glDeleteShader(id);
        delete[] source;
    }
    ShaderStatus MainFragmentShader::load() {
        if(Shader::loadFromFile("./rsrc/shaders/main.fs") != IOStatus::OK)
            return ShaderStatus::FAIL;
        id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(id, 1, &source, nullptr);
        glCompileShader(id);
        if(checkForCompileErrors()) {
            delete[] source;
            return ShaderStatus::FAIL;
        }
        return ShaderStatus::OK;
    }
    void MainFragmentShader::free() const {
        glDeleteShader(id);
        delete[] source;
    }
    bool MainShaderProgram::checkForLinkingErrors() const {
        GLint success;
        glGetShaderiv(id, GL_LINK_STATUS, &success);
        if (success != GL_FALSE)
            return false;
        GLint len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        if(len == 0)
            return true;
        #undef FAILMSG
        #define FAILMSG "Failed to link shaders:\n"
        char* buffer;
        do
            buffer = new(std::nothrow) char[len + sizeof(FAILMSG) - 1];
        while(buffer == nullptr);
        glGetShaderInfoLog(id, len, nullptr, buffer + sizeof(FAILMSG) - 1);
        std::memcpy(buffer, FAILMSG, sizeof(FAILMSG) - 1);
        boxer::show(buffer, "Error", boxer::Style::Error);
        delete[] buffer;
        return true;
    }
    ShaderStatus MainShaderProgram::init() {
        if(vertShader.load() != ShaderStatus::OK)
            return ShaderStatus::FAIL;
        if(fragShader.load() != ShaderStatus::OK) {
            vertShader.free();
            return ShaderStatus::FAIL;
        }
        id = glCreateProgram();
        glAttachShader(id, vertShader.id);
        glAttachShader(id, fragShader.id);
        glLinkProgram(id);
        if(checkForLinkingErrors()) {
            vertShader.free();
            fragShader.free();
            return ShaderStatus::FAIL;
        }
        sizeUniformId = glGetUniformLocation(id, "size");
        boardUniformId = glGetUniformLocation(id, "board");
        // Apparently you can do this?
        // Not sure tho.
        vertShader.free();
        fragShader.free();
        return ShaderStatus::OK;
    }
}