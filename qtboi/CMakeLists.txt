FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )

SET(QTBOI_MOC_HDRS
  QtBoiMainWindow.h
  QtBoiEmuThread.h
  QtBoiDisassemblyWindow.h
  QtBoiStatusWindow.h
)

SET(QTBOI_SOURCES
  qtboi.cc
  QtBoiMainWindow.cc
  QtBoiEmuThread.cc
  QtBoiDisassemblyWindow.cc
  QtBoiStatusWindow.cc
)

QT4_WRAP_CPP(QTBOI_MOC_SRCS ${QTBOI_MOC_HDRS})
ADD_EXECUTABLE(qtboi ${QTBOI_SOURCES} ${QTBOI_MOC_SRCS})

TARGET_LINK_LIBRARIES(qtboi ${QT_LIBRARIES} wenboicore)

