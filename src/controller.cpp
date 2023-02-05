#include "controller.h"
#include "resource_manager.h"
#include "point_renderer.h"
#include "triangle_renderer.h"

#include "mfd.hpp"

PointRenderer* Renderer;
TriangleRenderer* Renderer2;
double targetSpacing = 0.07;
int numberOfFrames = 100000;
double fps = 60.0;
Frame frame(0, 1.0 / fps);
PciSphSolver2Ptr solver;
std::ofstream logFile;
std::string method = "zhu_bridson";
double kernelRadius = 0.075;

Controller::Controller(GLuint width, GLuint height)
    : State(CONTROLLER_ACTIVE), Keys(), Width(width), Height(height) {}

Controller::~Controller() {
  delete Renderer;
  delete Renderer2;
}

void Controller::Init() {
  // 加载着色器
  ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr, "point");
  ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr, "triangle");
  // 设置专用于渲染的控制
  Renderer = new PointRenderer(ResourceManager::GetShader("point"));
  Renderer2 = new TriangleRenderer(ResourceManager::GetShader("triangle"));

  std::string logFilename = R"(log.txt)";
  logFile.open(logFilename.c_str());
  if (logFile) {
    Logging::setAllStream(&logFile);
  }
  initExample1(solver, targetSpacing, numberOfFrames, fps);
}

void Controller::Update(GLfloat dt) {
  if (frame.index == numberOfFrames) {
    return;
  }
  solver->update(frame);
  frame++;

  //  实时
  // int frameNum = static_cast<int>(dt * fps);
  // if (frameNum != 0) {
  //   frame.advance(frameNum);
  // } else {
  //   frame++;
  // }
}

void Controller::ProcessInput(GLfloat dt) {}

void Controller::Render() {
  auto tmp = solver->particleSystemData();
  std::vector<glm::vec2> particles;
  tmp->positions().forEach([&](const auto& p) { particles.push_back({p.x, p.y}); });
  Renderer->DrawPoints(particles, 3.0, {0.0, 1.0, 0.0});

  Vector2D gridSpacing(0.025, 0.025);
  Size2 resolution(static_cast<size_t>(2.0 / gridSpacing.x),
                   static_cast<size_t>(2.0 / gridSpacing.y));
  Vector2D origin(-1.0, -1.0);

  TriangleMesh3 mesh;
  particlesToTriangles(tmp->positions(), resolution, gridSpacing, origin, kernelRadius, method,
                       mesh);
  particles.clear();
  for (int i = 0; i < mesh.numberOfPoints(); i++) {
    auto _point = mesh.point(i);
    particles.push_back({_point.x, _point.y});
  }
  std::vector<glm::ivec3> index;
  for (int i = 0; i < mesh.numberOfTriangles(); i++) {
    auto _index = mesh.pointIndex(i);
    index.push_back({_index.x, _index.y, _index.z});
  }
  Renderer2->DrawTriangles(particles, index);
}