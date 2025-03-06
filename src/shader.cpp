#include "shader.hpp"
#include <memory>
#include <stdio.h>
#include <string.h>

void ShaderCode::LoadFile(const char *path, long &size,
                          std::unique_ptr<char[]> &uptr) {
  FILE *file = fopen(path, "r");
  if (file == NULL) {
    printf("unable to open shader program. '%s' %p\n", path, path);
    return;
  }

  fseek(file, 0, SEEK_END);
  size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *ptr = new char[size + 1];
  fread(ptr, 1, size, file);
  ptr[size] = 0;
  uptr.reset(ptr);
  fclose(file);
}

int ShaderCode::Load() {
  long vertexLength = 0;
  long fragmentLength = 0;
  LoadFile(vertexPath, vertexLength, vertexCode);
  LoadFile(fragmentPath, fragmentLength, fragmentCode);
  printf("read %ld bytes from %s\n", vertexLength, vertexPath);
  printf("read %ld bytes from %s\n", fragmentLength, fragmentPath);
  return 1;
}

Shader::Shader(ShaderCode *code) : code{code} {
  unsigned int vertex;
  unsigned int fragment;
  int success;
  char infoLog[512];

  // compile shaders
  vertex = glCreateShader(GL_VERTEX_SHADER);
  char *ptr = code->vertexCode.get();
  glShaderSource(vertex, 1, &ptr, NULL);
  glCompileShader(vertex);

  // check for compile errors if any
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    printf("'%s' ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n",
           code->vertexPath, infoLog);
    status = SHADER_ERR_COMPILE_VERTEX;
    return;
  };

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  ptr = code->fragmentCode.get();
  glShaderSource(fragment, 1, &ptr, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    printf("'%s' ERROR::SHADER::SHADER::COMPILATION_FAILED\n%s\n",
           code->fragmentPath, infoLog);
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
    printf("'%s' '%s' ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n",
           code->vertexPath, code->fragmentPath, infoLog);
    status = SHADER_ERR_LINK_PROGRAM;
    return;
  }

  // delete the shaders as they're linked into our program now and no longer
  // necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  status = SHADER_VALID;
}
