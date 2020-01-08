#ifndef SHADER_H
#define SHADER_H

#define GL_SILENCE_DEPRECATION

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class Shader {
  public:
  unsigned int ID;

  Shader(const GLchar* vShaderFilePath, const GLchar* fShaderFilePath)
  {
    string vSrcContent, fSrcContent;
    ifstream vShaderFile, fShaderFile;
    vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    try {
      vShaderFile.open(vShaderFilePath);
      fShaderFile.open(fShaderFilePath);
      stringstream vShaderStream, fShaderStream;
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();
      vShaderFile.close();
      fShaderFile.close();
      vSrcContent = vShaderStream.str();
      fSrcContent = fShaderStream.str();
    } catch (ifstream::failure e) {
      cout << "error reading shader files." << vShaderFilePath << " " << fShaderFilePath << endl;
    }
    const char* vShaderCode = vSrcContent.c_str();
    const char* fShaderCode = fSrcContent.c_str();

    GLuint vShader, fShader;
    int success;
    char infoLog[512];

    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vShaderCode, nullptr);
    glCompileShader(vShader);
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vShader, 512, NULL, infoLog);
      cout << "error compiling v shader: " << vShaderFilePath << infoLog << endl;
    }

    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShaderCode, NULL);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fShader, 512, NULL, infoLog);
      cout << "error compiling f shader: " << fShaderFilePath << infoLog << endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vShader);
    glAttachShader(ID, fShader);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(ID, 512, NULL, infoLog);
      cout << "error linking program:\n " << infoLog << endl;
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);
  }

  void use() { glUseProgram(ID); }

  void setBool(const string& name, bool value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); }
  void setInt(const string& name, bool value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), value); }
  void setFloat(const string& name, float value) const { glUniform1f(glGetUniformLocation(ID, name.c_str()), value); }
  void set4Float(const string& name, float f1, float f2, float f3, float f4) const { glUniform4f(glGetUniformLocation(ID, name.c_str()), f1, f2, f3, f4); }
  void setMat4(const string& name, glm::mat4 mat) const { glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
  void setVec3(const string& name, glm::vec3 v) const { glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(v)); }
  void setVec3(const string& name, float x, float y, float z) const { glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); }
};

#endif
