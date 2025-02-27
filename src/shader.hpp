#pragma once

#include "glad.h"
#include <stddef.h>

#define MAX_CODE_SIZE 2048

enum ShaderErrorCode {
  SHADER_VALID,
  SHADER_INVALID,
  SHADER_ERR_FILE,
  SHADER_ERR_COMPILE_VERTEX,
  SHADER_ERR_COMPILE_FRAGMENT,
  SHADER_ERR_LINK_PROGRAM,
};

struct ShaderCode {
  char vertexCode[MAX_CODE_SIZE] = {0};
  char fragmentCode[MAX_CODE_SIZE] = {0};
  const char *vertexPath = NULL;
  const char *fragmentPath = NULL;
  ShaderCode(const char *vertexPath, const char *fragmentPath)
      : vertexPath{vertexPath}, fragmentPath{fragmentPath} {}
  int Load();
};

struct Shader {
  // the program ID
  unsigned int ProgramID = (unsigned int)-1;
  ShaderErrorCode status = SHADER_INVALID;

  // constructor reads and builds the shader
  Shader(const char *vertexPath, const char *fragmentPath);
  bool IsValid() { return status == SHADER_VALID; }
  // use/activate the shader
  void use() { glUseProgram(ProgramID); }
  // utility uniform functions
  void setBool(const char *name, bool value) const {
    glUniform1i(glGetUniformLocation(ProgramID, name), (int)value);
  }
  void setInt(const char *name, int value) const {
    glUniform1i(glGetUniformLocation(ProgramID, name), value);
  }
  void setFloat(const char *name, float value) const {
    glUniform1f(glGetUniformLocation(ProgramID, name), value);
  }
};
