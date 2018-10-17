# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +


class pair(object):

    def __init__(self, name="", F_group="", B_group="", alpha=1.0):
        self._name = name
        self._F_group = F_group
        self._B_group = B_group
        self._alpha = alpha

    def setName(self, name):
        self._name = name

    def setFGroup(self, group):
        self._F_group = group

    def setBGroup(self, group):
        self._B_group = group

    def setAlpha(self, alpha):
        self._alpha = alpha

    @property
    def name(self):
        return self._name

    @property
    def FGroup(self):
        return self._F_group

    @property
    def BGroup(self):
        return self._B_group

    @property
    def alpha(self):
        return self._alpha

    def isValid(self):
        if self._name != "" and self._F_group != "" and self._B_group != "":
            return True

        return False
