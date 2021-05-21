#include "pch.h"
#include <string>
#include <fstream>
#include <sstream>
#include "SWE/SWE.h"


namespace swe{

    unsigned int Shader::shaderInUse;

    Shader::Shader(const char *vertexPath, const char *fragmentPath)
    {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            // close file handlers
            vShaderFile.close();
            fShaderFile.close();

            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }

        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();

        //create shaders from code
        unsigned int vShader = createShader(GL_VERTEX_SHADER, vShaderCode);
        unsigned int fShader = createShader(GL_FRAGMENT_SHADER, fShaderCode);

        linkShader(vShader, fShader);

        // delete the shaders
        glDeleteShader(vShader);
        glDeleteShader(fShader);
    }

    void Shader::use() const
    {
        glUseProgram(ID);
        shaderInUse = ID;
    }

    bool Shader::isActive() const
    {
        return shaderInUse == ID;
    }

    void Shader::setBool(const std::string &name, bool value) const
    {
        if (isActive())
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
        else
            std::cout << "Can not change uniform of shader that is not active." << std::endl;
    }

    void Shader::setInt(const std::string &name, int value) const
    {
        if (isActive())
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        else
            std::cout << "Can not change uniform of shader that is not active." << std::endl;
    }

    void Shader::setFloat(const std::string &name, float value) const
    {
        if (isActive())
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        else
            std::cout << "Can not change uniform of shader that is not active." << std::endl;
    }

    void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
    {
        if (isActive())
            glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
        else
            std::cout << "Can not change uniform of shader that is not active." << std::endl;
    }

    void Shader::setMat4(const std::string &name, glm::mat4 matrix) const
    {
        if (isActive())
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
        else
            std::cout << "Can not change uniform of shader that is not active." << std::endl;
    }

    unsigned int Shader::createShader(unsigned int type, const char *code)
    {
        unsigned int shader = glCreateShader(type);
        //check errors

        //compile shader
        glShaderSource(shader, 1, &code, NULL);
        glCompileShader(shader);

        //compilation error checking
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            if (type == GL_VERTEX_SHADER)
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                        << infoLog << std::endl;
            else if (type == GL_FRAGMENT_SHADER)
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                        << infoLog << std::endl;
            else
                std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n"
                        << infoLog << std::endl;
        }
        return shader;
    }

    void Shader::linkShader(unsigned int vShader, unsigned int fShader)
    {
        //create shader program
        ID = glCreateProgram();
        glAttachShader(ID, vShader);
        glAttachShader(ID, fShader);
        glLinkProgram(ID);

        //linking error checking
        int success;
        char infoLog[512];
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                    << infoLog << std::endl;
        }
    }
}