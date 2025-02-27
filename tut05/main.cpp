#include "../src/shader.hpp"
#include "../src/texture.hpp"
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec4 calc() {
  glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
  glm::mat4 trans = glm::mat4(1.0f);

  trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
  vec = trans * vec;

  printf("x=%.3f, y=%.3f, z=%.3f\n", vec.x, vec.y, vec.z);
  return vec;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

float mixValue = 0.5f;

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    mixValue += 0.005f; // change this value accordingly (might be too slow or
                        // too fast based on system hardware)
    if (mixValue >= 1.0f)
      mixValue = 1.0f;
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    mixValue -= 0.005f; // change this value accordingly (might be too slow or
                        // too fast based on system hardware)
    if (mixValue <= 0.0f)
      mixValue = 0.0f;
  }
}

ShaderCode shaderCode("assets/shaders/gls330/shader.vert",
                      "assets/shaders/gls330/shader.frag");

int main(int argc, char **argv) {

  int status = shaderCode.Load();
  if (!status) {
    return -1;
  }

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    status = -1;
    return status;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    status = -1;
    glfwTerminate();
    return status;
  }

  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  Shader shader(shaderCode.vertexCode, shaderCode.fragmentCode);
  if (!shader.IsValid()) {
    return -1;
  }

  float vertices[] = {
      // positions          // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
  };
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  TextureOptions opt1;
  unsigned int texture1 = MakeTexture("assets/textures/container.jpg", &opt1);
  TextureOptions opt2 = {.flip = true};
  unsigned int texture2 = MakeTexture("assets/textures/awesomeface.png", &opt2);

  // don't forget to activate the shader before setting uniforms!
  shader.use();
  // set it manually
  // glUniform1i(glGetUniformLocation(shader.ProgramID, "texture1"), 0);
  // set with shader class
  shader.setInt("texture1", 0);
  shader.setInt("texture2", 1);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.setFloat("mixValue", mixValue);
    // bind Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // create transformations
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
    transform = glm::rotate(transform, (float)glfwGetTime(),
                            glm::vec3(0.0f, 0.0f, 1.0f));

    shader.use();
    unsigned int transformLoc =
        glGetUniformLocation(shader.ProgramID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glfwTerminate();
  return 0;
}

// float vertices[] = {
//     // positions          // colors           // texture coords
//     0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
//     0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
//     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
//     -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
// };

// // position attribute
// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void
// *)0); glEnableVertexAttribArray(0);
// // color attribute
// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
//                       (void *)(3 * sizeof(float)));
// glEnableVertexAttribArray(1);
// // texture coord attribute
// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
//                       (void *)(6 * sizeof(float)));
// glEnableVertexAttribArray(2);
