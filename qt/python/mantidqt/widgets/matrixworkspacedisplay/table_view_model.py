# coding=utf-8
# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +
#  This file is part of the mantid workbench.
#
#
from __future__ import (absolute_import, division, print_function)

from qtpy import QtGui
from qtpy.QtCore import QVariant, Qt, QAbstractTableModel


def enum(**enums):
    return type('Enum', (), enums)


MatrixWorkspaceTableViewModelType = enum(x='x', y='y', e='e')


class MatrixWorkspaceTableViewModel(QAbstractTableModel):
    def __init__(self, ws, model_type):
        """
        :param ws:
        :param model_type: MatrixWorkspaceTableViewModelType
        :type model_type: MatrixWorkspaceTableViewModelType
        """
        assert model_type in [MatrixWorkspaceTableViewModelType.x, MatrixWorkspaceTableViewModelType.y,
                              MatrixWorkspaceTableViewModelType.e], "The Model type must be either X, Y or E."

        super(MatrixWorkspaceTableViewModel, self).__init__()

        self.ws = ws
        self.ws_spectrum_info = self.ws.spectrumInfo()
        self.row_count = self.ws.getNumberHistograms()
        self.column_count = self.ws.blocksize()

        self.masked_rows_cache = []
        self.monitor_rows_cache = []
        self.masked_bins_cache = {}

        self.masked_color = QtGui.QColor(240, 240, 240)

        self.monitor_color = QtGui.QColor(255, 253, 209)
        # TODO implement and account for in data
        # self.start_row = start_row

        self.type = model_type.lower()
        if self.type == MatrixWorkspaceTableViewModelType.x:
            self.relevant_data = self.ws.readX
        elif self.type == MatrixWorkspaceTableViewModelType.y:
            self.relevant_data = self.ws.readY
        elif self.type == MatrixWorkspaceTableViewModelType.e:
            self.relevant_data = self.ws.readE
        else:
            raise ValueError("Unknown model type {0}".format(self.type))

    def _makeVerticalHeader(self, section, role):
        axis_index = 1
        # check that the vertical axis actually exists in the workspace
        if self.ws.axes() > axis_index:
            if role == Qt.DisplayRole:
                return "{0}{1}{2}".format(section, " ", self.ws.getAxis(axis_index).label(section))
            else:
                spectrum_number = self.ws.getSpectrum(section).getSpectrumNo()
                return "index {0}\nspectra no {1}".format(section, spectrum_number)
        else:
            raise NotImplementedError("What do we do here? Handle if the vertical axis does NOT exist")

    def _makeHorizontalHeader(self, section, role):
        # X values get simpler labels
        if self.type == MatrixWorkspaceTableViewModelType.x:
            if role == Qt.DisplayRole:
                # for display, just the bin number
                return "{0}".format(section)
            else:
                # for tooltip index <bin number>
                return "index {0}".format(section)

        # for the Y and E values, create a label with the units
        axis_index = 0
        axis_label_separator = "\n"
        x_vec = self.ws.readX(0)
        if self.ws.isHistogramData():
            bin_centre_value = (x_vec[section] + x_vec[section + 1]) / 2.0
        else:
            bin_centre_value = x_vec[section]

        if role == Qt.DisplayRole:
            # format for the label display
            return u"{0}{1}{2:0.1f}{3}".format(section, axis_label_separator, bin_centre_value,
                                               self.ws.getAxis(axis_index).getUnit().symbol().utf8())
        else:
            # format for the tooltip
            unit = self.ws.getAxis(axis_index).getUnit()
            return u"index {0}{1}{2}{3:0.1f}{4} (bin centre)".format(section, axis_label_separator,
                                                                     unit.symbol().ascii(), bin_centre_value,
                                                                     unit.symbol().utf8())

    def headerData(self, section, orientation, role=None):
        if not (role == Qt.DisplayRole or role == Qt.ToolTipRole):
            return QVariant()

        if orientation == Qt.Vertical:
            return self._makeVerticalHeader(section, role)
        else:
            return self._makeHorizontalHeader(section, role)

    def rowCount(self, parent=None, *args, **kwargs):
        return self.row_count

    def columnCount(self, parent=None, *args, **kwargs):
        return self.column_count

    def data(self, index, role=None):
        row = index.row()
        if role == Qt.DisplayRole:
            # DisplayRole determines the text of each cell
            return str(self.relevant_data(row)[index.column()])
        elif role == Qt.BackgroundRole:
            # BackgroundRole determines the background of each cell

            # Checks if the row is MASKED, if so makes it the specified color for masked
            # The check for masked rows should be first as a monitor row can be masked as well - and we want it to be
            # colored as a masked row, rather than as a monitor row.
            # First do the check in the cache, and only if not present go through SpectrumInfo and cache it. This logic
            # is repeated in the other checks below
            if row in self.masked_rows_cache:
                return self.masked_color
            elif self.ws_spectrum_info.hasDetectors(row) and self.ws_spectrum_info.isMasked(row):
                self.masked_rows_cache.append(row)
                return self.masked_color

            # Checks if the row is a MONITOR, if so makes it the specified color for monitors
            elif row in self.monitor_rows_cache:
                return self.monitor_color
            elif self.ws_spectrum_info.hasDetectors(row) and self.ws_spectrum_info.isMonitor(row):
                self.monitor_rows_cache.append(row)
                return self.monitor_color

            # Checks if the BIN is MASKED, if so makes it the specified color for masked
            elif row in self.masked_bins_cache:
                # retrieve the masked bins IDs from the cache
                if index.column() in self.masked_bins_cache[row]:
                    return self.masked_color
            elif self.ws.hasMaskedBins(row):
                masked_bins = self.ws.maskedBinsIndices(row)
                if index.column() in masked_bins:
                    self.masked_bins_cache[row] = masked_bins
                    return self.masked_color

        elif role == Qt.ToolTipRole:
            # print("ToolTip Role not implemented but avoiding NotImplementedException.")
            # TODO Show tooltip for monitor spectrum, masked spectrum & masked monitor spectrum zzz fucking FIZZ BUZZ over here
            pass
        else:
            return QVariant()
