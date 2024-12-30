QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Use Precompiled headers (PCH)
CONFIG += precompile_header


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
QMAKE_CXXFLAGS += /bigobj
QMAKE_CXXFLAGS += /MP

SOURCES += \
    actorwidget.cpp \
    cgalbool.cpp \
    handleinteractor.cpp \
    localadjustmentinterstyle.cpp \
    main.cpp \
    mytitlebar.cpp \
    pickpointselectionstyle.cpp \
    polygonselectinteractorstyle.cpp \
    silhouetterenderer.cpp \
    smiledesigenwidget.cpp \
    smilelinestyle.cpp \
    teethpreview.cpp \
    zoomonlyinteractorstyle.cpp

HEADERS += \
    actorwidget.h \
    cgalbool.h \
    handleinteractor.h \
    localadjustmentinterstyle.h \
    mytitlebar.h \
    pickpointselectionstyle.h \
    polygonselectinteractorstyle.h \
    silhouetterenderer.h \
    smiledesigenwidget.h \
    smilelinestyle.h \
    teethpreview.h \
    zoomonlyinteractorstyle.h


# INCLUDEPATH += D:/cgal/CGAL-5.6.1/include
# INCLUDEPATH += D:/cgal/CGAL-5.6.1/auxiliary/gmp/include
# INCLUDEPATH += D:/boost_1_82_0

INCLUDEPATH += D:/LIBS/alignLib/include
LIBS += D:/LIBS/alignLib/lib/*.lib

INCLUDEPATH +=  D:/Libs/OpenGL/glm-ubuntu \
                D:/Libs/OpenGL/glm-ubuntu/glm

#pcl
INCLUDEPATH += D:/Libs/PCL-1.12.1/include/pcl-1.12
INCLUDEPATH += D:/Libs/PCL-1.12.1/3rdParty/FLANN/include
LIBS += D:/Libs/PCL-1.12.1/lib/pcl*.lib

#cgal
INCLUDEPATH += D:/cgal/CGAL-5.6.1/include
INCLUDEPATH += D:/cgal/CGAL-5.6.1/auxiliary/gmp/include


LIBS += -LD:/cgal/CGAL-5.6.1/auxiliary/gmp/lib/ -llibgmp-10 \
            -LD:/cgal/CGAL-5.6.1/auxiliary/gmp/lib/ -llibmpfr-4

# #boost
INCLUDEPATH += D:/Libs/PCL-1.12.1/3rdParty/Boost/include/boost-1_78
LIBS += D:/Libs/PCL-1.12.1/3rdParty/Boost/lib/*vc142-mt-x64-1_78.lib
#LIBS += D:/Libs/PCL-1.12.1/3rdParty/Boost/lib/*vc142-mt-gd-x64-1_78.lib



INCLUDEPATH += D:/eigen-3.4.0/eigen-3.4.0


# vtk9.1
INCLUDEPATH +=D:/vtk/VTK-9.1.0/include/vtk-9.1
LIBS += D:/vtk/VTK-9.1.0/lib/vtk*.lib

QMAKE_PROJECT_DEPTH = 0


# LIBS += -LD:/cgal/CGAL-5.6.1/auxiliary/gmp/lib/ -llibgmp-10 \
#         -LD:/cgal/CGAL-5.6.1/auxiliary/gmp/lib/ -llibmpfr-4

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS +=

#dlib
#dlib
# INCLUDEPATH += D:/LIBS/dlib-19.24/install/include
# LIBS += D:/LIBS/dlib-19.24/install/lib/dlib19.24.0_release_64bit_msvc1940.lib


INCLUDEPATH += D:/Libs/dlib/dlib-19.24/include
LIBS += D:/Libs/dlib/dlib-19.24/lib/*.lib
# OpenCV
INCLUDEPATH += D:/LIBS/opencv/new_sources/install/include
# LIBS += -LD:/LIBS/opencv/new_sources/install/lib \
#         opencv_world460d.lib
#设置Release
LIBS += D:/LIBS/opencv/new_sources/install/lib/opencv_world460.lib
#LIBS += D:/LIBS/opencv/new_sources/install/lib/opencv_world460d.lib


TRANSLATIONS = test_zh_CN.ts

