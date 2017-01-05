#ifndef MANTID_SLICEVIEWER_NONORTHOGONALOVERLAY_H_
#define MANTID_SLICEVIEWER_NONORTHOGONALOVERLAY_H_

#include "DllOption.h"
#include <QtCore/QtCore>
#include <QtGui/qwidget.h>
#include <qwt_plot.h>
#include <qwt_valuelist.h>
#include <qpainter.h>
#include "MantidKernel/System.h"
#include "MantidQtAPI/QwtRasterDataMD.h"
#include "MantidQtAPI/QwtRasterDataMDNonOrthogonal.h"
#include "MantidAPI/IMDEventWorkspace.h"
#include "MantidAPI/IMDHistoWorkspace.h"
#include "MantidAPI/IMDWorkspace.h"
#include "MantidQtAPI/NonOrthogonal.h"
#include "MantidKernel/Matrix.h"

namespace MantidQt {
namespace SliceViewer {

/** GUI for overlaying a nonorthogonal axes onto the plot
  in the SliceViewer. Should be generic to overlays on any QwtPlot.

  @date 2016-08-23

  Copyright &copy; 2011 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
  National Laboratory & European Spallation Source

  This file is part of Mantid.

  Mantid is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  Mantid is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  File change history is stored at: <https://github.com/mantidproject/mantid>
  Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
class EXPORT_OPT_MANTIDQT_SLICEVIEWER NonOrthogonalOverlay : public QWidget {
  Q_OBJECT

public:
  NonOrthogonalOverlay(QwtPlot *plot, QWidget *parent);
  ~NonOrthogonalOverlay() override;

  void calculateAxesSkew(Mantid::API::IMDWorkspace_sptr *ws, size_t dimX,
                         size_t dimY, Mantid::Kernel::VMD slicePoint);

  void setSlicePoint(Mantid::Kernel::VMD slicePoint);

  void enable();

  void disable();

  void updateXGridlines(QwtValueList xAxisTicks, double xAngle);
  void updateYGridlines(QwtValueList yAxisTicks, double yAngle);
  QwtValueList m_xAxisTicks;
  double m_xAngle;
  QwtValueList m_yAxisTicks;
  double m_yAngle;

private:
  QSize sizeHint() const override;
  QSize size() const;

  int height() const;
  int width() const;

  QPoint transform(QPointF coords) const;
  QPointF invTransform(QPoint pixels) const;

  void drawYLines(QPainter &painter, QPen &gridPen, int widthScreen,
                  int heightScreen, QwtValueList yAxisTicks, double yAngle);

  void drawXLines(QPainter &painter, QPen &gridPen, int widthScreen,
                  int heightScreen, QwtValueList xAxisTicks, double xAngle);

  void setSkewMatrix();

  QPointF skewMatrixApply(double x, double y);

  void paintEvent(QPaintEvent *event) override;

  bool m_enabled;

  QwtPlot *m_plot;
  Mantid::API::IMDWorkspace_sptr *m_ws;

  Mantid::coord_t m_fromHklToOrthogonal[9];
  Mantid::coord_t m_fromOrthogonalToHkl[9];

  size_t m_dimY;
  size_t m_dimX;
  size_t m_missingHKLDim;
  Mantid::Kernel::VMD m_slicePoint;

  /// Width of the line (in coordinates of the plot)
  double m_width;

  double m_angleX;
  double m_angleY;
};

} // namespace SliceViewer
} // namespace Mantid

#endif /* MANTID_SLICEVIEWER_NONORTHOGONALOVERLAY_H_ */
