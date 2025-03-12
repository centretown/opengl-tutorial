#include "glad.h"
#include <GLFW/glfw3.h>

#include <cstring>
#include <glm/ext/matrix_transform.hpp>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

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
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
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
Shader shader("assets/shaders/gls100/depth.vert",
              "assets/shaders/gls100/depth.frag");
Shader screenShader("assets/shaders/gls100/frame.vert",
                    "assets/shaders/gls100/frame.frag");
#else
Shader curShader("assets/shaders/gls330/depth.vert",
                 "assets/shaders/gls330/depth.frag");
Shader screenShader("assets/shaders/gls330/frame.vert",
                    "assets/shaders/gls330/frame.frag");
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

  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);
  // glFrontFace(GL_CW);

  curShader.Build();
  screenShader.Build();
  if (!curShader.IsValid() || !screenShader.IsValid()) {
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
  float quadVertices[] = {
      // vertex attributes for a quad that fills the entire screen in Normalized
      // Device Coordinates. NOTE that this plane is now much smaller and at the
      // top of the screen
      // positions   // texCoords
      -0.3f, 1.0f, 0.0f, 1.0f, -0.3f, 0.7f, 0.0f, 0.0f, 0.3f, 0.7f, 1.0f, 0.0f,

      -0.3f, 1.0f, 0.0f, 1.0f, 0.3f,  0.7f, 1.0f, 0.0f, 0.3f, 1.0f, 1.0f, 1.0f};
  // float quadVertices[] = {// vertex attributes for a quad that fills the
  // entire
  //                         // screen in Normalized Device Coordinates.
  //                         // positions   // texCoords
  //                         -1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
  //                         0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,

  //                         -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
  //                         1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};

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
  unsigned int quadVAO, quadVBO;
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  // glBindVertexArray(0);

  // load textures
  // -------------
  TextureOptions options;
  unsigned int cubeTexture =
      MakeTexture("../resources/textures/container.jpg", &options);
  unsigned int floorTexture =
      MakeTexture("../resources/textures/metal.png", &options);

  curShader.use();
  curShader.setInt("texture1", 0);

  screenShader.use();
  screenShader.setInt("screenTexture", 0);

  // framebuffer configuration
  // -------------------------
  unsigned int framebuffer;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  // create a color attachment texture
  unsigned int textureColorbuffer;
  glGenTextures(1, &textureColorbuffer);
  glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textureColorbuffer, 0);
  // create a renderbuffer object for depth and stencil attachment (we won't be
  // sampling these)
  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH,
                        SCREEN_WIDTH); // use a single renderbuffer object for
                                       // both a depth AND stencil buffer.
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, rbo); // now actually attach it
  // now that we actually created the framebuffer and added all attachments we
  // want to check if it is actually complete now
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // draw as wireframe
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {
    camera.ProcessInput(window);

    glm::vec3 axis(0.0f, 1.0f, 0.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering
                             // screen-space quad)

    // make sure we clear the framebuffer's content
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    curShader.use();
    glm::mat4 model = glm::mat4(1.0f);
    camera.Yaw += 180.0f; // rotate the camera's yaw 180 degrees around
    camera.ProcessMouseMovement(
        0, 0, false); // call this to make sure it updates its camera vectors,
                      // note that we disable pitch constrains for this specific
                      // case (otherwise we can't reverse camera's pitch values)
    glm::mat4 view = camera.GetViewMatrix(axis);
    camera.Yaw -= 180.0f; // reset it back to its original orientation
    camera.ProcessMouseMovement(0, 0, true);
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
        0.1f, 100.0f);
    curShader.setMat4("view", view);
    curShader.setMat4("projection", projection);
    // cubes
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    curShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    curShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // floor
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    curShader.setMat4("model", glm::mat4(1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // second render pass: draw as normal
    // ----------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model = glm::mat4(1.0f);
    view = camera.GetViewMatrix(axis);
    curShader.setMat4("view", view);

    // cubes
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    curShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    curShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // floor
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    curShader.setMat4("model", glm::mat4(1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // now draw the mirror quad with screen texture
    // --------------------------------------------
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't
                              // discarded due to depth test.

    screenShader.use();
    glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D,
                  textureColorbuffer); // use the color attachment texture as
                                       // the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &planeVAO);
  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &cubeVBO);
  glDeleteBuffers(1, &planeVBO);
  glDeleteBuffers(1, &quadVBO);
  glDeleteRenderbuffers(1, &rbo);
  glDeleteFramebuffers(1, &framebuffer);
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
