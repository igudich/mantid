#include "Iqt.h"
#include "../General/UserInputValidator.h"

#include "MantidAPI/ITableWorkspace.h"
#include "MantidGeometry/Instrument.h"
#include "MantidQtWidgets/Common/SignalBlocker.h"
#include "MantidQtWidgets/LegacyQwt/RangeSelector.h"

#include <qwt_plot.h>

#include <tuple>

namespace {
Mantid::Kernel::Logger g_log("Iqt");

/**
 * Calculate the number of bins in the sample & resolution workspaces
 * @param wsName The sample workspace name
 * @param resName the resolution woskapce name
 * @param energyMin Minimum energy for chosen bin range
 * @param energyMax Maximum energy for chosen bin range
 * @param binReductionFactor The factor by which to reduce the number of bins
 * @return A 4-tuple where the first entry denotes whether the
 * calculation was successful or not. The final 3 values
 * are EWidth, SampleBins, ResolutionBins if the calculation succeeded,
 * otherwise they are undefined.
 */
std::tuple<bool, float, int, int>
calculateBinParameters(QString wsName, QString resName, double energyMin,
                       double energyMax, double binReductionFactor) {
  using namespace Mantid::API;
  ITableWorkspace_sptr propsTable;
  const auto paramTableName = "__IqtProperties_temp";
  try {
    auto toIqt = AlgorithmManager::Instance().createUnmanaged("TransformToIqt");
    toIqt->initialize();
    toIqt->setChild(true); // record this as internal
    toIqt->setProperty("SampleWorkspace", wsName.toStdString());
    toIqt->setProperty("ResolutionWorkspace", resName.toStdString());
    toIqt->setProperty("ParameterWorkspace", paramTableName);
    toIqt->setProperty("EnergyMin", energyMin);
    toIqt->setProperty("EnergyMax", energyMax);
    toIqt->setProperty("BinReductionFactor", binReductionFactor);
    toIqt->setProperty("DryRun", true);
    toIqt->execute();
    propsTable = toIqt->getProperty("ParameterWorkspace");
    // the algorithm can create output even if it failed...
    IAlgorithm_sptr deleteAlg =
        AlgorithmManager::Instance().create("DeleteWorkspace");
    deleteAlg->initialize();
    deleteAlg->setChild(true);
    deleteAlg->setProperty("Workspace", paramTableName);
    deleteAlg->execute();
  } catch (std::exception &) {
    return std::make_tuple(false, 0.0f, 0, 0);
  }
  assert(propsTable);
  return std::make_tuple(
      true, propsTable->getColumn("EnergyWidth")->cell<float>(0),
      propsTable->getColumn("SampleOutputBins")->cell<int>(0),
      propsTable->getColumn("ResolutionBins")->cell<int>(0));
}
} // namespace

using namespace Mantid::API;

