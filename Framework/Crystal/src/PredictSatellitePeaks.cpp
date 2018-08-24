/*
 * PredictSatellitePeaks.cpp
 *
 *  Created on: July 15, 2018
 *      Author: Vickie Lynch
 */
#include "MantidCrystal/PredictSatellitePeaks.h"
#include "MantidAPI/Run.h"
#include "MantidAPI/Sample.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidDataObjects/PeaksWorkspace.h"
#include "MantidGeometry/Crystal/BasicHKLFilters.h"
#include "MantidGeometry/Crystal/HKLGenerator.h"
#include "MantidGeometry/Crystal/OrientedLattice.h"
#include "MantidGeometry/Objects/InstrumentRayTracer.h"
#include "MantidKernel/ArrayLengthValidator.h"
#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/EnabledWhenProperty.h"
#include <boost/math/special_functions/round.hpp>

namespace Mantid {
using namespace Mantid::DataObjects;
using namespace Mantid::API;
using namespace std;
using namespace Mantid::Geometry;
using namespace Mantid::Kernel;
namespace Crystal {

DECLARE_ALGORITHM(PredictSatellitePeaks)

/// Initialise the properties
void PredictSatellitePeaks::init() {
  declareProperty(
      make_unique<WorkspaceProperty<PeaksWorkspace>>("Peaks", "",
                                                     Direction::Input),
      "Workspace of Peaks with orientation matrix that indexed the peaks and "
      "instrument loaded");

  declareProperty(
      make_unique<WorkspaceProperty<PeaksWorkspace>>("SatellitePeaks", "",
                                                     Direction::Output),
      "Workspace of Peaks with peaks with fractional h,k, and/or l values");
  declareProperty(Kernel::make_unique<Kernel::ArrayProperty<double>>(
                      string("ModVector1"), "0.0,0.0,0.0"),
                  "Offsets for h, k, l directions ");
  declareProperty(Kernel::make_unique<Kernel::ArrayProperty<double>>(
                      string("ModVector2"), "0.0,0.0,0.0"),
                  "Offsets for h, k, l directions ");
  declareProperty(Kernel::make_unique<Kernel::ArrayProperty<double>>(
                      string("ModVector3"), "0.0,0.0,0.0"),
                  "Offsets for h, k, l directions ");
  declareProperty(
      make_unique<PropertyWithValue<int>>("MaxOrder", 0, Direction::Input),
      "Maximum order to apply ModVectors. Default = 0");

  declareProperty("GetModVectorsFromUB", true,
                  "If false Modulation Vectors will be read from input");

  declareProperty(make_unique<PropertyWithValue<bool>>("CrossTerms", false,
                                                       Direction::Input),
                  "Include cross terms (false)");

  declareProperty(
      "IncludeIntegerHKL", true,
      "If false order 0 peaks are not included in workspace (integer HKL)");

  declareProperty("IncludeAllPeaksInRange", false,
                  "If false only offsets from "
                  "peaks from Peaks workspace "
                  "in input are used");

  declareProperty(make_unique<PropertyWithValue<double>>("WavelengthMin", 0.1,
                                                         Direction::Input),
                  "Minimum wavelength limit at which to start looking for "
                  "single-crystal peaks.");
  declareProperty(make_unique<PropertyWithValue<double>>("WavelengthMax", 100.0,
                                                         Direction::Input),
                  "Maximum wavelength limit at which to start looking for "
                  "single-crystal peaks.");
  declareProperty(make_unique<PropertyWithValue<double>>("MinDSpacing", 0.1,
                                                         Direction::Input),
                  "Minimum d-spacing of peaks to consider. Default = 1.0");
  declareProperty(make_unique<PropertyWithValue<double>>("MaxDSpacing", 100.0,
                                                         Direction::Input),
                  "Maximum d-spacing of peaks to consider");

  setPropertySettings(
      "WavelengthMin",
      Kernel::make_unique<Kernel::EnabledWhenProperty>(
          string("IncludeAllPeaksInRange"), Kernel::IS_EQUAL_TO, "1"));

  setPropertySettings(
      "WavelengthMax",
      Kernel::make_unique<Kernel::EnabledWhenProperty>(
          string("IncludeAllPeaksInRange"), Kernel::IS_EQUAL_TO, "1"));
  setPropertySettings(
      "MinDSpacing",
      Kernel::make_unique<Kernel::EnabledWhenProperty>(
          string("IncludeAllPeaksInRange"), Kernel::IS_EQUAL_TO, "1"));

  setPropertySettings(
      "MaxDSpacing",
      Kernel::make_unique<Kernel::EnabledWhenProperty>(
          string("IncludeAllPeaksInRange"), Kernel::IS_EQUAL_TO, "1"));
}

/// Run the algorithm
void PredictSatellitePeaks::exec() {
  bool includePeaksInRange = getProperty("IncludeAllPeaksInRange");
  if (!includePeaksInRange) {
    exec_peaks();
    return;
  }
  PeaksWorkspace_sptr Peaks = getProperty("Peaks");
  if (!Peaks)
    throw std::invalid_argument(
        "Input workspace is not a PeaksWorkspace. Type=" + Peaks->id());

  V3D offsets1 = getOffsetVector("ModVector1");
  V3D offsets2 = getOffsetVector("ModVector2");
  V3D offsets3 = getOffsetVector("ModVector3");
  int maxOrder = getProperty("MaxOrder");
  bool crossTerms = getProperty("CrossTerms");
  bool includeOrderZero = getProperty("IncludeIntegerHKL");

  if (Peaks->getNumberPeaks() <= 0) {
    g_log.error() << "There are No peaks in the input PeaksWorkspace\n";
    return;
  }

  API::Sample sample = Peaks->mutableSample();

  OrientedLattice lattice = sample.getOrientedLattice();

  if (getProperty("GetModVectorsFromUB")) {
    offsets1 = lattice.getModVec(1);
    offsets2 = lattice.getModVec(2);
    offsets3 = lattice.getModVec(3);
    if (maxOrder == 0)
      maxOrder = lattice.getMaxOrder();
  }

  const auto instrument = Peaks->getInstrument();

  auto OutPeaks = boost::dynamic_pointer_cast<IPeaksWorkspace>(
      WorkspaceFactory::Instance().createPeaks());
  OutPeaks->setInstrument(instrument);
  OutPeaks->mutableSample().setOrientedLattice(&lattice);

  Kernel::Matrix<double> goniometer;
  goniometer.identityMatrix();

  const double lambdaMin = getProperty("WavelengthMin");
  const double lambdaMax = getProperty("WavelengthMax");
  IPeak &peak0 = Peaks->getPeak(0);

  std::vector<V3D> possibleHKLs;
  const double dMin = getProperty("MinDSpacing");
  const double dMax = getProperty("MaxDSpacing");
  Geometry::HKLGenerator gen(lattice, dMin);
  auto dSpacingFilter =
      boost::make_shared<HKLFilterDRange>(lattice, dMin, dMax);

  V3D hkl = *(gen.begin());
  g_log.information() << "HKL range for d_min of " << dMin << " to d_max of "
                      << dMax << " is from " << hkl << " to " << hkl * -1.0
                      << ", a total of " << gen.size() << " possible HKL's\n";
  if (gen.size() > MAX_NUMBER_HKLS)
    throw std::invalid_argument("More than 10 billion HKLs to search. Is "
                                "your d_min value too small?");

  possibleHKLs.clear();
  possibleHKLs.reserve(gen.size());
  std::remove_copy_if(gen.begin(), gen.end(), std::back_inserter(possibleHKLs),
                      (~dSpacingFilter)->fn());

  size_t N = possibleHKLs.size();
  N = max<size_t>(100, N);
  auto &UB = lattice.getUB();
  goniometer = peak0.getGoniometerMatrix();
  Progress prog(this, 0.0, 1.0, N);
  vector<vector<int>> AlreadyDonePeaks;
  auto orientedUB = goniometer * UB;
  HKLFilterWavelength lambdaFilter(orientedUB, lambdaMin, lambdaMax);
  int seqNum = 0;
  OutPeaks->mutableRun().addProperty<std::vector<double>>("Offset1", offsets1,
                                                          true);
  OutPeaks->mutableRun().addProperty<std::vector<double>>("Offset2", offsets2,
                                                          true);
  OutPeaks->mutableRun().addProperty<std::vector<double>>("Offset3", offsets3,
                                                          true);
  for (auto it = possibleHKLs.begin(); it != possibleHKLs.end(); ++it) {
    V3D hkl = *it;
    if (crossTerms) {
      predictOffsetsWithCrossTerms(Peaks, OutPeaks, offsets1, offsets2,
                                   offsets3, maxOrder, hkl, lambdaFilter,
                                   includePeaksInRange, includeOrderZero,
                                   seqNum, AlreadyDonePeaks);
    } else {
      predictOffsets(Peaks, OutPeaks, 0, offsets1, maxOrder, hkl, lambdaFilter,
                     includePeaksInRange, includeOrderZero, seqNum,
                     AlreadyDonePeaks);
      predictOffsets(Peaks, OutPeaks, 1, offsets2, maxOrder, hkl, lambdaFilter,
                     includePeaksInRange, includeOrderZero, seqNum,
                     AlreadyDonePeaks);
      predictOffsets(Peaks, OutPeaks, 2, offsets3, maxOrder, hkl, lambdaFilter,
                     includePeaksInRange, includeOrderZero, seqNum,
                     AlreadyDonePeaks);
    }
  }
  setProperty("SatellitePeaks", OutPeaks);
}

void PredictSatellitePeaks::exec_peaks() {
  PeaksWorkspace_sptr Peaks = getProperty("Peaks");
  if (!Peaks)
    throw std::invalid_argument(
        "Input workspace is not a PeaksWorkspace. Type=" + Peaks->id());

  V3D offsets1 = getOffsetVector("ModVector1");
  V3D offsets2 = getOffsetVector("ModVector2");
  V3D offsets3 = getOffsetVector("ModVector3");
  int maxOrder = getProperty("MaxOrder");
  bool crossTerms = getProperty("CrossTerms");

  bool includePeaksInRange = false;
  bool includeOrderZero = getProperty("IncludeIntegerHKL");

  if (Peaks->getNumberPeaks() <= 0) {
    g_log.error() << "There are No peaks in the input PeaksWorkspace\n";
    return;
  }

  API::Sample sample = Peaks->mutableSample();

  OrientedLattice lattice = sample.getOrientedLattice();

  const auto instrument = Peaks->getInstrument();

  auto OutPeaks = boost::dynamic_pointer_cast<IPeaksWorkspace>(
      WorkspaceFactory::Instance().createPeaks());
  OutPeaks->setInstrument(instrument);
  OutPeaks->mutableSample().setOrientedLattice(&lattice);

  vector<vector<int>> AlreadyDonePeaks;
  int seqNum = 0;
  HKLFilterWavelength lambdaFilter(DblMatrix(3, 3, true), 0.1, 100.);
  OutPeaks->mutableRun().addProperty<std::vector<double>>("Offset1", offsets1,
                                                          true);
  OutPeaks->mutableRun().addProperty<std::vector<double>>("Offset2", offsets2,
                                                          true);
  OutPeaks->mutableRun().addProperty<std::vector<double>>("Offset3", offsets3,
                                                          true);
  std::vector<Peak> peaks = Peaks->getPeaks();
  for (auto it = peaks.begin(); it != peaks.end(); ++it) {
    auto peak = *it;
    V3D hkl = peak.getHKL();
    if (crossTerms) {
      predictOffsetsWithCrossTerms(Peaks, OutPeaks, offsets1, offsets2,
                                   offsets3, maxOrder, hkl, lambdaFilter,
                                   includePeaksInRange, includeOrderZero,
                                   seqNum, AlreadyDonePeaks);
    } else {
      predictOffsets(Peaks, OutPeaks, 0, offsets1, maxOrder, hkl, lambdaFilter,
                     includePeaksInRange, includeOrderZero, seqNum,
                     AlreadyDonePeaks);
      predictOffsets(Peaks, OutPeaks, 1, offsets2, maxOrder, hkl, lambdaFilter,
                     includePeaksInRange, includeOrderZero, seqNum,
                     AlreadyDonePeaks);
      predictOffsets(Peaks, OutPeaks, 2, offsets3, maxOrder, hkl, lambdaFilter,
                     includePeaksInRange, includeOrderZero, seqNum,
                     AlreadyDonePeaks);
    }
  }
  setProperty("SatellitePeaks", OutPeaks);
}

void PredictSatellitePeaks::predictOffsets(
    DataObjects::PeaksWorkspace_sptr Peaks,
    boost::shared_ptr<Mantid::API::IPeaksWorkspace> &OutPeaks, int iVector,
    V3D offsets, int &maxOrder, V3D &hkl, HKLFilterWavelength &lambdaFilter,
    bool &includePeaksInRange, bool &includeOrderZero, int &seqNum,
    vector<vector<int>> &AlreadyDonePeaks) {
  if (offsets == V3D(0, 0, 0))
    return;
  const Kernel::DblMatrix &UB = Peaks->sample().getOrientedLattice().getUB();
  IPeak &peak1 = Peaks->getPeak(0);
  Kernel::Matrix<double> goniometer = peak1.getGoniometerMatrix();
  auto RunNumber = peak1.getRunNumber();
  Geometry::InstrumentRayTracer tracer(Peaks->getInstrument());
  for (int order = -maxOrder; order <= maxOrder; order++) {
    if (order == 0 && !includeOrderZero)
      continue; // exclude order 0
    V3D satelliteHKL(hkl);

    satelliteHKL += offsets * order;
    if (!lambdaFilter.isAllowed(satelliteHKL) && includePeaksInRange)
      continue;

    Kernel::V3D Qs = goniometer * UB * satelliteHKL * 2.0 * M_PI;

    // Check if Q is non-physical
    if (Qs[2] <= 0)
      continue;

    auto peak(Peaks->createPeak(Qs, 1));

    peak->setGoniometerMatrix(goniometer);

    if (!peak->findDetector(tracer))
      continue;
    vector<int> SavPk{RunNumber, boost::math::iround(1000.0 * satelliteHKL[0]),
                      boost::math::iround(1000.0 * satelliteHKL[1]),
                      boost::math::iround(1000.0 * satelliteHKL[2])};

    bool foundPeak =
        binary_search(AlreadyDonePeaks.begin(), AlreadyDonePeaks.end(), SavPk);

    if (!foundPeak) {
      AlreadyDonePeaks.push_back(SavPk);
    } else {
      continue;
    }

    peak->setHKL(satelliteHKL);
    peak->setIntHKL(hkl);
    peak->setPeakNumber(seqNum);
    seqNum++;
    peak->setRunNumber(RunNumber);
    V3D mnp;
    mnp[iVector] = order;
    peak->setIntMNP(mnp);
    OutPeaks->addPeak(*peak);
  }
}

void PredictSatellitePeaks::predictOffsetsWithCrossTerms(
    DataObjects::PeaksWorkspace_sptr Peaks,
    boost::shared_ptr<Mantid::API::IPeaksWorkspace> &OutPeaks, V3D offsets1,
    V3D offsets2, V3D offsets3, int &maxOrder, V3D &hkl,
    HKLFilterWavelength &lambdaFilter, bool &includePeaksInRange,
    bool &includeOrderZero, int &seqNum,
    vector<vector<int>> &AlreadyDonePeaks) {
  if (offsets1 == V3D(0, 0, 0) && offsets2 == V3D(0, 0, 0) &&
      offsets3 == V3D(0, 0, 0))
    return;
  const Kernel::DblMatrix &UB = Peaks->sample().getOrientedLattice().getUB();
  IPeak &peak1 = Peaks->getPeak(0);
  Kernel::Matrix<double> goniometer = peak1.getGoniometerMatrix();
  auto RunNumber = peak1.getRunNumber();
  Geometry::InstrumentRayTracer tracer(Peaks->getInstrument());
  DblMatrix offsetsMat(3, 3);
  offsetsMat.setColumn(0, offsets1);
  offsetsMat.setColumn(1, offsets2);
  offsetsMat.setColumn(2, offsets3);
  int maxOrder1 = maxOrder;
  if (offsets1 == V3D(0, 0, 0))
    maxOrder1 = 0;
  int maxOrder2 = maxOrder;
  if (offsets2 == V3D(0, 0, 0))
    maxOrder2 = 0;
  int maxOrder3 = maxOrder;
  if (offsets3 == V3D(0, 0, 0))
    maxOrder3 = 0;
  for (int m = -maxOrder1; m <= maxOrder1; m++)
    for (int n = -maxOrder2; n <= maxOrder2; n++)
      for (int p = -maxOrder3; p <= maxOrder3; p++) {
        if (m == 0 && n == 0 && p == 0 && !includeOrderZero)
          continue; // exclude 0,0,0
        V3D satelliteHKL(hkl);
        V3D mnp = V3D(m, n, p);
        satelliteHKL -= offsetsMat * mnp;
        if (!lambdaFilter.isAllowed(satelliteHKL) && includePeaksInRange)
          continue;

        Kernel::V3D Qs = goniometer * UB * satelliteHKL * 2.0 * M_PI;

        // Check if Q is non-physical
        if (Qs[2] <= 0)
          continue;

        auto peak(Peaks->createPeak(Qs, 1));

        peak->setGoniometerMatrix(goniometer);

        if (!peak->findDetector(tracer))
          continue;
        vector<int> SavPk{RunNumber,
                          boost::math::iround(1000.0 * satelliteHKL[0]),
                          boost::math::iround(1000.0 * satelliteHKL[1]),
                          boost::math::iround(1000.0 * satelliteHKL[2])};

        bool foundPeak = binary_search(AlreadyDonePeaks.begin(),
                                       AlreadyDonePeaks.end(), SavPk);

        if (!foundPeak) {
          AlreadyDonePeaks.push_back(SavPk);
        } else {
          continue;
        }

        peak->setHKL(satelliteHKL);
        peak->setIntHKL(hkl);
        peak->setPeakNumber(seqNum);
        seqNum++;
        peak->setRunNumber(RunNumber);
        peak->setIntMNP(V3D(m, n, p));
        OutPeaks->addPeak(*peak);
      }
}

V3D PredictSatellitePeaks::getOffsetVector(const std::string &label) {
  vector<double> offsets = getProperty(label);
  if (offsets.empty()) {
    offsets.push_back(0.0);
    offsets.push_back(0.0);
    offsets.push_back(0.0);
  }
  V3D offsets1 = V3D(offsets[0], offsets[1], offsets[2]);
  return offsets1;
}

} // namespace Crystal
} // namespace Mantid