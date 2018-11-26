==============================
Work flows algorithm profiling
==============================

.. contents:: Contents
    :local:

Summary
^^^^^^^

Due to the need of investigation of algorithms performance issues, the proper method
is introduced. It consists two to parts: special mantid build and analytical tool.
Available for Linux only.

Mantid build
^^^^^^^^^^^^

To build mantid for profiling run ``cmake`` with the additional option ``-DPROFILE_ALGORITHM_LINUX=ON``.
Built in such a way mantid creates a dump file ``algotimeregister.out`` in the running directory.
This file contains the time stamps for start and finish of executed algorithms with ~nanocecond
precision in a very simple text format.

Analysing tool
^^^^^^^^^^^^^^

The project is available here: https://github.com/nvaytet/mantid-profiler. It provides the nice graphical
tool to interpret the information from dumped file.

