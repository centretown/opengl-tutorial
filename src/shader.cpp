#include "shader.hpp"
#include <memory>
#include <stdio.h>
#include <string.h>

int Shader::LoadFile(const char *path, long &size,
                     std::unique_ptr<char[]> &uptr) {
  FILE *file = fopen(path, "r");
  if (file == NULL) {
    printf("unable to open shader program. '%s' %p\n", path, path);
    return SHADER_BUILD_FAIL;
  }

  fseek(file, 0, SEEK_END);
  size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *ptr = new char[size + 1];
  fread(ptr, 1, size, file);
  ptr[size] = 0;
  uptr.reset(ptr);
  fclose(file);
  return SHADER_BUILD_OK;
}

int Shader::Load() {
  long vertexLength = 0;
  long fragmentLength = 0;
  if (!LoadFile(vertexPath, vertexLength, vertexCode)) {
    return SHADER_BUILD_FAIL;
  }
  printf("read %ld bytes from %s\n", vertexLength, vertexPath);
  if (!LoadFile(fragmentPath, fragmentLength, fragmentCode)) {
    return SHADER_BUILD_FAIL;
  }
  printf("read %ld bytes from %s\n", fragmentLength, fragmentPath);
  return SHADER_BUILD_OK;
}

int Shader::Build() {
  if (!Load()) {
    status = SHADER_ERR_FILE;
    return SHADER_BUILD_FAIL;
  }

  unsigned int vertex;
  unsigned int fragment;
  int success;
  char infoLog[512];

  // compile shaders
  vertex = glCreateShader(GL_VERTEX_SHADER);
  char *ptr = vertexCode.get();
  glShaderSource(vertex, 1, &ptr, NULL);
  glCompileShader(vertex);

  // check for compile errors if any
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    printf("'%s' ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", vertexPath,
           infoLog);
    status = SHADER_ERR_COMPILE_VERTEX;
    return SHADER_BUILD_FAIL;
  };

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  ptr = fragmentCode.get();
  glShaderSource(fragment, 1, &ptr, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    printf("'%s' ERROR::SHADER::SHADER::COMPILATION_FAILED\n%s\n", fragmentPath,
           infoLog);
    status = SHADER_ERR_COMPILE_FRAGMENT;
    return SHADER_BUILD_FAIL;
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
    printf("'%s' '%s' ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", vertexPath,
           fragmentPath, infoLog);
    status = SHADER_ERR_LINK_PROGRAM;
    return SHADER_BUILD_FAIL;
  }

  // delete the shaders as they're linked into our program now and no longer
  // necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  status = SHADER_VALID;
  return SHADER_BUILD_OK;
}
