#include "pch.h"
#include <glad/glad.h>
#include <GLM/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <SWE/Components/shader.h>
#include <SWE/Components/transform.h>
#include <SWE/Engine/error.h>

#define VSPATH "shaders/default.vs"
#define FSPATH "shaders/default.fs"
#define GSPATH "shaders/default.gs"
#define LFSPATH "shaders/light.fs"

namespace swe{

    uint32_t Shader::shaderInUse = 0;
    Shader Shader::defaultShader = Shader(0);
    Shader Shader::lightShader = Shader(0);

    Shader::Shader(uint32_t ID) : ID(ID) {}

    Shader Shader::fromFile(const char *vertexPath, const char *fragmentPath, const char* geometryPath)
    {
        std::string vertexCode;
        std::string geometryCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream gShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
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

            if (geometryPath != nullptr)
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;

                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
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
        
        unsigned int id;
        if (geometryPath != nullptr)
        {
            const char* gShaderCode = geometryCode.c_str();
            unsigned int gShader = createShader(GL_GEOMETRY_SHADER, gShaderCode);
            id = linkShader(vShader, fShader, gShader);
        }
        else
            id = linkShader(vShader, fShader);

        // delete the shaders
        glDeleteShader(vShader);
        glDeleteShader(fShader);

        return Shader(id);
    }

    Shader Shader::fromString(const char *vertexCode, const char *fragmentCode, const char* geometryCode)
    {
        //create shaders from code
        unsigned int vShader = createShader(GL_VERTEX_SHADER, vertexCode);
        unsigned int fShader = createShader(GL_FRAGMENT_SHADER, fragmentCode);

        unsigned int gShader;
        unsigned int id;
        if (geometryCode != nullptr)
        {
            gShader = createShader(GL_GEOMETRY_SHADER, geometryCode);
            id = linkShader(vShader, fShader, gShader);
            glDeleteShader(gShader);
        }
        else
            id = linkShader(vShader, fShader);

        // delete the shaders
        glDeleteShader(vShader);
        glDeleteShader(fShader);

        return Shader(id);
    }

    Shader Shader::getDefaultShader()
    {
        if (defaultShader.ID == 0)
            defaultShader = fromFile(VSPATH, FSPATH, GSPATH);
        
        return defaultShader;
    }

    Shader Shader::getLightShader()
    {
        if (lightShader.ID == 0)
            lightShader = fromFile(VSPATH, LFSPATH, GSPATH);

        return lightShader;
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
            Error err = Error("Can not change uniform of shader that is not active.", errorLevel::Error, __SOURCELOCATION__);
    }

    void Shader::setInt(const std::string &name, int value) const
    {
        if (isActive())
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        else
            Error err = Error("Can not change uniform of shader that is not active.", errorLevel::Error, __SOURCELOCATION__);
    }

    void Shader::setFloat(const std::string &name, float value) const
    {
        if (isActive())
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        else
            Error err = Error("Can not change uniform of shader that is not active.", errorLevel::Error, __SOURCELOCATION__);
    }

    void Shader::setVec3(const std::string& name, glm::vec3 vec) const
    {
        if (isActive())
            glUniform3f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
        else
            Error err = Error("Can not change uniform of shader that is not active.", errorLevel::Error, __SOURCELOCATION__);
    }

    void Shader::setVec3(const std::string& name, float x, float y, float z) const
    {
        if (isActive())
            glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
        else
            Error err = Error("Can not change uniform of shader that is not active.", errorLevel::Error, __SOURCELOCATION__);
    }

    void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
    {
        if (isActive())
            glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
        else
            Error err = Error("Can not change uniform of shader that is not active.", errorLevel::Error, __SOURCELOCATION__);
    }

    void Shader::setMat4(const std::string &name, glm::mat4 matrix) const
    {
        if (isActive())
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
        else
            Error err = Error("Can not change uniform of shader that is not active.", errorLevel::Error, __SOURCELOCATION__);
    }

