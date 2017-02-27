#ifndef MANTID_CRYSTAL_CALIBRATIONHELPERSTEST_H_
#define MANTID_CRYSTAL_CALIBRATIONHELPERSTEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/DetectorInfo.h"
#include "MantidCrystal/CalibrationHelpers.h"
#include "MantidGeometry/Instrument.h"
#include "MantidKernel/V3D.h"
#include "MantidTestHelpers/WorkspaceCreationHelper.h"

using Mantid::Crystal::CalibrationHelpers;

using namespace Mantid::API;
using namespace Mantid::Crystal;
using namespace Mantid::Geometry;
using namespace Mantid::Kernel;

class CalibrationHelpersTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static CalibrationHelpersTest *createSuite() { return new CalibrationHelpersTest(); }
  static void destroySuite( CalibrationHelpersTest *suite ) { delete suite; }

  void test_fixUpSamplePosition_moves_the_sample() {
    // Create two identical workspaces
    const auto wsOld = WorkspaceCreationHelper::create2DWorkspaceWithFullInstrument(2, 1000, true);
    auto wsNew = WorkspaceCreationHelper::create2DWorkspaceWithFullInstrument(2, 1000, true);

    const auto &positionSampleOld = wsOld->detectorInfo().samplePosition();
    const auto &positionSourceOld = wsOld->detectorInfo().sourcePosition();

    const auto &instNew = wsNew->getInstrument();
    const double l1 = 10.0;
    const double newZ = 3.0;
    const auto &positionNew = V3D(1.0, 2.0, newZ);

    const auto &pmapOld = boost::make_shared<ParameterMap>(wsOld->instrumentParameters());
    CalibrationHelpers::fixUpSourceParameterMap(instNew, l1, positionNew, pmapOld);

    // Old workspace has sample unchanged
    TS_ASSERT_EQUALS(wsOld->detectorInfo().samplePosition(), positionSampleOld);
    // New workspace has sample at the new position
    TS_ASSERT_EQUALS(wsNew->detectorInfo().samplePosition(), positionNew);
    // Old workspace has source at the old position
    TS_ASSERT_EQUALS(wsOld->detectorInfo().sourcePosition(), positionSourceOld);
    // New workspace has source at the new position
    TS_ASSERT_EQUALS(wsNew->detectorInfo().sourcePosition(), V3D(1.0, 2.0, newZ - l1));
  }
};

#endif /* MANTID_CRYSTAL_CALIBRATIONHELPERSTEST_H_ */