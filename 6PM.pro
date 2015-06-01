QT += widgets

HEADERS += MVMainWindow.h \
    MVVersion.h \
    MVGlobals.h \
    MVControlManager.h \
    MVController.h \
    MVFrame.h \
    MVPlayer.h \
    MVNote.h \
    MVUtils.h \
    MVOscillator_ui.h \
    MVDefines.h \
    MVEnvelopeDrawer.h \
    MVDial.h \
    MVMeter.h \
    MVEnvelope_ui.h \
    MVAmpEnvelope.h \
    MVFreqEnvelope.h \
    MVAlgoDrawer.h \
    MVSlider.h \
    MVCheckBox.h \
    MVCheckBoxAction.h \
    MVSpinBoxAction.h \
    MVPushButton.h \
    MVBackground.h \
    MVOscillator.h \
    MVLfo.h
SOURCES += main.cpp \
    MVMainWindow.cpp \
    MVController.cpp \
    MVGlobals.cpp \
    MVFrame.cpp \
    MVPlayer.cpp \
    MVControlManager.cpp \
    MVNote.cpp \
    MVEnvelope_ui.cpp \
    MVOscillator_ui.cpp \
    MVEnvelopeDrawer.cpp \
    MVDial.cpp \
    MVMeter.cpp \
    MVAmpEnvelope.cpp \
    MVFreqEnvelope.cpp \
    MVAlgoDrawer.cpp \
    MVSlider.cpp \
    MVCheckBox.cpp \
    MVCheckBoxAction.cpp \
    MVSpinBoxAction.cpp \
    MVPushButton.cpp \
    MVBackground.cpp \
    MVOscillator.cpp \
    MVLfo.cpp \
    MVUtils.cpp
LIBS += -lasound \
    -ljack \
    -lpthread
LIBS += -L/usr/lib:/usr/local/lib
OTHER_FILES += 
RESOURCES += 6pm.qrc
TARGET = 6pm
