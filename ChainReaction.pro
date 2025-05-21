QT       += core gui openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets svg

CONFIG += c++17

# Compiler-specific settings
# g++{
#     release{
#     message("GCC detected")
#     QMAKE_CXXFLAGS += -flto
#     QMAKE_LFLAGS += -flto
#     }
# }
# clang{
#     release{
#     message("Clang detected")
#     QMAKE_CXXFLAGS += -flto
#     QMAKE_LFLAGS += -flto
#     }
# }
# msvc{
#     release{
#     message("MSVC detected")
#     QMAKE_CXXFLAGS += /GL
#     QMAKE_LFLAGS += /LTCG
#     }
# }
# else {
#     message("Unknown compiler. No optimizations set.")
# }

# Additional global settings
# release{
#     message("Turning off Debug Output")
#     DEFINES += QT_NO_DEBUG_OUTPUT
# }

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cdotsmanager.cpp \
    cdotswidget.cpp \
    cspatialmanager.cpp \
    cutility.cpp \
    main.cpp \
    mainwindow.cpp \
    tdot.cpp

HEADERS += \
    cdotsmanager.h \
    cdotswidget.h \
    cspatialmanager.h \
    cutility.h \
    mainwindow.h \
    tdot.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_TARGET_SDK_VERSION = 34
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
