#include "shader.hpp"
#include <stdio.h>

int ShaderCode::Load() {
  FILE *vertexFile = fopen(vertexPath, "r");
  if (vertexFile == NULL) {
    printf("unable to open vertex program. '%s' %p\n", vertexPath, vertexFile);
    return 0;
  }
  int count = fread(vertexCode, 1, sizeof(vertexCode), vertexFile);
  fclose(vertexFile);
  vertexCode[count] = 0;
  printf("read %d bytes from %s\n", count, vertexPath);
  // printf("\n%s\n\n", vertexCode);

  FILE *fragmentFile = fopen(fragmentPath, "r");
  if (fragmentFile == NULL) {
    printf("unable to open fragment program. '%s'\n", fragmentPath);
    return 0;
  }
  count = fread(fragmentCode, 1, sizeof(fragmentCode), fragmentFile);
  fclose(fragmentFile);
  fragmentCode[count] = 0;
  printf("read %d bytes from %s\n", count, fragmentPath);
  // printf("\n%s\n\n", fragmentCode);

  return 1;
}

Shader::Shader(const char *vShaderCode, const char *fShaderCode) {
  unsigned int vertex;
  unsigned int fragment;
  int success;
  char infoLog[512];

  // compile shaders
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);

  // check for compile errors if any
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    status = SHADER_ERR_COMPILE_VERTEX;
    return;
  };

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    printf("ERROR::SHADER::SHADER::COMPILATION_FAILED\n%s\n", infoLog);
    status = SHADER_ERR_COMPILE_FRAGMENT;
    return;
  }

  // shader Program
  ProgramID = glCreateProgram();
  glAttachShader(ProgramID, vertex);
  glAttachShader(ProgramID, fragment);
  glLinkProgram(ProgramID);
  // print linking errors if any
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ProgramID, 512, NULL, infoLog);
    printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    status = SHADER_ERR_LINK_PROGRAM;
    return;
  }

  // delete the shaders as they're linked into our program now and no longer
  // necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  status = SHADER_VALID;
}
