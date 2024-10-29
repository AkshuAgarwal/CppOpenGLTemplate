#include "shader.h"

std::string readShaderFile(const char *file) {
    std::ifstream f(file);
    std::string str;

    if (!f.is_open()) {
        std::cout << "Error opening file: " + std::string(file) << "\n";
        return 0;
    }

    f.seekg(0, std::ios::end);
    str.reserve(f.tellg());
    f.seekg(0, std::ios::beg);

    if (str.capacity() <= 0) {
        std::cout << "Error opening file: " + std::string(file) << "\n";
        return 0;
    }

    str.assign(
        (std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>()
    );

    return str;
}

Shader::Shader(GLenum type) : type(type){};

int Shader::createFromData(const char *data, const char *path) {
    GLuint shader_id = glCreateShader(this->type);
    glShaderSource(shader_id, 1, &data, nullptr);
    glCompileShader(shader_id);

    GLint res = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &res);

    if (!res) {
        char *msg = 0;
        int l = 0;

        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &l);

        if (l) {
            msg = new char[l];
            glGetShaderInfoLog(shader_id, l, &l, msg);
            msg[l - 1] = 0;

            std::cout << "Error compiling shader: ";
            if (path) {
                std::cout << path;
            }
            std::cout << '\n' << msg << '\n';
            delete[] msg;
        } else {
            if (path) {
                std::cout << path << " ";
            }
            std::cout << "Unknown error while compiling shader" << '\n';
        }

        glDeleteShader(shader_id);
        return 0;
    }

    this->id = shader_id;
    return 1;
}

int Shader::createFromFile(const char *file) {
    std::string data = readShaderFile(file);
    return this->createFromData(data.c_str(), file);
}

ShaderProgram::ShaderProgram() { this->id = glCreateProgram(); }

int ShaderProgram::_checkLinkStatus() {
    GLint info = 0;
    glGetProgramiv(this->id, GL_LINK_STATUS, &info);

    if (info != GL_TRUE) {
        char *msg = 0;
        int l = 0;

        glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &l);

        msg = new char[l];
        glGetProgramInfoLog(this->id, l, &l, msg);

        std::cout << std::string("Link error: ") + msg << "\n";

        delete[] msg;
        glDeleteProgram(this->id);
        this->id = 0;

        return 0;
    }

    return 1;
}

int ShaderProgram::loadFromData(
    const char *vertex_shader_data, const char *fragment_shader_data,
    const char *geometry_shader_data
) {
    Shader *vertex_shader, *fragment_shader, *geometry_shader;
    GLuint vertex_id = 0, fragment_id = 0, geometry_id = 0;

    vertex_shader = new Shader(GL_VERTEX_SHADER);
    vertex_shader->createFromData(vertex_shader_data);
    vertex_id = vertex_shader->id;

    fragment_shader = new Shader(GL_FRAGMENT_SHADER);
    fragment_shader->createFromData(fragment_shader_data);
    fragment_id = fragment_shader->id;

    if (geometry_shader_data) {
        geometry_shader = new Shader(GL_GEOMETRY_SHADER);
        geometry_shader->createFromData(geometry_shader_data);
        geometry_id = geometry_shader->id;
    }

    if (vertex_id == 0 || fragment_id == 0 ||
        (geometry_shader_data && geometry_id == 0)) {
        return 0;
    }

    glAttachShader(this->id, vertex_id);
    glAttachShader(this->id, fragment_id);
    if (geometry_shader_data)
        glAttachShader(this->id, geometry_id);

    glLinkProgram(this->id);

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);
    if (geometry_shader_data)
        glDeleteShader(fragment_id);

    if (!this->_checkLinkStatus())
        return 0;

    glValidateProgram(this->id);
    return 1;
}

int ShaderProgram::loadFromFile(
    const char *vertex_shader_file, const char *fragment_shader_file,
    const char *geometry_shader_file
) {
    std::string vertex_shader_data, fragment_shader_data, geometry_shader_data;

    vertex_shader_data = readShaderFile(vertex_shader_file);
    fragment_shader_data = readShaderFile(fragment_shader_file);

    if (!geometry_shader_file) {
        return this->loadFromData(
            vertex_shader_data.c_str(), fragment_shader_data.c_str(), 0
        );
    } else {
        geometry_shader_data = readShaderFile(geometry_shader_file);

        return this->loadFromData(
            vertex_shader_data.c_str(), fragment_shader_data.c_str(),
            geometry_shader_data.c_str()
        );
    }
}

void ShaderProgram::bind() const { glUseProgram(this->id); }

void ShaderProgram::clear() {
    glDeleteProgram(this->id);
    this->id = 0;
}

GLint ShaderProgram::getUniform(const char *name) const {
    GLint uniform = glGetUniformLocation(this->id, name);
    if (uniform == -1) {
        std::cout << "uniform error " + std::string(name);
    }

    return uniform;
}
