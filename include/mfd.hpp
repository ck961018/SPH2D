#include <jet/jet.h>

#ifdef JET_WINDOWS
#include <direct.h>
#else
#include <sys/stat.h>
#endif

using namespace jet;

const std::string kSpherical = "spherical";
const std::string kSph = "sph";
const std::string kZhuBridson = "zhu_bridson";
const std::string kAnisotropic = "anisotropic";

double sSphCutOffDensity = 0.5;
double sZhuBridsonCutOffThreshold = 0.6;
double sAnisoCutOffDensity = 0.5;
double sAnisoPositionSmoothingFactor = 0.5;
size_t sAnisoMinNumNeighbors = 25;

void runSimulation(const SphSolver3Ptr& solver, int numberOfFrames, double fps) {
  for (Frame frame(0, 1.0 / fps); frame.index < numberOfFrames; ++frame) {
    solver->update(frame);
  }
}

// Water-drop example (PCISPH)
void initExample1(PciSphSolver2Ptr& solver, double targetSpacing, int numberOfFrames, double fps) {
  BoundingBox2D domain(Vector2D(-1, -1), Vector2D(1, 1));
  BoundingBox2D domain2(Vector2D(-0.01, -1), Vector2D(0.01, -0.7));

  // Build solver
  solver = PciSphSolver2::builder()
               .withTargetDensity(1000.0)
               .withTargetSpacing(targetSpacing)
               .withRelativeKernelRadius(1.8)
               .makeShared();

  solver->setPseudoViscosityCoefficient(0.2);

  // Build emitter
  BoundingBox2D sourceBound(domain);
  sourceBound.expand(-targetSpacing);

  // auto plane =
  //     Plane2::builder().withNormal({0, 1}).withPoint({0, 0.35 * domain.height()}).makeShared();

  auto sphere = Sphere2::builder()
                    .withCenter(domain.midPoint())
                    .withRadius(0.35 * domain.width())
                    .makeShared();

  auto surfaceSet = ImplicitSurfaceSet2::builder().withExplicitSurfaces({sphere}).makeShared();

  auto emitter = VolumeParticleEmitter2::builder()
                     .withImplicitSurface(surfaceSet)
                     .withSpacing(targetSpacing)
                     .withMaxRegion(sourceBound)
                     .withIsOneShot(true)
                     .makeShared();

  solver->setEmitter(emitter);

  // Build collider
  auto box = Box2::builder().withIsNormalFlipped(true).withBoundingBox(domain).makeShared();
  auto box2 = Box2::builder().withIsNormalFlipped(false).withBoundingBox(domain2).makeShared();
  auto surfaceSet2 = ImplicitSurfaceSet2::builder().withExplicitSurfaces({box, box2}).makeShared();

  auto collider = RigidBodyCollider2::builder().withSurface(surfaceSet2).makeShared();

  solver->setCollider(collider);
}

void printInfo(const Size2& resolution, const BoundingBox2D& domain, const Vector2D& gridSpacing,
               size_t numberOfParticles, const std::string& method) {
  printf("Resolution: %zu x %zu\n", resolution.x, resolution.y);
  printf("Domain: [%f, %f] x [%f, %f]\n", domain.lowerCorner.x, domain.lowerCorner.y,
         domain.upperCorner.x, domain.upperCorner.y);
  printf("Grid spacing: [%f, %f]\n", gridSpacing.x, gridSpacing.y);
  printf("Number of particles: %zu\n", numberOfParticles);
  printf("Reconstruction method: %s\n", method.c_str());
}

void particlesToTriangles(const ArrayAccessor1<Vector2D>& positions, const Size2& resolution,
                          const Vector2D& gridSpacing, const Vector2D& origin, double kernelRadius,
                          const std::string& method, TriangleMesh3& mesh) {
  PointsToImplicit2Ptr converter;
  if (method == kSpherical) {
    converter = std::make_shared<SphericalPointsToImplicit2>(kernelRadius, false);
  } else if (method == kSph) {
    converter = std::make_shared<SphPointsToImplicit2>(kernelRadius, sSphCutOffDensity, false);
  } else if (method == kZhuBridson) {
    converter = std::make_shared<ZhuBridsonPointsToImplicit2>(kernelRadius,
                                                              sZhuBridsonCutOffThreshold, false);
  } else {
    converter = std::make_shared<AnisotropicPointsToImplicit2>(kernelRadius, sAnisoCutOffDensity,
                                                               sAnisoPositionSmoothingFactor,
                                                               sAnisoMinNumNeighbors, false);
  }

  VertexCenteredScalarGrid2 sdf(resolution, gridSpacing, origin);
  // printInfo(resolution, sdf.boundingBox(), gridSpacing, positions.size(), method);

  converter->convert(positions, &sdf);

  marchingSquares(sdf.dataAccessor(), sdf.gridSpacing(), sdf.dataOrigin(), &mesh);
}