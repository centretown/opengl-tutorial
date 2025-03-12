#include "glad.h"
#include <GLFW/glfw3.h>

#include <cstring>
#include <glm/ext/matrix_transform.hpp>
#include <map>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <vector>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "win.hpp"

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

// #define MAX_ANGLE 180.0f
// #define MIN_ANGLE 15.0f
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 1024
float windowWidth = (float)SCREEN_WIDTH;
float windowHeight = (float)SCREEN_HEIGHT;

void InitializeLights(Shader &targetShader, Camera &camera);
void UpdateLights(Shader &targetShader, Camera &camera);
// void DrawContainers(float scale);
void DrawContainers(float scale, Shader &shader, unsigned int texture, int VAO);
// camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
// float lastX = SCREEN_WIDTH / 2.0f;
// float lastY = SCREEN_HEIGHT / 2.0f;
// bool firstMouse = true;

// #define USE_OPEN_GLES
#if defined(USE_OPEN_GLES)
#define GLSL_VERSION 100
#elif defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif // USE_OPEN_GLES

#ifdef USE_OPEN_GLES
Shader curShader("assets/shaders/gls100/depth.vert",
                 "assets/shaders/gls100/depth.frag");
#else
Shader curShader("assets/shaders/gls330/depth.vert",
                 "assets/shaders/gls330/depth.frag");
#endif

// float rotation_angle = 0.0f;

int main(int argc, char **argv) {

  GLFWwindow *window = InitWindow(&camera, SCREEN_WIDTH, SCREEN_HEIGHT);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    return 1;
  }

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    glfwTerminate();
    return -1;
  }

  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CW);

  curShader.Build();
  if (!curShader.IsValid()) {
    glfwTerminate();
    return -1;
  }
  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float cubeVertices[] = {
      // positions          // texture Coords
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};
  float planeVertices[] = {
      // positions          // texture Coords
      5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, 5.0f,
      0.0f, 0.0f,  -5.0f, -0.5f, -5.0f, 0.0f,  2.0f,

      5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, -5.0f,
      0.0f, 2.0f,  5.0f,  -0.5f, -5.0f, 2.0f,  2.0f};
  float transparentVertices[] = {
      // positions         // texture Coords (swapped y coordinates because
      // texture is flipped upside down)
      0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 0.0f, -0.5f, 0.0f,
      0.0f, 1.0f, 1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

      0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 1.0f, -0.5f, 0.0f,
      1.0f, 1.0f, 1.0f, 0.5f,  0.0f, 1.0f, 0.0f};
  // cube VAO
  unsigned int cubeVAO, cubeVBO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  // plane VAO
  unsigned int planeVAO, planeVBO;
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);
  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  // transparent VAO
  unsigned int transparentVAO, transparentVBO;
  glGenVertexArrays(1, &transparentVAO);
  glGenBuffers(1, &transparentVBO);
  glBindVertexArray(transparentVAO);
  glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices),
               transparentVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);

  // load textures
  // -------------
  TextureOptions options;
  unsigned int cubeTexture =
      MakeTexture("../resources/textures/marble.jpg", &options);
  unsigned int floorTexture =
      MakeTexture("../resources/textures/metal.png", &options);
  unsigned int grassTexture =
      MakeTexture("../resources/textures/window.png", &options);

  std::vector<glm::vec3> windows;
  windows.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
  windows.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
  windows.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
  windows.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
  windows.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

  curShader.use();
  curShader.setInt("texture1", 0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  std::map<float, glm::vec3> sorted;
  for (unsigned int i = 0; i < windows.size(); i++) {
    float distance = glm::length(camera.Position - windows[i]);
    sorted[distance] = windows[i];
  }

  while (!glfwWindowShouldClose(window)) {
    camera.ProcessInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    curShader.use();
    glm::vec3 axis(0.0f, 1.0f, 0.0f);
    glm::mat4 view = camera.GetViewMatrix(axis);
    glm::mat4 projection = camera.GetProjectionMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    curShader.setMat4("projection", projection);
    curShader.setMat4("view", view);

    DrawContainers(1.0f, curShader, cubeTexture, cubeVAO);
    // floor
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    curShader.setMat4("model", glm::mat4(1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(transparentVAO);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    // for (unsigned int i = 0; i < windows.size(); i++) {
    for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin();
         it != sorted.rend(); ++it) {
      model = glm::mat4(1.0f);
      model = glm::translate(model, it->second);
      curShader.setMat4("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};

void InitializeLights(Shader &targetShader, Camera &camera) {

  targetShader.use();
  targetShader.setVec3("viewPos", camera.Position);
  // targetShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
  targetShader.setFloat("material.shininess", 64.0f);

  // point light 1
  targetShader.setVec3("pointLights[0].position", pointLightPositions[0]);
  targetShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
  targetShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
  targetShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("pointLights[0].constant", 1.0f);
  targetShader.setFloat("pointLights[0].linear", 0.09f);
  targetShader.setFloat("pointLights[0].quadratic", 0.032f);
  // point light 2
  targetShader.setVec3("pointLights[1].position", pointLightPositions[1]);
  targetShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
  targetShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
  targetShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("pointLights[1].constant", 1.0f);
  targetShader.setFloat("pointLights[1].linear", 0.09f);
  targetShader.setFloat("pointLights[1].quadratic", 0.032f);
  // point light 3
  targetShader.setVec3("pointLights[2].position", pointLightPositions[2]);
  targetShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
  targetShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
  targetShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("pointLights[2].constant", 1.0f);
  targetShader.setFloat("pointLights[2].linear", 0.09f);
  targetShader.setFloat("pointLights[2].quadratic", 0.032f);
  // point light 4
  targetShader.setVec3("pointLights[3].position", pointLightPositions[3]);
  targetShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
  targetShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
  targetShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("pointLights[3].constant", 1.0f);
  targetShader.setFloat("pointLights[3].linear", 0.09f);
  targetShader.setFloat("pointLights[3].quadratic", 0.032f);
  // spotLight
  targetShader.setVec3("spotLight.position", camera.Position);
  targetShader.setVec3("spotLight.direction", camera.Front);
  targetShader.setVec3("spotLight.ambient", 0.1f, 0.1f, 0.1f);
  targetShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
  targetShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("spotLight.constant", 1.0f);
  targetShader.setFloat("spotLight.linear", 0.09f);
  targetShader.setFloat("spotLight.quadratic", 0.032f);
  targetShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
  targetShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
}

void UpdateLights(Shader &targetShader, Camera &camera) {
  targetShader.use();
  targetShader.setVec3("viewPos", camera.Position);
  targetShader.setVec3("spotLight.position", camera.Position);
  targetShader.setVec3("spotLight.direction", camera.Front);
}

void DrawContainers(float scale, Shader &shader, unsigned int texture,
                    int VAO) {
  glBindVertexArray(VAO);
  shader.use();
  // float scale = 1.1f;
  // cubes
  glBindTexture(GL_TEXTURE_2D, texture);
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
  model = glm::scale(model, glm::vec3(scale, scale, scale));
  shader.setMat4("model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
  model = glm::scale(model, glm::vec3(scale, scale, scale));
  shader.setMat4("model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}