namespace MantidQt {
namespace CustomInterfaces {
namespace IDA {
Iqt::Iqt(QWidget *parent)
    : IndirectDataAnalysisTab(parent), m_iqtTree(nullptr), m_iqtResFileType() {
  m_uiForm.setupUi(parent);
}

void Iqt::setup() {
  m_iqtTree = new QtTreePropertyBrowser();
  m_uiForm.properties->addWidget(m_iqtTree);

  // Create and configure properties
  m_properties["ELow"] = m_dblManager->addProperty("ELow");
  m_dblManager->setDecimals(m_properties["ELow"], NUM_DECIMALS);

  m_properties["EWidth"] = m_dblManager->addProperty("EWidth");
  m_dblManager->setDecimals(m_properties["EWidth"], NUM_DECIMALS);
  m_properties["EWidth"]->setEnabled(false);

  m_properties["EHigh"] = m_dblManager->addProperty("EHigh");
  m_dblManager->setDecimals(m_properties["EHigh"], NUM_DECIMALS);

  m_properties["SampleBinning"] = m_dblManager->addProperty("SampleBinning");
  m_dblManager->setDecimals(m_properties["SampleBinning"], 0);

  m_properties["SampleBins"] = m_dblManager->addProperty("SampleBins");
  m_dblManager->setDecimals(m_properties["SampleBins"], 0);
  m_properties["SampleBins"]->setEnabled(false);

  m_properties["ResolutionBins"] = m_dblManager->addProperty("ResolutionBins");
  m_dblManager->setDecimals(m_properties["ResolutionBins"], 0);
  m_properties["ResolutionBins"]->setEnabled(false);

  m_iqtTree->addProperty(m_properties["ELow"]);
  m_iqtTree->addProperty(m_properties["EWidth"]);
  m_iqtTree->addProperty(m_properties["EHigh"]);
  m_iqtTree->addProperty(m_properties["SampleBinning"]);
  m_iqtTree->addProperty(m_properties["SampleBins"]);
  m_iqtTree->addProperty(m_properties["ResolutionBins"]);

  m_dblManager->setValue(m_properties["SampleBinning"], 10);

  m_iqtTree->setFactoryForManager(m_dblManager, m_dblEdFac);

  auto xRangeSelector = m_uiForm.ppPlot->addRangeSelector("IqtRange");

  // signals / slots & validators
  connect(xRangeSelector, SIGNAL(selectionChangedLazy(double, double)), this,
          SLOT(rsRangeChangedLazy(double, double)));
  connect(m_dblManager, SIGNAL(valueChanged(QtProperty *, double)), this,
          SLOT(updateRS(QtProperty *, double)));
  connect(m_dblManager, SIGNAL(valueChanged(QtProperty *, double)), this,
          SLOT(updatePropertyValues(QtProperty *, double)));
  connect(m_uiForm.dsInput, SIGNAL(dataReady(const QString &)), this,
          SLOT(plotInput(const QString &)));
  connect(m_uiForm.dsResolution, SIGNAL(dataReady(const QString &)), this,
          SLOT(updateDisplayedBinParameters()));
  connect(m_batchAlgoRunner, SIGNAL(batchComplete(bool)), this,
          SLOT(algorithmComplete(bool)));
  connect(m_uiForm.pbRun, SIGNAL(clicked()), this, SLOT(runClicked()));
  connect(m_uiForm.pbSave, SIGNAL(clicked()), this, SLOT(saveClicked()));
  connect(m_uiForm.pbPlot, SIGNAL(clicked()), this, SLOT(plotClicked()));
  connect(m_uiForm.pbTile, SIGNAL(clicked()), this, SLOT(plotTiled()));
  connect(m_uiForm.pbPlotPreview, SIGNAL(clicked()), this,
          SLOT(plotCurrentPreview()));
  connect(m_uiForm.cbCalculateErrors, SIGNAL(clicked()), this,
          SLOT(errorsClicked()));
  connect(m_uiForm.spTiledPlotFirst, SIGNAL(valueChanged(int)), this,
          SLOT(setTiledPlotFirstPlot(int)));
  connect(m_uiForm.spTiledPlotLast, SIGNAL(valueChanged(int)), this,
          SLOT(setTiledPlotLastPlot(int)));
}

void Iqt::run() {
  using namespace Mantid::API;

  setRunIsRunning(true);

  updateDisplayedBinParameters();

  // Construct the result workspace for Python script export
  QString sampleName = m_uiForm.dsInput->getCurrentDataName();
  m_pythonExportWsName =
      sampleName.left(sampleName.lastIndexOf("_")).toStdString() + "_iqt";

  QString wsName = m_uiForm.dsInput->getCurrentDataName();
  QString resName = m_uiForm.dsResolution->getCurrentDataName();
  QString nIterations = m_uiForm.spIterations->cleanText();
  bool calculateErrors = m_uiForm.cbCalculateErrors->isChecked();

  double energyMin = m_dblManager->value(m_properties["ELow"]);
  double energyMax = m_dblManager->value(m_properties["EHigh"]);
  double numBins = m_dblManager->value(m_properties["SampleBinning"]);

  IAlgorithm_sptr IqtAlg =
      AlgorithmManager::Instance().create("TransformToIqt");
  IqtAlg->initialize();

  IqtAlg->setProperty("SampleWorkspace", wsName.toStdString());
  IqtAlg->setProperty("ResolutionWorkspace", resName.toStdString());
  IqtAlg->setProperty("NumberOfIterations", nIterations.toStdString());
  IqtAlg->setProperty("CalculateErrors", calculateErrors);

  IqtAlg->setProperty("EnergyMin", energyMin);
  IqtAlg->setProperty("EnergyMax", energyMax);
  IqtAlg->setProperty("BinReductionFactor", numBins);
  IqtAlg->setProperty("OutputWorkspace", m_pythonExportWsName);

  IqtAlg->setProperty("DryRun", false);

  m_batchAlgoRunner->addAlgorithm(IqtAlg);
  m_batchAlgoRunner->executeBatchAsync();
}

/**
 * Handle algorithm completion.
 *
 * @param error If the algorithm failed
 */
void Iqt::algorithmComplete(bool error) {
  setRunIsRunning(false);
  if (!error) {
    setPlotResultEnabled(true);
    setTiledPlotEnabled(true);
    setSaveResultEnabled(true);

    setPlotSpectrumIndexMax(static_cast<int>(getNumberOfSpectra()) - 1);
    setPlotSpectrumIndex(selectedSpectrum());
    setTiledPlotFirstIndex(selectedSpectrum());
    setTiledPlotLastIndex(static_cast<int>(getNumberOfSpectra()) - 1);
  }
}
/**
 * Handle saving of workspace
 */
void Iqt::saveClicked() {
  checkADSForPlotSaveWorkspace(m_pythonExportWsName, false);
  addSaveWorkspaceToQueue(QString::fromStdString(m_pythonExportWsName));
  m_batchAlgoRunner->executeBatchAsync();
}

/**
 * Handle mantid plotting
 */
void Iqt::plotClicked() {
  checkADSForPlotSaveWorkspace(m_pythonExportWsName, false);
  setPlotResultIsPlotting(true);
  plotSpectrum(QString::fromStdString(m_pythonExportWsName),
               getPlotSpectrumIndex());
  setPlotResultIsPlotting(false);
}

void Iqt::errorsClicked() {
  m_uiForm.spIterations->setEnabled(isErrorsEnabled());
}

bool Iqt::isErrorsEnabled() { return m_uiForm.cbCalculateErrors->isChecked(); }

std::size_t Iqt::getXMinIndex(Mantid::MantidVec const &yData,
                              std::vector<double>::const_iterator iter) {
  auto cropIndex = yData.size() - 1;
  if (iter != yData.end()) {
    auto const index = iter - yData.begin();
    cropIndex = index > 0 ? index - 1 : index;
  } else
    showMessageBox(
        "Incorrect data provided for Tiled Plot: y values are out of range");
  return cropIndex;
}

double Iqt::getXMinValue(MatrixWorkspace_const_sptr workspace,
                         std::size_t const &index) {
  auto const firstSpectraYData = workspace->dataY(index);
  auto const positionIter =
      std::find_if(firstSpectraYData.begin(), firstSpectraYData.end(),
                   [&](double const &value) { return value < 1.0; });
  auto const cropIndex = getXMinIndex(firstSpectraYData, positionIter);
  return workspace->dataX(index)[cropIndex];
}

void Iqt::plotTiled() {
  setTiledPlotIsPlotting(true);

  MatrixWorkspace_const_sptr outWs =
      AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(
          m_pythonExportWsName);

  auto const tiledPlotWsName = outWs->getName() + "_tiled";
  auto const firstTiledPlot = m_uiForm.spTiledPlotFirst->text().toInt();
  auto const lastTiledPlot = m_uiForm.spTiledPlotLast->text().toInt();

  // Get first x value which corresponds to a y value below 1
  auto const cropValue =
      getXMinValue(outWs, static_cast<std::size_t>(firstTiledPlot));

  // Clone workspace before cropping to keep in ADS
  IAlgorithm_sptr clone = AlgorithmManager::Instance().create("CloneWorkspace");
  clone->initialize();
  clone->setProperty("InputWorkspace", outWs->getName());
  clone->setProperty("OutputWorkspace", tiledPlotWsName);
  clone->execute();

  // Crop based on selected first and last spectra
  IAlgorithm_sptr crop = AlgorithmManager::Instance().create("CropWorkspace");
  crop->initialize();
  crop->setProperty("InputWorkspace", tiledPlotWsName);
  crop->setProperty("OutputWorkspace", tiledPlotWsName);
  crop->setProperty("StartWorkspaceIndex", firstTiledPlot);
  crop->setProperty("EndWorkspaceIndex", lastTiledPlot);
  crop->setProperty("XMin", cropValue);
  crop->execute();
  MatrixWorkspace_const_sptr tiledPlotWs =
      AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(
          tiledPlotWsName);

  // Plot tiledwindow
  std::size_t const numberOfPlots = lastTiledPlot - firstTiledPlot + 1;
  if (numberOfPlots != 0) {
    QString pyInput = "from mantidplot import newTiledWindow\n";
    pyInput += "newTiledWindow(sources=[";
    for (auto index = firstTiledPlot; index <= lastTiledPlot; ++index) {
      if (index > firstTiledPlot) {
        pyInput += ",";
      }
      std::string const pyInStr =
          "(['" + tiledPlotWsName + "'], " + std::to_string(index) + ")";
      pyInput += QString::fromStdString(pyInStr);
    }
    pyInput += "])\n";
    runPythonCode(pyInput);
  }
  setTiledPlotIsPlotting(false);
}

/**
 * Ensure we have present and valid file/ws inputs.
 *
 * The underlying Fourier transform of Iqt
 * also means we must enforce several rules on the parameters.
 */
bool Iqt::validate() {
  UserInputValidator uiv;

  uiv.checkDataSelectorIsValid("Sample", m_uiForm.dsInput);
  uiv.checkDataSelectorIsValid("Resolution", m_uiForm.dsResolution);

  const auto eLow = m_dblManager->value(m_properties["ELow"]);
  const auto eHigh = m_dblManager->value(m_properties["EHigh"]);
  if (eLow >= eHigh)
    uiv.addErrorMessage("ELow must be strictly less than EHigh.\n");

  QString message = uiv.generateErrorMessage();
  showMessageBox(message);

  return message.isEmpty();
}

/**
 * Ensures that absolute min and max energy are equal.
 *
 * @param prop Qt property that was changed
 * @param val New value of that property
 */
void Iqt::updatePropertyValues(QtProperty *prop, double val) {
  disconnect(m_dblManager, SIGNAL(valueChanged(QtProperty *, double)), this,
             SLOT(updatePropertyValues(QtProperty *, double)));

  if (prop == m_properties["EHigh"]) {
    // If the user enters a negative value for EHigh assume they did not mean to
    // add a -
    if (val < 0) {
      val = -val;
      m_dblManager->setValue(m_properties["EHigh"], val);
    }

    m_dblManager->setValue(m_properties["ELow"], -val);
  } else if (prop == m_properties["ELow"]) {
    // If the user enters a positive value for ELow, assume they meant to add a
    if (val > 0) {
      val = -val;
      m_dblManager->setValue(m_properties["ELow"], val);
    }

    m_dblManager->setValue(m_properties["EHigh"], -val);
  }

  connect(m_dblManager, SIGNAL(valueChanged(QtProperty *, double)), this,
          SLOT(updatePropertyValues(QtProperty *, double)));

  updateDisplayedBinParameters();
}

/**
 * Calculates binning parameters.
 */
void Iqt::updateDisplayedBinParameters() {
  QString wsName = m_uiForm.dsInput->getCurrentDataName();
  QString resName = m_uiForm.dsResolution->getCurrentDataName();
  if (wsName.isEmpty() || resName.isEmpty())
    return;

  double energyMin = m_dblManager->value(m_properties["ELow"]);
  double energyMax = m_dblManager->value(m_properties["EHigh"]);
  double numBins = m_dblManager->value(m_properties["SampleBinning"]);

  if (numBins == 0)
    return;
  if (energyMin == 0 && energyMax == 0)
    return;

  bool success(false);
  float energyWidth(0.0f);
  int resolutionBins(0), sampleBins(0);
  std::tie(success, energyWidth, sampleBins, resolutionBins) =
      calculateBinParameters(wsName, resName, energyMin, energyMax, numBins);
  if (success) {
    disconnect(m_dblManager, SIGNAL(valueChanged(QtProperty *, double)), this,
               SLOT(updatePropertyValues(QtProperty *, double)));
    // Update data in property editor
    m_dblManager->setValue(m_properties["EWidth"], energyWidth);
    m_dblManager->setValue(m_properties["ResolutionBins"], resolutionBins);
    m_dblManager->setValue(m_properties["SampleBins"], sampleBins);
    connect(m_dblManager, SIGNAL(valueChanged(QtProperty *, double)), this,
            SLOT(updatePropertyValues(QtProperty *, double)));

    // Warn for low number of resolution bins
    if (resolutionBins < 5)
      showMessageBox(
          "Number of resolution bins is less than 5.\nResults may be "
          "inaccurate.");
  }
}

void Iqt::loadSettings(const QSettings &settings) {
  m_uiForm.dsInput->readSettings(settings.group());
  m_uiForm.dsResolution->readSettings(settings.group());
}

void Iqt::plotInput(const QString &wsname) {
  MatrixWorkspace_sptr workspace;
  try {
    workspace = Mantid::API::AnalysisDataService::Instance()
                    .retrieveWS<MatrixWorkspace>(wsname.toStdString());
    setInputWorkspace(workspace);
  } catch (Mantid::Kernel::Exception::NotFoundError &) {
    showMessageBox(QString("Unable to retrieve workspace: " + wsname));
    return;
  }

  IndirectDataAnalysisTab::plotInput(m_uiForm.ppPlot);
  auto xRangeSelector = m_uiForm.ppPlot->getRangeSelector("IqtRange");

  try {
    QPair<double, double> range = m_uiForm.ppPlot->getCurveRange("Sample");
    double rounded_min(range.first);
    double rounded_max(range.second);
    const std::string instrName(workspace->getInstrument()->getName());
    if (instrName == "BASIS") {
      xRangeSelector->setRange(range.first, range.second);
      m_dblManager->setValue(m_properties["ELow"], rounded_min);
      m_dblManager->setValue(m_properties["EHigh"], rounded_max);
      m_dblManager->setValue(m_properties["EWidth"], 0.0004);
      m_dblManager->setValue(m_properties["SampleBinning"], 1);
    } else {
      rounded_min = floor(rounded_min * 10 + 0.5) / 10.0;
      rounded_max = floor(rounded_max * 10 + 0.5) / 10.0;

      // corrections for if nearest value is outside of range
      if (rounded_max > range.second) {
        rounded_max -= 0.1;
      }

      if (rounded_min < range.first) {
        rounded_min += 0.1;
      }

      // check incase we have a really small range
      if (fabs(rounded_min) > 0 && fabs(rounded_max) > 0) {
        xRangeSelector->setRange(rounded_min, rounded_max);
        m_dblManager->setValue(m_properties["ELow"], rounded_min);
        m_dblManager->setValue(m_properties["EHigh"], rounded_max);
      } else {
        xRangeSelector->setRange(range.first, range.second);
        m_dblManager->setValue(m_properties["ELow"], range.first);
        m_dblManager->setValue(m_properties["EHigh"], range.second);
      }
      // set default value for width
      m_dblManager->setValue(m_properties["EWidth"], 0.005);
    }
  } catch (std::invalid_argument &exc) {
    showMessageBox(exc.what());
  }

  updateDisplayedBinParameters();
}

/**
 * Updates the range selectors and properties when range selector is moved.
 *
 * @param min Range selector min value
 * @param max Range selector amx value
 */
void Iqt::rsRangeChangedLazy(double min, double max) {
  double oldMin = m_dblManager->value(m_properties["ELow"]);
  double oldMax = m_dblManager->value(m_properties["EHigh"]);

  if (fabs(oldMin - min) > 0.0000001)
    m_dblManager->setValue(m_properties["ELow"], min);

  if (fabs(oldMax - max) > 0.0000001)
    m_dblManager->setValue(m_properties["EHigh"], max);
}

void Iqt::updateRS(QtProperty *prop, double val) {
  auto xRangeSelector = m_uiForm.ppPlot->getRangeSelector("IqtRange");

  if (prop == m_properties["ELow"])
    xRangeSelector->setMinimum(val);
  else if (prop == m_properties["EHigh"])
    xRangeSelector->setMaximum(val);
}

void Iqt::setTiledPlotFirstPlot(int value) {
  MantidQt::API::SignalBlocker<QObject> blocker(m_uiForm.spTiledPlotFirst);
  auto lastPlotIndex = m_uiForm.spTiledPlotLast->text().toInt();
  auto firstPlotMinimum = lastPlotIndex - m_maxTiledPlots >= 0
                              ? lastPlotIndex - m_maxTiledPlots
                              : 0;
  setMinMaxOfTiledPlotFirstIndex(firstPlotMinimum, lastPlotIndex);
  if (lastPlotIndex - value <= m_maxTiledPlots)
    setMinMaxOfTiledPlotLastIndex(value, value + m_maxTiledPlots);
  setTiledPlotFirstIndex(value);
}

void Iqt::setTiledPlotLastPlot(int value) {
  MantidQt::API::SignalBlocker<QObject> blocker(m_uiForm.spTiledPlotLast);
  auto firstPlotIndex = m_uiForm.spTiledPlotFirst->text().toInt();
  setMinMaxOfTiledPlotLastIndex(firstPlotIndex,
                                firstPlotIndex + m_maxTiledPlots);
  if (value - firstPlotIndex <= m_maxTiledPlots && value - m_maxTiledPlots >= 0)
    setMinMaxOfTiledPlotFirstIndex(value - m_maxTiledPlots, value);
  setTiledPlotLastIndex(value);
}

void Iqt::setMinMaxOfTiledPlotFirstIndex(int minimum, int maximum) {
  m_uiForm.spTiledPlotFirst->setMinimum(minimum);
  m_uiForm.spTiledPlotFirst->setMaximum(maximum);
}

void Iqt::setMinMaxOfTiledPlotLastIndex(int minimum, int maximum) {
  m_uiForm.spTiledPlotLast->setMinimum(minimum);
  m_uiForm.spTiledPlotLast->setMaximum(maximum);
}

void Iqt::setTiledPlotFirstIndex(int value) {
  MantidQt::API::SignalBlocker<QObject> blocker(m_uiForm.spTiledPlotFirst);
  m_uiForm.spTiledPlotFirst->setValue(value);
}

void Iqt::setTiledPlotLastIndex(int value) {
  MantidQt::API::SignalBlocker<QObject> blocker(m_uiForm.spTiledPlotLast);
  auto firstPlotIndex = m_uiForm.spTiledPlotFirst->text().toInt();
  auto lastPlotIndex = value - m_maxTiledPlots > firstPlotIndex
                           ? firstPlotIndex + m_maxTiledPlots
                           : value;
  m_uiForm.spTiledPlotLast->setValue(lastPlotIndex);
}

void Iqt::setPlotSpectrumIndexMax(int maximum) {
  MantidQt::API::SignalBlocker<QObject> blocker(m_uiForm.spSpectrum);
  m_uiForm.spSpectrum->setMaximum(maximum);
}

void Iqt::setPlotSpectrumIndex(int spectrum) {
  MantidQt::API::SignalBlocker<QObject> blocker(m_uiForm.spSpectrum);
  m_uiForm.spSpectrum->setValue(spectrum);
}

int Iqt::getPlotSpectrumIndex() { return m_uiForm.spSpectrum->text().toInt(); }

void Iqt::setRunEnabled(bool enabled) { m_uiForm.pbRun->setEnabled(enabled); }

void Iqt::setPlotResultEnabled(bool enabled) {
  m_uiForm.pbPlot->setEnabled(enabled);
  m_uiForm.spSpectrum->setEnabled(enabled);
}

void Iqt::setTiledPlotEnabled(bool enabled) {
  m_uiForm.pbTile->setEnabled(enabled);
  m_uiForm.spTiledPlotFirst->setEnabled(enabled);
  m_uiForm.spTiledPlotLast->setEnabled(enabled);
}

void Iqt::setSaveResultEnabled(bool enabled) {
  m_uiForm.pbSave->setEnabled(enabled);
}

void Iqt::setRunIsRunning(bool running) {
  m_uiForm.pbRun->setText(running ? "Running..." : "Run");
  setRunEnabled(!running);
}

void Iqt::setPlotResultIsPlotting(bool plotting) {
  m_uiForm.pbPlot->setText(plotting ? "Plotting..." : "Plot Spectrum");
  setPlotResultEnabled(!plotting);
}

void Iqt::setTiledPlotIsPlotting(bool plotting) {
  m_uiForm.pbTile->setText(plotting ? "Plotting..." : "Tiled Plot");
  setTiledPlotEnabled(!plotting);
}

void Iqt::runClicked() { runTab(); }

} // namespace IDA
} // namespace CustomInterfaces
} // namespace MantidQt
