#pragma once
#include <fstream>
#include <glad/glad.h>
#include <iostream>

std::string readShaderFile(const char *file);

class Shader {
  public:
    GLuint id = 0;
    GLenum type = 0;

    Shader(GLenum type);

    int createFromData(const char *data, const char *path = 0);
    int createFromFile(const char *file);
};

class ShaderProgram {
  public:
    GLuint id = 0;

    ShaderProgram();

    int loadFromData(
        const char *vertex_shader_data, const char *fragment_shader_data, const char *geometry_shader_data = 0
    );
    int loadFromFile(
        const char *vertex_shader_file, const char *fragment_shader_file, const char *geometry_shader_file = 0
    );

    void bind() const;
    void clear();

    GLint getUniform(const char *name) const;

  private:
    int _checkLinkStatus();
};
