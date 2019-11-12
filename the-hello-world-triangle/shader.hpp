#ifndef SHADER_H
#define SHADER_H

#define GL_SILENCE_DEPRECATION

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

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
      cout << "error reading shader files." << endl;
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
      cout << "error compiling v shader: " << infoLog << endl;
    }

    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShaderCode, NULL);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fShader, 512, NULL, infoLog);
      cout << "error compiling f shader: " << infoLog << endl;
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
};

#endif
