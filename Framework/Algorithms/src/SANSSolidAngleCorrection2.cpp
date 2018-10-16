// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +

#include "MantidAlgorithms/SANSSolidAngleCorrection2.h"
#include "MantidAPI/AlgorithmProperty.h"
#include "MantidAPI/HistogramValidator.h"
#include "MantidAPI/SpectrumInfo.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidAPI/WorkspaceUnitValidator.h"
#include "MantidDataObjects/EventList.h"
#include "MantidDataObjects/EventWorkspace.h"
#include "MantidKernel/CompositeValidator.h"
#include "MantidKernel/PropertyManager.h"
#include "MantidKernel/PropertyManagerDataService.h"
#include "MantidKernel/ListValidator.h"
#include "MantidGeometry/Instrument.h"


namespace Mantid {
namespace Algorithms {

using namespace Kernel;
using namespace API;
using namespace Geometry;
using namespace DataObjects;

// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(SANSSolidAngleCorrection2)

/*
 * Returns the angle between the sample-to-pixel vector and its
 * projection on the X-Z plane.
 * */
static double getYTubeAngle(const SpectrumInfo &spectrumInfo, size_t index) {
  const V3D samplePos = spectrumInfo.samplePosition();

  // Get the vector from the sample position to the detector pixel
  V3D sampleDetVec = spectrumInfo.position(index) - samplePos;

  // Get the projection of that vector on the X-Z plane
  V3D inPlane = V3D(sampleDetVec);
  inPlane.setY(0.0);

  // This is the angle between the sample-to-detector vector
  // and its project on the X-Z plane.
  return sampleDetVec.angle(inPlane);
}


void SANSSolidAngleCorrection2::init() {
  auto wsValidator = boost::make_shared<CompositeValidator>();
  // wsValidator->add<WorkspaceUnitValidator>("Wavelength");
  wsValidator->add<HistogramValidator>();
  declareProperty(make_unique<WorkspaceProperty<>>(
      "InputWorkspace", "", Direction::Input, wsValidator));
  declareProperty(make_unique<WorkspaceProperty<>>("OutputWorkspace", "",
                                                   Direction::Output));

  std::vector<std::string> exp_options{"Normal", "Tube", "Wing"};
  declareProperty(
      "Type", "Normal",
      boost::make_shared<StringListValidator>(exp_options),
      "Select the method to calculate the Solid Angle.\n"
      "Normal: cos^3(theta); Tube: cons(alpha)*cos^3(theta); "
      "Wing: cos^3(alpha);");

}

void SANSSolidAngleCorrection2::exec() {

  MatrixWorkspace_const_sptr inputWS = getProperty("InputWorkspace");
  DataObjects::EventWorkspace_const_sptr inputEventWS =
      boost::dynamic_pointer_cast<const EventWorkspace>(inputWS);
  if (inputEventWS)
    return execEvent();

  // Now create the output workspace
  MatrixWorkspace_sptr outputWS = getProperty("OutputWorkspace");
  if (outputWS != inputWS) {
    outputWS = WorkspaceFactory::Instance().create(inputWS);
    outputWS->setDistribution(true);
    outputWS->setYUnit("");
    outputWS->setYUnitLabel("Steradian");
    setProperty("OutputWorkspace", outputWS);
  }

  const int numHists = static_cast<int>(inputWS->getNumberHistograms());
  Progress progress(this, 0.0, 1.0, numHists);

  // Number of X bins
  const int xLength = static_cast<int>(inputWS->y(0).size());

  const auto &spectrumInfo = inputWS->spectrumInfo();
  const auto &componentInfo = inputWS->componentInfo();
  const auto &instrument = inputWS->getInstrument();
  const double pixelSizeX = inputWS->getInstrument()->getNumberParameter("x-pixel-size")[0];
  const double pixelSizeY = inputWS->getInstrument()->getNumberParameter("y-pixel-size")[0];

  PARALLEL_FOR_IF(Kernel::threadSafe(*outputWS, *inputWS))
  for (int i = 0; i < numHists; ++i) {
    PARALLEL_START_INTERUPT_REGION
    outputWS->setSharedX(i, inputWS->sharedX(i));

    if (!spectrumInfo.hasDetectors(i)) {
      g_log.warning() << "Workspace index " << i
                      << " has no detector assigned to it - discarding\n";
      continue;
    }

    // Skip if we have a monitor or if the detector is masked.
    if (spectrumInfo.isMonitor(i) || spectrumInfo.isMasked(i))
      continue;

    const auto &YIn = inputWS->y(i);
    const auto &EIn = inputWS->e(i);

    auto &YOut = outputWS->mutableY(i);
    auto &EOut = outputWS->mutableE(i);

    double corr = calculateSolidAngle(i, spectrumInfo, componentInfo, pixelSizeX, pixelSizeY);

    // Correct data for all X bins
    for (int j = 0; j < xLength; j++) {
      YOut[j] = YIn[j] / corr;
      EOut[j] = fabs(EIn[j] / corr);
    }
    progress.report("Solid Angle Correction");
    PARALLEL_END_INTERUPT_REGION
  }
  PARALLEL_CHECK_INTERUPT_REGION
}

void SANSSolidAngleCorrection2::execEvent() {
  MatrixWorkspace_sptr inputWS = getProperty("InputWorkspace");

  // generate the output workspace pointer
  MatrixWorkspace_sptr outputWS = getProperty("OutputWorkspace");
  if (outputWS != inputWS) {
    outputWS = inputWS->clone();
    setProperty("OutputWorkspace", outputWS);
  }
  auto outputEventWS = boost::dynamic_pointer_cast<EventWorkspace>(outputWS);

  const int numberOfSpectra =
      static_cast<int>(outputEventWS->getNumberHistograms());
  Progress progress(this, 0.0, 1.0, numberOfSpectra);
  progress.report("Solid Angle Correction");

  const auto &spectrumInfo = outputEventWS->spectrumInfo();
  const auto &componentInfo = inputWS->componentInfo();
  const double pixelSizeX = inputWS->getInstrument()->getNumberParameter("x-pixel-size")[0];
  const double pixelSizeY = inputWS->getInstrument()->getNumberParameter("y-pixel-size")[0];

  PARALLEL_FOR_IF(Kernel::threadSafe(*outputEventWS))
  for (int i = 0; i < numberOfSpectra; i++) {
    PARALLEL_START_INTERUPT_REGION

    if (!spectrumInfo.hasDetectors(i)) {
      g_log.warning() << "Workspace index " << i
                      << " has no detector assigned to it - discarding\n";
      continue;
    }

    // Skip if we have a monitor or if the detector is masked.
    if (spectrumInfo.isMonitor(i) || spectrumInfo.isMasked(i))
      continue;

    double corr = calculateSolidAngle(i, spectrumInfo, componentInfo, pixelSizeX, pixelSizeY);

    EventList &el = outputEventWS->getSpectrum(i);
    el /= corr;
    progress.report("Solid Angle Correction");
    PARALLEL_END_INTERUPT_REGION
  }
  PARALLEL_CHECK_INTERUPT_REGION
}

/**
 * Compute the solid angle
 * */
double SANSSolidAngleCorrection2::calculateSolidAngle(
    int histogramIndex, const SpectrumInfo &spectrumInfo, const ComponentInfo &componentInfo,
    const double pixelSizeX, const double pixelSizeY) {

  // Compute solid angle correction factor

  const std::string type = getProperty("Type");
  double pixelScale = pixelSizeX * componentInfo.scaleFactor(histogramIndex)[0]  
    * pixelSizeY * componentInfo.scaleFactor(histogramIndex)[1];
  double distance = spectrumInfo.l2(histogramIndex);

  double angularTerm = 0;
  if (type == "Normal"){
    double cosTheta = cos(spectrumInfo.twoTheta(histogramIndex));  
    angularTerm = cosTheta*cosTheta*cosTheta;
  }
  else if (type == "Tube"){
    double cosTheta = cos(spectrumInfo.twoTheta(histogramIndex));  
    double cosAlpha = cos(getYTubeAngle(spectrumInfo, histogramIndex));  
    angularTerm = cosTheta*cosTheta*cosAlpha;
  }
  else if (type == "Wing"){
    double cosAlpha = cos(getYTubeAngle(spectrumInfo, histogramIndex));  
    angularTerm = cosAlpha*cosAlpha*cosAlpha;
  } else {
    throw std::runtime_error("Invalid type of correction");
  }

  return (pixelScale * angularTerm) / (distance*distance);

}

} // namespace Algorithms
} // namespace Mantid
