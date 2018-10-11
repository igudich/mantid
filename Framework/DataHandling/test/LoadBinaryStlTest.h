#ifndef LOAD_BINARYSTL_TEST_H_
#define LOAD_BINARYSTL_TEST_H_

#include "MantidAPI/AnalysisDataService.h"
#include "MantidAPI/FileFinder.h"
#include "MantidAPI/FrameworkManager.h"
#include "MantidAPI/Sample.h"
#include "MantidDataHandling/LoadBinaryStl.h"
#include "MantidDataHandling/LoadInstrument.h"
#include "MantidGeometry/Objects/MeshObject.h"
#include "MantidKernel/OptionalBool.h"
#include "MantidTestHelpers/ComponentCreationHelper.h"
#include "MantidTestHelpers/WorkspaceCreationHelper.h"
#include <cxxtest/TestSuite.h>

using namespace Mantid;
using namespace Mantid::API;
using namespace Mantid::DataHandling;
using namespace Mantid::DataObjects;
using namespace Mantid::Geometry;

class LoadBinaryStlTest : public CxxTest::TestSuite {
public:
  static LoadBinaryStlTest *createSuite() { return new LoadBinaryStlTest(); }
  static void destroySuite(LoadBinaryStlTest *suite) { delete suite; }

  void assert_shape_matches(std::unique_ptr<Geometry::MeshObject> &shape,
                            int vertices, int triangles, int volume,
                            double delta) {
    TS_ASSERT(shape->hasValidShape());
    TS_ASSERT_EQUALS(shape->numberOfVertices(), vertices);
    TS_ASSERT_EQUALS(shape->numberOfTriangles(), triangles);
    TS_ASSERT_DELTA(shape->volume(), volume, delta);
  }

  void test_loading_cube_stl() {
    std::string path = FileFinder::Instance().getFullPath("cubeBin.stl");

    std::unique_ptr<LoadBinaryStl> loader =
        std::make_unique<LoadBinaryStl>(path);
    auto cube = loader->readStl();

    assert_shape_matches(cube, 8, 12, 3000, 0.001);
  }

  void test_loading_cylinder_stl() {
    std::string path = FileFinder::Instance().getFullPath("cylinderBin.stl");

    std::unique_ptr<LoadBinaryStl> loader =
        std::make_unique<LoadBinaryStl>(path);
    auto cylinder = loader->readStl();

    assert_shape_matches(cylinder, 722, 1440, 589, 1);
  }

  void test_loading_tube_stl() {
    std::string path = FileFinder::Instance().getFullPath("tubeBin.stl");

    std::unique_ptr<LoadBinaryStl> loader =
        std::make_unique<LoadBinaryStl>(path);
    auto tube = loader->readStl();

    assert_shape_matches(tube, 1080, 2160, 7068, 1);
  }
  // check that isBinaryStl returns false if the file contains an incomplete
  // vertex
  void test_fail_invalid_vertex() {
    std::string path =
        FileFinder::Instance().getFullPath("invalid_vertexBin.stl");

    std::unique_ptr<LoadBinaryStl> loader =
        std::make_unique<LoadBinaryStl>(path);

    TS_ASSERT(!(loader->isBinarySTL()));
  }
  // check that isBinaryStl returns false if the file contains an incomplete
  // triangle
  void test_fail_invalid_triangle() {
    std::string path =
        FileFinder::Instance().getFullPath("invalid_triangleBin.stl");

    std::unique_ptr<LoadBinaryStl> loader =
        std::make_unique<LoadBinaryStl>(path);

    TS_ASSERT(!(loader->isBinarySTL()));
  }

  void test_fail_ascii_stl() {
    std::string path = FileFinder::Instance().getFullPath("cube.stl");

    std::unique_ptr<LoadBinaryStl> loader =
        std::make_unique<LoadBinaryStl>(path);

    TS_ASSERT(!(loader->isBinarySTL()));
  }
};

#endif /* LOAD_BINARYSTL_TEST_H_ */