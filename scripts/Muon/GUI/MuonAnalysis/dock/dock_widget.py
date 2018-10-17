# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +
from __future__ import (absolute_import, division, print_function)

from PyQt4 import QtGui

from Muon.GUI.Common.context_example.context_example_widget import ContextExampleWidget
from Muon.GUI.Common.dummy.dummy_widget import DummyWidget
from Muon.GUI.Common.dummy_label.dummy_label_widget import DummyLabelWidget
from Muon.GUI.Common.dock.dock_view import DockView

from Muon.GUI.Common.muon_context.muon_context import *


class DockWidget(QtGui.QWidget):

    """
    This is a special case of the widget class structure.
    Normally we would only store the presenter and would
    get the view via the presenter. However, the docks
    have no logic and therefore do not have a presenter.
    So this class simply wraps the dock (view) and
    populates it
    """

    def __init__(self, context, parent=None):
        super(DockWidget, self).__init__(parent)

        self.dockWidget = QtGui.QWidget()

        self.dock_view = DockView(self)

        self.context_example = ContextExampleWidget(context, parent=self)
        self.dock_view.addDock(self.context_example.widget, "Example context")

        self.btn = DummyWidget("moo", self)
        self.dock_view.addDock(self.btn.widget, "first")
        self.btn.setButtonConnection(self.handleButton)

        self.label = DummyLabelWidget(context, Tab2Text, self)
        self.dock_view.addDock(self.label.widget, "second")

        self.btn2 = DummyWidget("waaa", self)
        self.dock_view.addDock(self.btn2.widget, "third")
        self.btn2.setButtonConnection(self.handleButton)

        self.dock_view.makeTabs()
        self.dock_view.keepDocksOpen()

        QHbox = QtGui.QHBoxLayout()
        QHbox.addWidget(self.dock_view)

        self.dockWidget.setLayout(QHbox)

    # set signals and slots
    def setUpdateContext(self, slot):
        self.context_example.setUpdateContext(slot)
        # the buttons change the label value
        # so we want to update context
        self.btn.setButtonConnection(slot)
        self.btn2.setButtonConnection(slot)

    def loadFromProject(self, project):
        self.label.updateLabel(project)

    def handleButton(self, message):
        self.label.updateLabel(message)

    # interaction with context
    def updateContext(self):
        self.label.updateContext()
        self.context_example.updateContext()

    def loadFromContext(self, context):
        self.label.loadFromContext(context)
        self.context_example.loadFromContext(context)

    # needed for docking
    @property
    def widget(self):
        return self.dockWidget

    def closeEvent(self, event):
        self.dock_view.closeEvent(event)
