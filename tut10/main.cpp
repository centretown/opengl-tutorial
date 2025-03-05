#include "glad.h"
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "texture.hpp"

// #define MAX_ANGLE 180.0f
// #define MIN_ANGLE 15.0f
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
float windowWidth = (float)SCREEN_WIDTH;
float windowHeight = (float)SCREEN_HEIGHT;
// forward decls.
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
GLFWwindow *InitWindow();

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

static float deltaTime = 0.0f;    // Time between current frame and last frame
static float lastFrame = 0.0f;    // Time of last frame
static float currentFrame = 0.0f; // Time of last frame

// static glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
// static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// static float mixValue = 0.5f;
// static float fovValue = 45.0f;
//
ShaderCode depthCode("assets/shaders/gls330/target.vert",
                     "assets/shaders/gls330/target.frag");

ShaderCode sourceCode("assets/shaders/gls330/source.vert",
                      "assets/shaders/gls330/source.frag");

const char *textureFileName = "assets/textures/container2.png";
const char *specularFileName = "assets/textures/container2_specular.png";
TextureOptions textureOptions;
TextureOptions specularOptions;

// glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main(int argc, char **argv) {

  if (!depthCode.Load() || !sourceCode.Load()) {
    return -1;
  }

  GLFWwindow *window = InitWindow();

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    glfwTerminate();
    return -1;
  }
  // don't forget to activate the shader before setting uniforms!
  glEnable(GL_DEPTH_TEST);

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  float vertices[] = {
      // positions          // normals           // texture coords
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
      0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
      0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
      -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
      -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
      -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
      -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
  };

  Shader targetShader(&depthCode);
  Shader sourceShader(&sourceCode);
  if (!targetShader.IsValid() || !sourceShader.IsValid()) {
    glfwTerminate();
    return -1;
  }

  // first, configure the target cube's VAO (and VBO)
  unsigned int VBO, targetVAO;
  glGenVertexArrays(1, &targetVAO);
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindVertexArray(targetVAO);
  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  // second, configure the light's VAO (VBO stays the same; the vertices are the
  // same for the light object which is also a 3D cube)
  unsigned int sourceVAO;
  glGenVertexArrays(1, &sourceVAO);
  glBindVertexArray(sourceVAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  unsigned int diffuseMap = MakeTexture(textureFileName, &textureOptions);
  unsigned int specularMap = MakeTexture(specularFileName, &specularOptions);
  targetShader.use();
  targetShader.setInt("material.diffuse", 0);
  targetShader.setInt("material.specular", 1);

  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

  glm::vec3 pointLightPositions[] = {
      glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
      glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};

  while (!glfwWindowShouldClose(window)) {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // be sure to activate shader when setting uniforms/drawing objects
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
    targetShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    targetShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    targetShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    targetShader.setFloat("spotLight.constant", 1.0f);
    targetShader.setFloat("spotLight.linear", 0.09f);
    targetShader.setFloat("spotLight.quadratic", 0.032f);
    targetShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    targetShader.setFloat("spotLight.outerCutOff",
                          glm::cos(glm::radians(15.0f)));

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)1 / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    targetShader.setMat4("projection", projection);
    targetShader.setMat4("view", view);
    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    targetShader.setMat4("model", model);
    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    // render the cube
    glBindVertexArray(targetVAO);
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    for (unsigned int i = 0; i < 10; i++) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * i;
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      targetShader.setMat4("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // draw the lamp objects
    sourceShader.use();
    sourceShader.setMat4("projection", projection);
    sourceShader.setMat4("view", view);

    for (int i = 0; i < 4; i++) {
      model = glm::mat4(1.0f);
      model = glm::translate(model, pointLightPositions[i]);
      model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
      sourceShader.setMat4("model", model);

      glBindVertexArray(sourceVAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &targetVAO);
  glDeleteVertexArrays(1, &sourceVAO);
  glDeleteBuffers(1, &VBO);
  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  windowWidth = width;
  windowHeight = height;
  glViewport(0, 0, width, height);
}

GLFWwindow *InitWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    return NULL;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  glfwSetScrollCallback(window, scroll_callback);
  return window;
}

bool leftButtonDown = false;

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  leftButtonDown = (button == GLFW_MOUSE_BUTTON_LEFT && action == 1);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;
  if (leftButtonDown)
    camera.ProcessMouseMovement(-xoffset, -yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
