#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum ControlerState { CONTROLER_ACTIVE, CONTROLER_MENU, CONTROLER_WIN };

class Controler {
 public:
  ControlerState State;
  GLboolean Keys[1024];
  GLuint Width, Height;
  // Constructor/Destructor
  Controler(GLuint width, GLuint height);
  ~Controler();
  // Initialize game state (load all shaders/textures/levels)
  void Init();
  // GameLoop
  void ProcessInput(GLfloat dt);
  void Update(GLfloat dt);
  void Render();
};

#endif