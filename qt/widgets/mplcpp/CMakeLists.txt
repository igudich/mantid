# Rules for matplotlib cpp library
set ( LIB_SRCS
  src/Artist.cpp
  src/Axes.cpp
  src/Colors.cpp
  src/Colormap.cpp
  src/Figure.cpp
  src/FigureCanvasQt.cpp
  src/Line2D.cpp
  src/ScalarMappable.cpp
)

set ( MOC_HEADERS
  inc/MantidQtWidgets/MplCpp/FigureCanvasQt.h
)

set (NOMOC_HEADERS
  inc/MantidQtWidgets/MplCpp/Artist.h
  inc/MantidQtWidgets/MplCpp/Axes.h
  inc/MantidQtWidgets/MplCpp/Colors.h
  inc/MantidQtWidgets/MplCpp/Colormap.h
  inc/MantidQtWidgets/MplCpp/Figure.h
  inc/MantidQtWidgets/MplCpp/Line2D.h
  inc/MantidQtWidgets/MplCpp/ScalarMappable.h
)

find_package ( BoostPython REQUIRED )

# Target
mtd_add_qt_library (TARGET_NAME MantidQtWidgetsMplCpp
  QT_VERSION 5
  SRC ${LIB_SRCS}
  MOC ${MOC_HEADERS}
  NOMOC ${NOMOC_HEADERS}
  DEFS
    IN_MANTIDQT_MPLCPP
  INCLUDE_DIRS
      inc
      ../../../Framework/PythonInterface/core/inc
      ${Boost_INCLUDE_DIRS}
      ${PYTHON_INCLUDE_PATH}
      ${PYTHON_NUMPY_INCLUDE_DIR}
  LINK_LIBS
    ${TCMALLOC_LIBRARIES_LINKTIME}
    ${Boost_LIBRARIES}
    ${PYTHON_LIBRARIES}
    PythonInterfaceCore
  INSTALL_DIR
    ${LIB_DIR}
  OSX_INSTALL_RPATH
    @loader_path/../MacOS
    @loader_path/../Libraries
  LINUX_INSTALL_RPATH
    "\$ORIGIN/../${LIB_DIR}"
)

# Testing
add_subdirectory ( test )
