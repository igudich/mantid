#  This file is part of the mantidqt package
#
#  Copyright (C) 2017 mantidproject
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
from __future__ import (absolute_import)

# 3rd party imports
from qtpy.QtWidgets import QDialog, QVBoxLayout

# local imports
from mantidqt.utils.qt import import_qtlib, load_ui


WorkspaceTreeWidget = import_qtlib('_widgetscore', 'mantidqt.widgets.workspacewidget',
                                   'WorkspaceTreeWidgetSimple')

PlotSelectionDialogUI, PlotSelectionDialogUIBase = load_ui(__file__, 'plotselectiondialog.ui')


class PlotSelectionDialog(PlotSelectionDialogUIBase):

    def __init__(self, workspaces,
               parent=None):
        super(PlotSelectionDialog, self).__init__(parent)

        ui = PlotSelectionDialogUI()
        ui.setupUi(self)
        self.ui = ui
