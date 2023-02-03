#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum ControllerState { CONTROLLER_ACTIVE, CONTROLLER_MENU, CONTROLLER_WIN };

class Controller {
 public:
  ControllerState State;
  GLboolean Keys[1024];
  GLuint Width, Height;
  Controller(GLuint width, GLuint height);
  ~Controller();
  void Init();
  void ProcessInput(GLfloat dt);
  void Update(GLfloat dt);
  void Render();
};

#endif