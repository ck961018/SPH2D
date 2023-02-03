#include "triangle_renderer.h"

TriangleRenderer::TriangleRenderer(Shader &shader) {
  this->shader = shader;
  this->initRenderData();
}

TriangleRenderer::~TriangleRenderer() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void TriangleRenderer::DrawTriangles(std::vector<glm::vec2> position,
                                     std::vector<glm::ivec3> index) {
  // prepare transformations
  this->shader.Use();
  this->shader.SetVector3f("spriteColor", {0.2, 0.5, 0.3});

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, position.size() * sizeof(glm::vec2), position.data());

  glDrawArrays(GL_POINTS, 0, position.size());

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, index.size() * sizeof(glm::ivec3), index.data());

  glDrawElements(GL_TRIANGLES, index.size() * 3, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
}

void TriangleRenderer::initRenderData() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, 200000, nullptr, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec2), nullptr);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 200000 * sizeof(glm::ivec3), nullptr, GL_DYNAMIC_DRAW);
}