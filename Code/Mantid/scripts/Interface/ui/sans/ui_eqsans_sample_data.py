# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui/sans/eqsans_sample_data.ui'
#
# Created: Thu Aug 18 12:24:04 2011
#      by: PyQt4 UI code generator 4.7.2
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_Frame(object):
    def setupUi(self, Frame):
        Frame.setObjectName("Frame")
        Frame.resize(1014, 874)
        Frame.setFrameShape(QtGui.QFrame.NoFrame)
        Frame.setFrameShadow(QtGui.QFrame.Raised)
        self.verticalLayout = QtGui.QVBoxLayout(Frame)
        self.verticalLayout.setMargin(0)
        self.verticalLayout.setObjectName("verticalLayout")
        self.scrollArea = QtGui.QScrollArea(Frame)
        self.scrollArea.setFrameShape(QtGui.QFrame.NoFrame)
        self.scrollArea.setFrameShadow(QtGui.QFrame.Sunken)
        self.scrollArea.setWidgetResizable(True)
        self.scrollArea.setObjectName("scrollArea")
        self.scrollAreaWidgetContents_2 = QtGui.QWidget(self.scrollArea)
        self.scrollAreaWidgetContents_2.setGeometry(QtCore.QRect(0, 0, 1014, 874))
        self.scrollAreaWidgetContents_2.setObjectName("scrollAreaWidgetContents_2")
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.scrollAreaWidgetContents_2)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.horizontalLayout_7 = QtGui.QHBoxLayout()
        self.horizontalLayout_7.setObjectName("horizontalLayout_7")
        self.label_3 = QtGui.QLabel(self.scrollAreaWidgetContents_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_3.sizePolicy().hasHeightForWidth())
        self.label_3.setSizePolicy(sizePolicy)
        self.label_3.setMinimumSize(QtCore.QSize(175, 0))
        self.label_3.setMaximumSize(QtCore.QSize(175, 16777215))
        self.label_3.setObjectName("label_3")
        self.horizontalLayout_7.addWidget(self.label_3)
        spacerItem = QtGui.QSpacerItem(78, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_7.addItem(spacerItem)
        self.data_file_edit = QtGui.QLineEdit(self.scrollAreaWidgetContents_2)
        self.data_file_edit.setMinimumSize(QtCore.QSize(300, 0))
        self.data_file_edit.setObjectName("data_file_edit")
        self.horizontalLayout_7.addWidget(self.data_file_edit)
        self.data_file_browse_button = QtGui.QPushButton(self.scrollAreaWidgetContents_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.data_file_browse_button.sizePolicy().hasHeightForWidth())
        self.data_file_browse_button.setSizePolicy(sizePolicy)
        self.data_file_browse_button.setMinimumSize(QtCore.QSize(0, 0))
        self.data_file_browse_button.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.data_file_browse_button.setObjectName("data_file_browse_button")
        self.horizontalLayout_7.addWidget(self.data_file_browse_button)
        self.data_file_plot_button = QtGui.QPushButton(self.scrollAreaWidgetContents_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.data_file_plot_button.sizePolicy().hasHeightForWidth())
        self.data_file_plot_button.setSizePolicy(sizePolicy)
        self.data_file_plot_button.setMinimumSize(QtCore.QSize(0, 0))
        self.data_file_plot_button.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.data_file_plot_button.setObjectName("data_file_plot_button")
        self.horizontalLayout_7.addWidget(self.data_file_plot_button)
        spacerItem1 = QtGui.QSpacerItem(49, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_7.addItem(spacerItem1)
        self.verticalLayout_2.addLayout(self.horizontalLayout_7)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.label = QtGui.QLabel(self.scrollAreaWidgetContents_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        self.label.setMinimumSize(QtCore.QSize(175, 0))
        self.label.setMaximumSize(QtCore.QSize(175, 16777215))
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        spacerItem2 = QtGui.QSpacerItem(78, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem2)
        self.sample_thickness_edit = QtGui.QLineEdit(self.scrollAreaWidgetContents_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.sample_thickness_edit.sizePolicy().hasHeightForWidth())
        self.sample_thickness_edit.setSizePolicy(sizePolicy)
        self.sample_thickness_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.sample_thickness_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.sample_thickness_edit.setObjectName("sample_thickness_edit")
        self.horizontalLayout.addWidget(self.sample_thickness_edit)
        spacerItem3 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem3)
        self.verticalLayout_2.addLayout(self.horizontalLayout)
        spacerItem4 = QtGui.QSpacerItem(20, 10, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        self.verticalLayout_2.addItem(spacerItem4)
        self.groupBox = QtGui.QGroupBox(self.scrollAreaWidgetContents_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox.sizePolicy().hasHeightForWidth())
        self.groupBox.setSizePolicy(sizePolicy)
        self.groupBox.setObjectName("groupBox")
        self.verticalLayout_3 = QtGui.QVBoxLayout(self.groupBox)
        self.verticalLayout_3.setObjectName("verticalLayout_3")
        self.gridLayout = QtGui.QGridLayout()
        self.gridLayout.setObjectName("gridLayout")
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.sample_label = QtGui.QLabel(self.groupBox)
        self.sample_label.setMinimumSize(QtCore.QSize(243, 0))
        self.sample_label.setMaximumSize(QtCore.QSize(203, 16777215))
        self.sample_label.setObjectName("sample_label")
        self.horizontalLayout_2.addWidget(self.sample_label)
        self.sample_edit = QtGui.QLineEdit(self.groupBox)
        self.sample_edit.setMinimumSize(QtCore.QSize(300, 0))
        self.sample_edit.setObjectName("sample_edit")
        self.horizontalLayout_2.addWidget(self.sample_edit)
        self.sample_button = QtGui.QPushButton(self.groupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.sample_button.sizePolicy().hasHeightForWidth())
        self.sample_button.setSizePolicy(sizePolicy)
        self.sample_button.setObjectName("sample_button")
        self.horizontalLayout_2.addWidget(self.sample_button)
        self.sample_plot_button = QtGui.QPushButton(self.groupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.sample_plot_button.sizePolicy().hasHeightForWidth())
        self.sample_plot_button.setSizePolicy(sizePolicy)
        self.sample_plot_button.setObjectName("sample_plot_button")
        self.horizontalLayout_2.addWidget(self.sample_plot_button)
        spacerItem5 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem5)
        self.gridLayout.addLayout(self.horizontalLayout_2, 5, 0, 1, 1)
        self.horizontalLayout_5 = QtGui.QHBoxLayout()
        self.horizontalLayout_5.setObjectName("horizontalLayout_5")
        self.empty_label = QtGui.QLabel(self.groupBox)
        self.empty_label.setMinimumSize(QtCore.QSize(243, 0))
        self.empty_label.setMaximumSize(QtCore.QSize(203, 16777215))
        self.empty_label.setObjectName("empty_label")
        self.horizontalLayout_5.addWidget(self.empty_label)
        self.empty_edit = QtGui.QLineEdit(self.groupBox)
        self.empty_edit.setMinimumSize(QtCore.QSize(300, 0))
        self.empty_edit.setObjectName("empty_edit")
        self.horizontalLayout_5.addWidget(self.empty_edit)
        self.empty_button = QtGui.QPushButton(self.groupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.empty_button.sizePolicy().hasHeightForWidth())
        self.empty_button.setSizePolicy(sizePolicy)
        self.empty_button.setObjectName("empty_button")
        self.horizontalLayout_5.addWidget(self.empty_button)
        self.empty_plot_button = QtGui.QPushButton(self.groupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.empty_plot_button.sizePolicy().hasHeightForWidth())
        self.empty_plot_button.setSizePolicy(sizePolicy)
        self.empty_plot_button.setObjectName("empty_plot_button")
        self.horizontalLayout_5.addWidget(self.empty_plot_button)
        spacerItem6 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_5.addItem(spacerItem6)
        self.gridLayout.addLayout(self.horizontalLayout_5, 6, 0, 1, 1)
        self.horizontalLayout_6 = QtGui.QHBoxLayout()
        self.horizontalLayout_6.setObjectName("horizontalLayout_6")
        self.beam_radius_label = QtGui.QLabel(self.groupBox)
        self.beam_radius_label.setMinimumSize(QtCore.QSize(243, 0))
        self.beam_radius_label.setObjectName("beam_radius_label")
        self.horizontalLayout_6.addWidget(self.beam_radius_label)
        self.beam_radius_edit = QtGui.QLineEdit(self.groupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.beam_radius_edit.sizePolicy().hasHeightForWidth())
        self.beam_radius_edit.setSizePolicy(sizePolicy)
        self.beam_radius_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.beam_radius_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.beam_radius_edit.setObjectName("beam_radius_edit")
        self.horizontalLayout_6.addWidget(self.beam_radius_edit)
        spacerItem7 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_6.addItem(spacerItem7)
        self.gridLayout.addLayout(self.horizontalLayout_6, 8, 0, 1, 1)
        self.horizontalLayout_8 = QtGui.QHBoxLayout()
        self.horizontalLayout_8.setObjectName("horizontalLayout_8")
        self.fit_together_check = QtGui.QCheckBox(self.groupBox)
        self.fit_together_check.setObjectName("fit_together_check")
        self.horizontalLayout_8.addWidget(self.fit_together_check)
        self.gridLayout.addLayout(self.horizontalLayout_8, 10, 0, 1, 1)
        self.horizontalLayout_3 = QtGui.QHBoxLayout()
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.fix_transmission_layout = QtGui.QHBoxLayout()
        self.fix_transmission_layout.setObjectName("fix_transmission_layout")
        self.calculate_radio = QtGui.QRadioButton(self.groupBox)
        self.calculate_radio.setMinimumSize(QtCore.QSize(180, 0))
        self.calculate_radio.setObjectName("calculate_radio")
        self.fix_transmission_layout.addWidget(self.calculate_radio)
        spacerItem8 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.fix_transmission_layout.addItem(spacerItem8)
        self.fix_trans_radio = QtGui.QRadioButton(self.groupBox)
        self.fix_trans_radio.setObjectName("fix_trans_radio")
        self.fix_transmission_layout.addWidget(self.fix_trans_radio)
        self.transmission_edit = QtGui.QLineEdit(self.groupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.transmission_edit.sizePolicy().hasHeightForWidth())
        self.transmission_edit.setSizePolicy(sizePolicy)
        self.transmission_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.transmission_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.transmission_edit.setObjectName("transmission_edit")
        self.fix_transmission_layout.addWidget(self.transmission_edit)
        self.plus_minus_label = QtGui.QLabel(self.groupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.plus_minus_label.sizePolicy().hasHeightForWidth())
        self.plus_minus_label.setSizePolicy(sizePolicy)
        self.plus_minus_label.setObjectName("plus_minus_label")
        self.fix_transmission_layout.addWidget(self.plus_minus_label)
        self.dtransmission_edit = QtGui.QLineEdit(self.groupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.dtransmission_edit.sizePolicy().hasHeightForWidth())
        self.dtransmission_edit.setSizePolicy(sizePolicy)
        self.dtransmission_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.dtransmission_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.dtransmission_edit.setObjectName("dtransmission_edit")
        self.fix_transmission_layout.addWidget(self.dtransmission_edit)
        spacerItem9 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.fix_transmission_layout.addItem(spacerItem9)
        self.horizontalLayout_3.addLayout(self.fix_transmission_layout)
        self.gridLayout.addLayout(self.horizontalLayout_3, 2, 0, 1, 1)
        self.theta_dep_chk = QtGui.QCheckBox(self.groupBox)
        self.theta_dep_chk.setObjectName("theta_dep_chk")
        self.gridLayout.addWidget(self.theta_dep_chk, 11, 0, 1, 1)
        self.verticalLayout_3.addLayout(self.gridLayout)
        self.verticalLayout_2.addWidget(self.groupBox)
        spacerItem10 = QtGui.QSpacerItem(20, 30, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        self.verticalLayout_2.addItem(spacerItem10)
        self.line = QtGui.QFrame(self.scrollAreaWidgetContents_2)
        self.line.setFrameShape(QtGui.QFrame.HLine)
        self.line.setFrameShadow(QtGui.QFrame.Sunken)
        self.line.setObjectName("line")
        self.verticalLayout_2.addWidget(self.line)
        spacerItem11 = QtGui.QSpacerItem(20, 10, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        self.verticalLayout_2.addItem(spacerItem11)
        self.horizontalLayout_4 = QtGui.QHBoxLayout()
        self.horizontalLayout_4.setObjectName("horizontalLayout_4")
        self.background_chk = QtGui.QCheckBox(self.scrollAreaWidgetContents_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.background_chk.sizePolicy().hasHeightForWidth())
        self.background_chk.setSizePolicy(sizePolicy)
        self.background_chk.setMinimumSize(QtCore.QSize(175, 0))
        self.background_chk.setMaximumSize(QtCore.QSize(175, 16777215))
        self.background_chk.setObjectName("background_chk")
        self.horizontalLayout_4.addWidget(self.background_chk)
        spacerItem12 = QtGui.QSpacerItem(78, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_4.addItem(spacerItem12)
        self.background_edit = QtGui.QLineEdit(self.scrollAreaWidgetContents_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.background_edit.sizePolicy().hasHeightForWidth())
        self.background_edit.setSizePolicy(sizePolicy)
        self.background_edit.setMinimumSize(QtCore.QSize(300, 0))
        self.background_edit.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.background_edit.setObjectName("background_edit")
        self.horizontalLayout_4.addWidget(self.background_edit)
        self.background_browse = QtGui.QPushButton(self.scrollAreaWidgetContents_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.background_browse.sizePolicy().hasHeightForWidth())
        self.background_browse.setSizePolicy(sizePolicy)
        self.background_browse.setMinimumSize(QtCore.QSize(0, 0))
        self.background_browse.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.background_browse.setObjectName("background_browse")
        self.horizontalLayout_4.addWidget(self.background_browse)
        self.background_plot_button = QtGui.QPushButton(self.scrollAreaWidgetContents_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.background_plot_button.sizePolicy().hasHeightForWidth())
        self.background_plot_button.setSizePolicy(sizePolicy)
        self.background_plot_button.setMinimumSize(QtCore.QSize(0, 0))
        self.background_plot_button.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.background_plot_button.setObjectName("background_plot_button")
        self.horizontalLayout_4.addWidget(self.background_plot_button)
        spacerItem13 = QtGui.QSpacerItem(49, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_4.addItem(spacerItem13)
        self.verticalLayout_2.addLayout(self.horizontalLayout_4)
        self.horizontalLayout_13 = QtGui.QHBoxLayout()
        self.horizontalLayout_13.setObjectName("horizontalLayout_13")
        self.bck_thickness_label = QtGui.QLabel(self.scrollAreaWidgetContents_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.bck_thickness_label.sizePolicy().hasHeightForWidth())
        self.bck_thickness_label.setSizePolicy(sizePolicy)
        self.bck_thickness_label.setMinimumSize(QtCore.QSize(178, 0))
        self.bck_thickness_label.setMaximumSize(QtCore.QSize(178, 16777215))
        self.bck_thickness_label.setObjectName("bck_thickness_label")
        self.horizontalLayout_13.addWidget(self.bck_thickness_label)
        spacerItem14 = QtGui.QSpacerItem(78, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_13.addItem(spacerItem14)
        self.bck_thickness_edit = QtGui.QLineEdit(self.scrollAreaWidgetContents_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.bck_thickness_edit.sizePolicy().hasHeightForWidth())
        self.bck_thickness_edit.setSizePolicy(sizePolicy)
        self.bck_thickness_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.bck_thickness_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.bck_thickness_edit.setObjectName("bck_thickness_edit")
        self.horizontalLayout_13.addWidget(self.bck_thickness_edit)
        spacerItem15 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_13.addItem(spacerItem15)
        self.verticalLayout_2.addLayout(self.horizontalLayout_13)
        self.transmission_grpbox = QtGui.QGroupBox(self.scrollAreaWidgetContents_2)
        self.transmission_grpbox.setObjectName("transmission_grpbox")
        self.verticalLayout_4 = QtGui.QVBoxLayout(self.transmission_grpbox)
        self.verticalLayout_4.setObjectName("verticalLayout_4")
        self.bck_fix_transmission_layout = QtGui.QHBoxLayout()
        self.bck_fix_transmission_layout.setObjectName("bck_fix_transmission_layout")
        self.bck_calculate_radio = QtGui.QRadioButton(self.transmission_grpbox)
        self.bck_calculate_radio.setObjectName("bck_calculate_radio")
        self.bck_fix_transmission_layout.addWidget(self.bck_calculate_radio)
        spacerItem16 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.bck_fix_transmission_layout.addItem(spacerItem16)
        self.bck_fix_trans_radio = QtGui.QRadioButton(self.transmission_grpbox)
        self.bck_fix_trans_radio.setObjectName("bck_fix_trans_radio")
        self.bck_fix_transmission_layout.addWidget(self.bck_fix_trans_radio)
        self.bck_transmission_edit = QtGui.QLineEdit(self.transmission_grpbox)
        self.bck_transmission_edit.setEnabled(True)
        self.bck_transmission_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.bck_transmission_edit.setMaximumSize(QtCore.QSize(80, 30))
        self.bck_transmission_edit.setObjectName("bck_transmission_edit")
        self.bck_fix_transmission_layout.addWidget(self.bck_transmission_edit)
        self.bck_plus_minus_label = QtGui.QLabel(self.transmission_grpbox)
        self.bck_plus_minus_label.setMaximumSize(QtCore.QSize(16777215, 30))
        self.bck_plus_minus_label.setObjectName("bck_plus_minus_label")
        self.bck_fix_transmission_layout.addWidget(self.bck_plus_minus_label)
        self.bck_dtransmission_edit = QtGui.QLineEdit(self.transmission_grpbox)
        self.bck_dtransmission_edit.setEnabled(True)
        self.bck_dtransmission_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.bck_dtransmission_edit.setMaximumSize(QtCore.QSize(80, 30))
        self.bck_dtransmission_edit.setObjectName("bck_dtransmission_edit")
        self.bck_fix_transmission_layout.addWidget(self.bck_dtransmission_edit)
        spacerItem17 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.bck_fix_transmission_layout.addItem(spacerItem17)
        self.verticalLayout_4.addLayout(self.bck_fix_transmission_layout)
        self.horizontalLayout_9 = QtGui.QHBoxLayout()
        self.horizontalLayout_9.setObjectName("horizontalLayout_9")
        self.bck_sample_label = QtGui.QLabel(self.transmission_grpbox)
        self.bck_sample_label.setMinimumSize(QtCore.QSize(243, 0))
        self.bck_sample_label.setMaximumSize(QtCore.QSize(203, 16777215))
        self.bck_sample_label.setObjectName("bck_sample_label")
        self.horizontalLayout_9.addWidget(self.bck_sample_label)
        self.bck_sample_edit = QtGui.QLineEdit(self.transmission_grpbox)
        self.bck_sample_edit.setMinimumSize(QtCore.QSize(300, 0))
        self.bck_sample_edit.setObjectName("bck_sample_edit")
        self.horizontalLayout_9.addWidget(self.bck_sample_edit)
        self.bck_sample_button = QtGui.QPushButton(self.transmission_grpbox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.bck_sample_button.sizePolicy().hasHeightForWidth())
        self.bck_sample_button.setSizePolicy(sizePolicy)
        self.bck_sample_button.setObjectName("bck_sample_button")
        self.horizontalLayout_9.addWidget(self.bck_sample_button)
        self.bck_sample_plot_button = QtGui.QPushButton(self.transmission_grpbox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.bck_sample_plot_button.sizePolicy().hasHeightForWidth())
        self.bck_sample_plot_button.setSizePolicy(sizePolicy)
        self.bck_sample_plot_button.setObjectName("bck_sample_plot_button")
        self.horizontalLayout_9.addWidget(self.bck_sample_plot_button)
        spacerItem18 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_9.addItem(spacerItem18)
        self.verticalLayout_4.addLayout(self.horizontalLayout_9)
        self.horizontalLayout_10 = QtGui.QHBoxLayout()
        self.horizontalLayout_10.setObjectName("horizontalLayout_10")
        self.bck_empty_label = QtGui.QLabel(self.transmission_grpbox)
        self.bck_empty_label.setMinimumSize(QtCore.QSize(243, 0))
        self.bck_empty_label.setMaximumSize(QtCore.QSize(203, 16777215))
        self.bck_empty_label.setObjectName("bck_empty_label")
        self.horizontalLayout_10.addWidget(self.bck_empty_label)
        self.bck_empty_edit = QtGui.QLineEdit(self.transmission_grpbox)
        self.bck_empty_edit.setMinimumSize(QtCore.QSize(300, 0))
        self.bck_empty_edit.setObjectName("bck_empty_edit")
        self.horizontalLayout_10.addWidget(self.bck_empty_edit)
        self.bck_empty_button = QtGui.QPushButton(self.transmission_grpbox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.bck_empty_button.sizePolicy().hasHeightForWidth())
        self.bck_empty_button.setSizePolicy(sizePolicy)
        self.bck_empty_button.setObjectName("bck_empty_button")
        self.horizontalLayout_10.addWidget(self.bck_empty_button)
        self.bck_empty_plot_button = QtGui.QPushButton(self.transmission_grpbox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.bck_empty_plot_button.sizePolicy().hasHeightForWidth())
        self.bck_empty_plot_button.setSizePolicy(sizePolicy)
        self.bck_empty_plot_button.setObjectName("bck_empty_plot_button")
        self.horizontalLayout_10.addWidget(self.bck_empty_plot_button)
        spacerItem19 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_10.addItem(spacerItem19)
        self.verticalLayout_4.addLayout(self.horizontalLayout_10)
        self.horizontalLayout_11 = QtGui.QHBoxLayout()
        self.horizontalLayout_11.setObjectName("horizontalLayout_11")
        self.bck_beam_radius_label = QtGui.QLabel(self.transmission_grpbox)
        self.bck_beam_radius_label.setMinimumSize(QtCore.QSize(243, 0))
        self.bck_beam_radius_label.setMaximumSize(QtCore.QSize(203, 16777215))
        self.bck_beam_radius_label.setObjectName("bck_beam_radius_label")
        self.horizontalLayout_11.addWidget(self.bck_beam_radius_label)
        self.bck_beam_radius_edit = QtGui.QLineEdit(self.transmission_grpbox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.bck_beam_radius_edit.sizePolicy().hasHeightForWidth())
        self.bck_beam_radius_edit.setSizePolicy(sizePolicy)
        self.bck_beam_radius_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.bck_beam_radius_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.bck_beam_radius_edit.setObjectName("bck_beam_radius_edit")
        self.horizontalLayout_11.addWidget(self.bck_beam_radius_edit)
        spacerItem20 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_11.addItem(spacerItem20)
        self.verticalLayout_4.addLayout(self.horizontalLayout_11)
        self.horizontalLayout_12 = QtGui.QHBoxLayout()
        self.horizontalLayout_12.setObjectName("horizontalLayout_12")
        self.bck_fit_together_check = QtGui.QCheckBox(self.transmission_grpbox)
        self.bck_fit_together_check.setObjectName("bck_fit_together_check")
        self.horizontalLayout_12.addWidget(self.bck_fit_together_check)
        self.verticalLayout_4.addLayout(self.horizontalLayout_12)
        self.bck_theta_dep_chk = QtGui.QCheckBox(self.transmission_grpbox)
        self.bck_theta_dep_chk.setObjectName("bck_theta_dep_chk")
        self.verticalLayout_4.addWidget(self.bck_theta_dep_chk)
        self.verticalLayout_2.addWidget(self.transmission_grpbox)
        spacerItem21 = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout_2.addItem(spacerItem21)
        self.scrollArea.setWidget(self.scrollAreaWidgetContents_2)
        self.verticalLayout.addWidget(self.scrollArea)

        self.retranslateUi(Frame)
        QtCore.QMetaObject.connectSlotsByName(Frame)

    def retranslateUi(self, Frame):
        Frame.setWindowTitle(QtGui.QApplication.translate("Frame", "Frame", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Frame", "Scattering data file", None, QtGui.QApplication.UnicodeUTF8))
        self.data_file_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter a valid data file path.", None, QtGui.QApplication.UnicodeUTF8))
        self.data_file_browse_button.setToolTip(QtGui.QApplication.translate("Frame", "Click to browse.", None, QtGui.QApplication.UnicodeUTF8))
        self.data_file_browse_button.setText(QtGui.QApplication.translate("Frame", "Browse...", None, QtGui.QApplication.UnicodeUTF8))
        self.data_file_plot_button.setToolTip(QtGui.QApplication.translate("Frame", "Click to plot 2D data.", None, QtGui.QApplication.UnicodeUTF8))
        self.data_file_plot_button.setText(QtGui.QApplication.translate("Frame", "Plot", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Frame", "Sample thickness [cm]", None, QtGui.QApplication.UnicodeUTF8))
        self.sample_thickness_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter sample thickness [cm]", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox.setTitle(QtGui.QApplication.translate("Frame", "Transmission", None, QtGui.QApplication.UnicodeUTF8))
        self.sample_label.setText(QtGui.QApplication.translate("Frame", "Sample direct beam data file", None, QtGui.QApplication.UnicodeUTF8))
        self.sample_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter a valid data file path.", None, QtGui.QApplication.UnicodeUTF8))
        self.sample_button.setText(QtGui.QApplication.translate("Frame", "Browse...", None, QtGui.QApplication.UnicodeUTF8))
        self.sample_plot_button.setToolTip(QtGui.QApplication.translate("Frame", "Click to plot 2D data.", None, QtGui.QApplication.UnicodeUTF8))
        self.sample_plot_button.setText(QtGui.QApplication.translate("Frame", "Plot", None, QtGui.QApplication.UnicodeUTF8))
        self.empty_label.setText(QtGui.QApplication.translate("Frame", "Empty direct beam data file", None, QtGui.QApplication.UnicodeUTF8))
        self.empty_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter a valid data file path.", None, QtGui.QApplication.UnicodeUTF8))
        self.empty_button.setText(QtGui.QApplication.translate("Frame", "Browse...", None, QtGui.QApplication.UnicodeUTF8))
        self.empty_plot_button.setToolTip(QtGui.QApplication.translate("Frame", "Click to plot 2D data.", None, QtGui.QApplication.UnicodeUTF8))
        self.empty_plot_button.setText(QtGui.QApplication.translate("Frame", "Plot", None, QtGui.QApplication.UnicodeUTF8))
        self.beam_radius_label.setText(QtGui.QApplication.translate("Frame", "Beam radius (pixels)", None, QtGui.QApplication.UnicodeUTF8))
        self.beam_radius_edit.setToolTip(QtGui.QApplication.translate("Frame", "Radius of the beam in pixels.", None, QtGui.QApplication.UnicodeUTF8))
        self.fit_together_check.setToolTip(QtGui.QApplication.translate("Frame", "Select to fit the two frames together when in frame-skipping mode.", None, QtGui.QApplication.UnicodeUTF8))
        self.fit_together_check.setText(QtGui.QApplication.translate("Frame", "Fit frames together", None, QtGui.QApplication.UnicodeUTF8))
        self.calculate_radio.setToolTip(QtGui.QApplication.translate("Frame", "Select to let the reduction software calculate the transmission.", None, QtGui.QApplication.UnicodeUTF8))
        self.calculate_radio.setText(QtGui.QApplication.translate("Frame", "Calculate transmission", None, QtGui.QApplication.UnicodeUTF8))
        self.fix_trans_radio.setText(QtGui.QApplication.translate("Frame", "Fix transmission", None, QtGui.QApplication.UnicodeUTF8))
        self.transmission_edit.setToolTip(QtGui.QApplication.translate("Frame", "Sample transmission in %.", None, QtGui.QApplication.UnicodeUTF8))
        self.plus_minus_label.setText(QtGui.QApplication.translate("Frame", "+/-", None, QtGui.QApplication.UnicodeUTF8))
        self.dtransmission_edit.setToolTip(QtGui.QApplication.translate("Frame", "Uncertainty on the sample transmission.", None, QtGui.QApplication.UnicodeUTF8))
        self.theta_dep_chk.setToolTip(QtGui.QApplication.translate("Frame", "Select to apply a theta-dependent transmission correction.", None, QtGui.QApplication.UnicodeUTF8))
        self.theta_dep_chk.setText(QtGui.QApplication.translate("Frame", "Theta-dependent correction", None, QtGui.QApplication.UnicodeUTF8))
        self.background_chk.setToolTip(QtGui.QApplication.translate("Frame", "Select to apply a background subtraction.", None, QtGui.QApplication.UnicodeUTF8))
        self.background_chk.setText(QtGui.QApplication.translate("Frame", "Background data file", None, QtGui.QApplication.UnicodeUTF8))
        self.background_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter a valid data file path.", None, QtGui.QApplication.UnicodeUTF8))
        self.background_browse.setToolTip(QtGui.QApplication.translate("Frame", "Click to browse.", None, QtGui.QApplication.UnicodeUTF8))
        self.background_browse.setText(QtGui.QApplication.translate("Frame", "Browse...", None, QtGui.QApplication.UnicodeUTF8))
        self.background_plot_button.setToolTip(QtGui.QApplication.translate("Frame", "Click to plot 2D data.", None, QtGui.QApplication.UnicodeUTF8))
        self.background_plot_button.setText(QtGui.QApplication.translate("Frame", "Plot", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_thickness_label.setText(QtGui.QApplication.translate("Frame", "Sample thickness [cm]", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_thickness_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter background sample thickness [cm]", None, QtGui.QApplication.UnicodeUTF8))
        self.transmission_grpbox.setTitle(QtGui.QApplication.translate("Frame", "Transmission", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_calculate_radio.setToolTip(QtGui.QApplication.translate("Frame", "Select to let the reduction software calculate the background transmission.", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_calculate_radio.setText(QtGui.QApplication.translate("Frame", "Calculate  transmission", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_fix_trans_radio.setText(QtGui.QApplication.translate("Frame", "Fix transmission", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_transmission_edit.setToolTip(QtGui.QApplication.translate("Frame", "Transmission value for the background in %.", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_plus_minus_label.setText(QtGui.QApplication.translate("Frame", "+/-", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_dtransmission_edit.setToolTip(QtGui.QApplication.translate("Frame", "Uncertainty on the background transmission.", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_sample_label.setText(QtGui.QApplication.translate("Frame", "Sample direct beam data file", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_sample_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter a valid data file path.", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_sample_button.setText(QtGui.QApplication.translate("Frame", "Browse...", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_sample_plot_button.setToolTip(QtGui.QApplication.translate("Frame", "Click to plot 2D data.", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_sample_plot_button.setText(QtGui.QApplication.translate("Frame", "Plot", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_empty_label.setText(QtGui.QApplication.translate("Frame", "Empty direct beam data file", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_empty_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter a valid data file path.", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_empty_button.setText(QtGui.QApplication.translate("Frame", "Browse...", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_empty_plot_button.setToolTip(QtGui.QApplication.translate("Frame", "Click to plot 2D data.", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_empty_plot_button.setText(QtGui.QApplication.translate("Frame", "Plot", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_beam_radius_label.setText(QtGui.QApplication.translate("Frame", "Beam radius (pixels)", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_beam_radius_edit.setToolTip(QtGui.QApplication.translate("Frame", "Radius of the beam in pixels.", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_fit_together_check.setToolTip(QtGui.QApplication.translate("Frame", "Select to fit the two frames together when in frame-skipping mode.", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_fit_together_check.setText(QtGui.QApplication.translate("Frame", "Fit frames together", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_theta_dep_chk.setToolTip(QtGui.QApplication.translate("Frame", "Select to apply a theta-dependent transmission correction.", None, QtGui.QApplication.UnicodeUTF8))
        self.bck_theta_dep_chk.setText(QtGui.QApplication.translate("Frame", "Theta-dependent correction", None, QtGui.QApplication.UnicodeUTF8))

