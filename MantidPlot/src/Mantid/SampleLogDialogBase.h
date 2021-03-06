// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2016 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef SAMPLELOGDIALOGBASE_H_
#define SAMPLELOGDIALOGBASE_H_

//----------------------------------
// Includes
//----------------------------------
#include "MantidAPI/ExperimentInfo.h"
#include "MantidAPI/LogFilterGenerator.h"
#include "MantidKernel/TimeSeriesProperty.h"
#include <QDialog>
#include <memory>
//----------------------------------
// Forward declarations
//----------------------------------
class MantidUI;
class ApplicationWindow;

// Qt
class QBoxLayout;
class QRadioButton;
class QTreeWidgetItem;
class QTreeWidget;
class QPushButton;
class QLabel;
class QSpinBox;
class QLineEdit;
class QLayout;

/**
This is the base class for the Sample Log Dialog.
It provides methods to create, initialise and show the dialog window
with the log information loaded.

Original author - Martyn Gigg, Tessella Support Services plc
Refactored into base class by Dimitar Tasev

@author Dimitar Tasev, Mantid Development Team
@date 18/07/2016
*/
class SampleLogDialogBase : public QDialog {
  Q_OBJECT
public:
  /// Constructor
  SampleLogDialogBase(const QString &wsname, QWidget *parentContainer,
                      Qt::WFlags flags = nullptr,
                      size_t experimentInfoIndex = 0);

  /// Virtual Destructor for derived classes
  virtual ~SampleLogDialogBase();

protected slots:
  /// Plot logs
  virtual void importSelectedLogs();

  /// Show the stats of the selected log
  virtual void showLogStatistics();
  virtual void showLogStatisticsOfItem(
      QTreeWidgetItem *item,
      const Mantid::API::LogFilterGenerator::FilterType filter =
          Mantid::API::LogFilterGenerator::FilterType::None);

  /// Context menu popup
  virtual void popupMenu(const QPoint &pos);

  /// Import a single item
  virtual void importItem(QTreeWidgetItem *item) = 0;

  virtual void selectExpInfoNumber(int num);

protected:
  /// This function is not virtual because it is called from derived classes
  /// without overriding
  /// This function initialises everything in the tree widget
  void init();

  /// Sets the dialog's window title
  void setDialogWindowTitle(const QString &wsname);

  /// Sets the QTreeWidget column names to the default values
  void setTreeWidgetColumnNames();

  /// Adds the import and close button to the layout and connects them
  void addImportAndCloseButtonsTo(QBoxLayout *qLayout);

  /// Adds the experiment info selector to the layout
  void addExperimentInfoSelectorTo(QBoxLayout *qLayout);

  /// Sets up the QTreeWidget's connections for functionality
  void setUpTreeWidgetConnections();

  /// Which type of filtering is selected - in base class case, none
  virtual Mantid::API::LogFilterGenerator::FilterType getFilterType() const {
    return Mantid::API::LogFilterGenerator::FilterType::None;
  }

  /// A tree widget
  QTreeWidget *m_tree;

  /// The parent container of the window
  QWidget *m_parentContainer;

  /// The workspace name
  std::string m_wsname;

  /// Index into the ExperimentInfo list.
  size_t m_experimentInfoIndex;

  /// The actual experiment info being looked at.
  Mantid::API::ExperimentInfo_const_sptr m_ei;

  /// Buttons to do things
  QPushButton *buttonPlot, *buttonClose;

  /// Number of statistic values
  static const std::size_t NUM_STATS = 8;

  /// Testboxes with stats data
  QLineEdit *statValues[NUM_STATS];

  /// Widget to select the # of the experiment info to look at.
  QSpinBox *m_spinNumber;

  /// these values are used to specify the format of the log file, all of which
  /// are stored as strings
  enum logType {
    string,        ///< indicates the log is a string, no other known formatting
    numTSeries,    ///< for time series properties that contain numbers
    stringTSeries, ///< for logs that are string time series properties
    numeric,       ///< for logs that are single numeric values (int or double)
    numericArray   ///< for logs that are an array of numeric values (int or
                   /// double)
  };
};

/// Object that applies a filter to a property for as long as it is in scope.
/// When scope ends, filter is cleared.
template <typename T> class ScopedFilter {
public:
  ScopedFilter(Mantid::Kernel::TimeSeriesProperty<T> *prop,
               const std::unique_ptr<Mantid::Kernel::LogFilter> &logFilter)
      : m_prop(prop) {
    if (logFilter && logFilter->filter()) {
      m_prop->filterWith(logFilter->filter());
    }
  }
  ~ScopedFilter() { m_prop->clearFilter(); }

private:
  Mantid::Kernel::TimeSeriesProperty<T> *m_prop;
};

#endif // SAMPLELOGDIALOGBASE_H_
