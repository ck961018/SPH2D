#ifndef TRIANGLE_RENDERER_H
#define TRIANGLE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "texture.h"
#include "shader.h"

class TriangleRenderer {
 public:
  // Constructor (inits shaders/shapes)
  TriangleRenderer(Shader &shader);
  // Destructor
  ~TriangleRenderer();
  // Renders a defined quad textured with given sprite
  void DrawTriangles(std::vector<glm::vec2> position, std::vector<glm::ivec3> index);

 private:
  // Render state
  Shader shader;
  GLuint VAO, VBO, EBO;
  // Initializes and configures the quad's buffer and vertex attributes
  void initRenderData();
};

#endif