    void Shader::setDLight(const std::string& name, DirectionalLight* dl) const
    {
        if (isActive())
        {
            
            glUniform3f(glGetUniformLocation(ID, (name+".direction").c_str()), dl->direction->x, dl->direction->y, dl->direction->z);

            glUniform3f(glGetUniformLocation(ID, (name + ".ambient").c_str()), dl->ambient->x*dl->color->x, dl->ambient->y*dl->color->y, dl->ambient->z*dl->color->z);
            glUniform3f(glGetUniformLocation(ID, (name + ".diffuse").c_str()), dl->diffuse->x*dl->color->x, dl->diffuse->y*dl->color->y, dl->diffuse->z*dl->color->z);
            glUniform3f(glGetUniformLocation(ID, (name + ".specular").c_str()), dl->specular->x*dl->color->x, dl->specular->y*dl->color->y, dl->specular->z*dl->color->z);
        }
        else
            Error err = Error("Can not change uniform of shader that is not active.", errorLevel::Error, __SOURCELOCATION__);
    }

    void Shader::setPLight(const std::string& name, PointLight* pl) const
    {
        if (isActive())
        {
            transform_ptr t = pl->parent->getComponent<Transform>();
            glUniform3f(glGetUniformLocation(ID, (name + ".position").c_str()), t->position->x, t->position->y, t->position->z);

            glUniform1f(glGetUniformLocation(ID, (name + ".quadratic").c_str()), pl->quadratic);
            glUniform1f(glGetUniformLocation(ID, (name + ".linear").c_str()), pl->linear);
            glUniform1f(glGetUniformLocation(ID, (name + ".constant").c_str()), pl->constant);

            glUniform3f(glGetUniformLocation(ID, (name + ".ambient").c_str()), pl->ambient->x * pl->color->x, pl->ambient->y * pl->color->y, pl->ambient->z * pl->color->z);
            glUniform3f(glGetUniformLocation(ID, (name + ".diffuse").c_str()), pl->diffuse->x * pl->color->x, pl->diffuse->y * pl->color->y, pl->diffuse->z * pl->color->z);
            glUniform3f(glGetUniformLocation(ID, (name + ".specular").c_str()), pl->specular->x * pl->color->x, pl->specular->y * pl->color->y, pl->specular->z * pl->color->z);
        }
        else
            Error err = Error("Can not change uniform of shader that is not active.", errorLevel::Error, __SOURCELOCATION__);
    }

    void Shader::setSLight(const std::string& name, SpotLight* sl) const
    {
        if (isActive())
        {
            transform_ptr t = sl->parent->getComponent<Transform>();
            glUniform3f(glGetUniformLocation(ID, (name + ".position").c_str()), t->position->x, t->position->y, t->position->z);

            glUniform1f(glGetUniformLocation(ID, (name + ".innerCutOff").c_str()), sl->innerCutOff);
            glUniform1f(glGetUniformLocation(ID, (name + ".outerCutOff").c_str()), sl->outerCutOff);
            glUniform3f(glGetUniformLocation(ID, (name + ".direction").c_str()), sl->direction->x, sl->direction->y, sl->direction->z);

            glUniform3f(glGetUniformLocation(ID, (name + ".ambient").c_str()), sl->ambient->x * sl->color->x, sl->ambient->y * sl->color->y, sl->ambient->z * sl->color->z);
            glUniform3f(glGetUniformLocation(ID, (name + ".diffuse").c_str()), sl->diffuse->x * sl->color->x, sl->diffuse->y * sl->color->y, sl->diffuse->z * sl->color->z);
            glUniform3f(glGetUniformLocation(ID, (name + ".specular").c_str()), sl->specular->x * sl->color->x, sl->specular->y * sl->color->y, sl->specular->z * sl->color->z);
        }
        else
            Error err = Error("Can not change uniform of shader that is not active.", errorLevel::Error, __SOURCELOCATION__);
    }

    unsigned int Shader::createShader(unsigned int type, const char *code)
    {
        unsigned int shader = glCreateShader(type);

        if (shader == NULL)
            std::cout << "Glad may not be initialized for this thread." << std::endl;

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

    unsigned int Shader::linkShader(unsigned int vShader, unsigned int fShader, unsigned int gShader)
    {

        //create shader program
        unsigned int id = glCreateProgram();

        glAttachShader(id, vShader);
        glAttachShader(id, gShader);
        glAttachShader(id, fShader);
        glLinkProgram(id);

        //linking error checking
        int success;
        char infoLog[512];
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                << infoLog << std::endl;
        }

        return id;
    }

    unsigned int Shader::linkShader(unsigned int vShader, unsigned int fShader)
    {

        //create shader program
        unsigned int id = glCreateProgram();

        glAttachShader(id, vShader);
        glAttachShader(id, fShader);
        glLinkProgram(id);

        //linking error checking
        int success;
        char infoLog[512];
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                    << infoLog << std::endl;
        }

        return id;
    } 
}