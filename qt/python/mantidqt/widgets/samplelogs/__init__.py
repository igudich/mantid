"""
You can run this widget independently by for example:

"""
from mantidqt.widgets.samplelogs.presenter import SampleLogs
from mantid.simpleapi import Load
from qtpy.QtWidgets import QApplication

ws=Load(r'C:\Users\qbr77747\Downloads\SampleData-ISIS\HRP39187.RAW')

app = QApplication([])
window = SampleLogs(ws)
app.exec_()
