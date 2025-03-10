
#include "win.hpp"
#include "glad.h"

// #define USE_OPEN_GLES

WinState winState;

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  winState.leftButtonDown = (button == GLFW_MOUSE_BUTTON_LEFT && action == 1);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (winState.firstMouse) {
    winState.lastX = xpos;
    winState.lastY = ypos;
    winState.firstMouse = false;
  }

  float xoffset = xpos - winState.lastX;
  float yoffset = winState.lastY -
                  ypos; // reversed since y-coordinates go from bottom to top

  winState.lastX = xpos;
  winState.lastY = ypos;
  if (winState.leftButtonDown)
    winState.camera->ProcessMouseMovement(-xoffset, -yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  winState.camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  winState.camera->ResizeWindow(width, height);
  glViewport(0, 0, width, height);
}

GLFWwindow *InitWindow(Camera *camera, int width, int height) {
  winState = WinState(camera, width, height);
  glfwInit();

#ifdef USE_OPEN_GLES
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
  GLFWwindow *window =
      glfwCreateWindow(width, height, "LearnOpenGL OpenGL ES 2.0", NULL, NULL);
  if (!window) {
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    window = glfwCreateWindow(width, height, "LearnOpenGL OpenGL ES 2.0 Native",
                              NULL, NULL);
    if (!window) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }
  }
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window =
      glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    return window;
  }
#endif // USE_OPEN_GLES

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  GLFWwindow *window =
      glfwCreateWindow(width, height, "LearnOpenGL Apple", NULL, NULL);
  if (window == NULL) {
    return window;
  }
#endif

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window, scroll_callback);
  // tell GLFW to capture our mouse
  // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  return window;
}
