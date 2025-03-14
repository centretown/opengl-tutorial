#include "glad.h"
#include <GLFW/glfw3.h>

#include <cstring>
#include <glm/ext/matrix_transform.hpp>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "filesystem.h"
#include "model.hpp"
#include "shader.hpp"
#include "win.hpp"

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

// #define MAX_ANGLE 180.0f
// #define MIN_ANGLE 15.0f
#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 960
float windowWidth = (float)SCREEN_WIDTH;
float windowHeight = (float)SCREEN_HEIGHT;

// forward decls.
void InitializeLights(Shader &targetShader, Camera &camera);
void UpdateLights(Shader &targetShader, Camera &camera);

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// #define USE_OPEN_GLES
#if defined(USE_OPEN_GLES)
#define GLSL_VERSION 100
#elif defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif // USE_OPEN_GLES

#ifdef USE_OPEN_GLES
Shader curShader("assets/shaders/gls100/target.vert",
                 "assets/shaders/gls100/target.frag");
#else
Shader curShader("assets/shaders/gls330/target.vert",
                 "assets/shaders/gls330/target.frag");
#endif

const char *stlDir = "resources/stl";
const char *objDir = "resources/objects";
const char *defaultModelPath = "resources/objects/cyborg/cyborg.obj";
char modelPath[1024];

float rotation_angle = 0.0f;

int main(int argc, char **argv) {

  if (argc > 1) {
    const char *obj = argv[1];
    if (argc > 2 && !strncmp("stl", obj, 3)) {
      snprintf(modelPath, sizeof(modelPath), "%s/%s.stl", stlDir, argv[2]);
    } else if (argc > 2 && !strncmp("gltf", obj, 4)) {
      snprintf(modelPath, sizeof(modelPath), "%s/%s/scene.gltf", objDir,
               argv[2]);
    } else {
      snprintf(modelPath, sizeof(modelPath), "%s/%s/%s.obj", objDir, obj, obj);
    }
  } else {
    strncpy(modelPath, defaultModelPath, sizeof(modelPath));
  }

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

  curShader.Build();
  if (!curShader.IsValid()) {
    glfwTerminate();
    return -1;
  }
  Model curModel(FileSystem::getPath(modelPath));
  // calculate scale
  float scale = 1.0;
  float diffx = curModel.max.x - curModel.min.x;
  float diffy = curModel.max.y - curModel.min.y;
  float diffz = curModel.max.z - curModel.min.z;
  float diff = fmax(diffx, diffy);
  if (diff != 0.0f)
    scale = 1.0 / diff;
  float scaleX = 1.0 / diffx;
  diffx = (curModel.min.x + diffx) * scaleX;
  float scaleY = 1.0 / diffy;
  diffy = (curModel.min.y + diffy) * scaleY;
  float scaleZ = 1.0 / diffz;
  diffz = (curModel.min.y + diffz) * scaleZ;
  // diffx /= 2.0f;
  InitializeLights(curShader, camera);

  while (!glfwWindowShouldClose(window)) {
    // currentFrame = glfwGetTime();
    // deltaTime = currentFrame - lastFrame;
    // lastFrame = currentFrame;
    camera.ProcessInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    curShader.use();

    UpdateLights(curShader, camera);
    // view/projection transformations
    // glm::vec3 axis(0.0f, 1.0f, 0.0f);
    glm::vec3 axis(0.0f, 1.0f, 0.0f);
    glm::mat4 view = camera.GetViewMatrix(axis);
    glm::mat4 projection = camera.GetProjectionMatrix();

    curShader.setMat4("projection", projection);
    curShader.setMat4("view", view);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    // translate it down so it's at the center of the scene
    model = glm::translate(model, glm::vec3(diffx, diffy, -diffz));
    // it's a bit too big for our scene, so scale it down
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    // rotate about y-axis
    // model = glm::rotate(model, glm::radians(rotation_angle), axis);
    curShader.setMat4("model", model);

    curModel.Draw(curShader);
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
