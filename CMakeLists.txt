cmake_minimum_required(VERSION 3.16)

project(ChainReaction VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find all required Qt components in one go, as often shown in tutorials.
# I've included Sensors and Xml as per the tutorial, if your app uses them.
# If not, you can remove 'Sensors' and 'Xml'.
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui OpenGLWidgets Svg)

qt_standard_project_setup()

# Executable and source files.
qt_add_executable(ChainReaction WIN32 MACOSX_BUNDLE
    cdotsmanager.cpp cdotsmanager.h
    cdotswidget.cpp cdotswidget.h
    cspatialmanager.cpp cspatialmanager.h
    cutility.cpp cutility.h
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    tdot.cpp tdot.h
    android/AndroidManifest.xml android/build.gradle android/res/values/libs.xml android/gradle/wrapper/gradle-wrapper.jar android/gradle/wrapper/gradle-wrapper.properties android/gradle.properties android/gradlew android/gradlew.bat
    resources.qrc
    MANUAL_FINALIZATION
    android/AndroidManifest.xml android/build.gradle android/res/values/libs.xml android/res/xml/qtprovider_paths.xml android/gradle/wrapper/gradle-wrapper.jar android/gradle/wrapper/gradle-wrapper.properties android/gradle.properties android/gradlew android/gradlew.bat
)

# Link to all necessary Qt modules.
target_link_libraries(ChainReaction PRIVATE
    Qt::Core
    Qt::Gui
    Qt::OpenGLWidgets
    Qt6::Widgets      # Necessary for .ui files
    Qt::Svg
)

# --- Android-specific Configuration (following the tutorial) ---
if(ANDROID)
    # Specify the Android package source directory using a target property.
    # This is the method the tutorial uses for the manifest.
    set_property(TARGET ChainReaction APPEND PROPERTY
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")

    # QT_ANDROID_MIN_SDK_VERSION:
    # Set to recommended minimum for Qt 6.5.3. (26 is actually but trying 24)
    set_property(TARGET ChainReaction PROPERTY QT_ANDROID_MIN_SDK_VERSION 24)

    # QT_ANDROID_TARGET_SDK_VERSION:
    # Set to current recommended target.
    set_property(TARGET ChainReaction PROPERTY QT_ANDROID_TARGET_SDK_VERSION 34)

    # Manually finalize the executable for Android deployment.
    # This pairs with MANUAL_FINALIZATION in qt_add_executable.
    qt_finalize_executable(ChainReaction) # <--- Added from tutorial
endif()
# --- End Android-specific Configuration ---


# Install only on non-Android platforms
if(NOT ANDROID)
    install(TARGETS ChainReaction
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION .
    )
endif()

# Deployment script
qt_generate_deploy_app_script(
    TARGET ChainReaction
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})
