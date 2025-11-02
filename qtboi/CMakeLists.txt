find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(QTBOI_MOC_HDRS
    QtBoiMainWindow.h
    QtBoiEmuThread.h
    QtBoiDisassemblyWindow.h
    QtBoiStatusWindow.h
)

set(QTBOI_SOURCES
    qtboi.cc
    QtBoiMainWindow.cc
    QtBoiEmuThread.cc
    QtBoiDisassemblyWindow.cc
    QtBoiStatusWindow.cc
)

qt_add_executable(qtboi
    ${QTBOI_SOURCES}
    ${QTBOI_MOC_HDRS}
)

target_link_libraries(qtboi PRIVATE Qt6::Widgets wenboicore)

