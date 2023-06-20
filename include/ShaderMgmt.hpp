#pragma once
#ifndef KNC_SHADERMGMT_HPP
#define KNC_SHADERMGMT_HPP
#include <glad/glad.h>
namespace Shaders {
    enum struct IOStatus {
        OK,
        FAIL
    };
    enum struct ShaderStatus {
        OK,
        FAIL
    };
    struct Shader {
        GLuint id;
        char* source;
        IOStatus loadFromFile(const char* path);
        bool checkForCompileErrors() const;
    };
    struct MainVertexShader : public Shader {
        ShaderStatus load();
        void free() const;
    };
    struct MainFragmentShader : public Shader {
        
        ShaderStatus load();
        void free() const;
    };
    struct MainShaderProgram {
        GLuint id;
        GLint sizeUniformId;
        GLint boardUniformId;
        MainVertexShader vertShader;
        MainFragmentShader fragShader;
        bool checkForLinkingErrors() const;
        ShaderStatus init();
    };
}
#endif