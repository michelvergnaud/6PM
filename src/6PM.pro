QT += widgets

HEADERS += UI/MVMainWindow.h \
    MVVersion.h \
    MVGlobals.h \
    MVUtils.h \
    MVDefines.h \
    MVPresetManager.h \
    MVPreset.h \
    MVCommandProcessor.h \
    Synth/MVControlNames.h \
    Synth/MVControlManager.h \
    Synth/MVPlayer.h \
    Synth/MVNote.h \
    Synth/MVAmpEnvelope.h \
    Synth/MVFreqEnvelope.h \
    Synth/MVOscillator.h \
    Synth/MVLfo.h \
    Synth/MVFilter.h \
    Synth/MVController.h \
    UI/MVMeter.h \
    UI/MVEnvelopeDrawer.h \
    UI/MVDial.h \
    UI/MVAlgoDrawer.h \
    UI/MVSlider.h \
    UI/MVCheckBox.h \
    UI/MVCheckBoxAction.h \
    UI/MVSpinBoxAction.h \
    UI/MVPushButton.h \
    UI/MVBackground.h \
    UI/MVFilterEnvDialog.h \
    UI/MVAmpEnvelope_UI.h \
    UI/MVFreqEnvelope_UI.h \
    UI/MVEnvelope_UI.h \
    UI/MVOscillator_UI.h \
    UI/MVAmpEnvelopeDrawer.h \
    UI/MVFreqEnvelopeDrawer.h \
    UI/MVControllerWidget.h \
    UI/MVDropTitle.h \
    UI/MVSavePresetsDialog.h \
    UI/MVAlgoEditor.h \
    UI/MVMainFrame.h \
    UI/MVControlUIManager.h
SOURCES += main.cpp \
    MVGlobals.cpp \
    MVUtils.cpp \
    MVPresetManager.cpp \
    MVPreset.cpp \
    MVCommandProcessor.cpp \
    Synth/MVPlayer.cpp \
    Synth/MVControlManager.cpp \
    Synth/MVNote.cpp \
    Synth/MVAmpEnvelope.cpp \
    Synth/MVFreqEnvelope.cpp \
    Synth/MVOscillator.cpp \
    Synth/MVLfo.cpp \
    Synth/MVFilter.cpp \
    Synth/MVController.cpp \
    UI/MVMeter.cpp \
    UI/MVMainWindow.cpp \
    UI/MVEnvelopeDrawer.cpp \
    UI/MVDial.cpp \
    UI/MVAlgoDrawer.cpp \
    UI/MVSlider.cpp \
    UI/MVCheckBox.cpp \
    UI/MVCheckBoxAction.cpp \
    UI/MVSpinBoxAction.cpp \
    UI/MVPushButton.cpp \
    UI/MVBackground.cpp \
    UI/MVFilterEnvDialog.cpp \
    UI/MVAmpEnvelope_UI.cpp \
    UI/MVFreqEnvelope_UI.cpp \
    UI/MVOscillator_UI.cpp \
    UI/MVEnvelope_UI.cpp \
    UI/MVAmpEnvelopeDrawer.cpp \
    UI/MVFreqEnvelopeDrawer.cpp \
    UI/MVControllerWidget.cpp \
    UI/MVDropTitle.cpp \
    UI/MVSavePresetsDialog.cpp \
    UI/MVAlgoEditor.cpp \
    UI/MVMainFrame.cpp \
    UI/MVControlUIManager.cpp
LIBS += -lasound \
    -ljack \
    -lpthread
LIBS += -L/usr/lib:/usr/local/lib
RESOURCES += ./6pm.qrc
TARGET = 6pm
