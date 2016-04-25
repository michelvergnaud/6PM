/*
    6PM
     Copyright (c) 2015-2016, Michel Vergnaud

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MVFRAME_H
#define MVFRAME_H

#include "MVDefines.h"

#include <QSplitter>
#include <QList>
#include <QFileInfo>

class MVControllerWidget;
class MVControlmanager;
class MVAlgoDrawer;
class MVAlgoEditorDialog;
class MVOscillator_UI;
class MVEnvelope_UI;
class MVFilterEnvDialog;
class MVEnvelopeDrawer;
class MVMeter;
class MVBackground;
class MVSlider;
class MVDial;
class MVPushButton;
class MVCheckBox;
class QComboBox;
class QLabel;
class QPushButton;
class QSpinBox;
class QRadioButton;


class MVMainFrame : public QSplitter
{
    Q_OBJECT
public:
    MVMainFrame(QWidget *parent = 0);
    ~MVMainFrame();

    bool areAssignNumsVisible();
    void updateUIs();
    void updateAmpEnvelope(int);
    void updateFreqEnvelope(int);
    void loadPreset(int);
    void addPreset(QString);
    void bankSelect(int);

    void setTimeScales();
    void setPresetModified(bool);
    void cancelEditAlgo();
    void updateAlgos();
    void updateBanksAndPresets();
    void saveSettings();

protected :
    void keyReleaseEvent(QKeyEvent *);

private:

    MVBackground * midiMapButtonsWidget;
    MVBackground * algoButtonsWidget;
    MVBackground * bankButtonsWidget;

    QComboBox * cbAlgo;
    MVAlgoDrawer * algoDrawer;
    MVAlgoEditorDialog * algoEditorDialog;
    QRadioButton * radioPreDef;
    QRadioButton * radioUser;
    MVPushButton * butEditAlgo;
    int saveUserAlgoIndex;

    QComboBox * cbPreset;
    QComboBox * cbBank;
    MVPushButton * butNewBank;
    MVPushButton * butRenameBank;

    QComboBox * cbMidiMap;
    void updateCbMidiMap();
    void updateCbBank();
    QString currentBank;

    MVPushButton * butSavePreset;
    MVPushButton * butReloadPreset;
    MVPushButton * butSavePresetAs;
    MVPushButton * butRenamePreset;
    MVPushButton * butDeletePreset;

    MVPushButton * butSaveMidiMap;
    MVPushButton * butSaveMidiMapAs;
    MVPushButton * butRenameMidiMap;
    MVPushButton * butDeleteMidiMap;

    MVPushButton * editSender;
    QString oldFileName;
    QString oldName;
    QString newName;

    MVDial * dialDiapason;
    MVPushButton * butTest;

    QPushButton * butPanic;

    QComboBox * comboFilterType;
    MVCheckBox * cbFilterUsed;
    MVDial * dialFilterF;
    MVDial * dialFilterQ;
    QRadioButton * radAbs;
    QRadioButton * radNote;
    MVPushButton * butShowFilterEnvDialog;

    MVOscillator_UI ** oscUI;
    MVEnvelope_UI ** envAmpUI;
    MVEnvelope_UI ** envFreqUI;
    MVFilterEnvDialog * filterEnvDialog;
    MVMeter * meterL;
    MVMeter * meterR;
    MVSlider * volSlider;
    MVBackground * oscBg[NB_OSC];

    void updateCbAlgo();
    void connectWidgets();
    void disconnectWidgets();

private slots:
    void onButtSaveMidiMap();
    void onButtSaveMidiMapAs();
    void onButtRenameMidiMap();
    void onButtDeleteMidiMap();
    void midiMapReturnPressed();
    void editingMidiMapFinished();

    void onButtSavePreset();
    void onButtSavePresetAs();
    void onButtReloadPreset();
    void onButtRenamePreset();
    void onButtDeletePreset();
    void presetReturnPressed();
    void editingPresetFinished();

    void onButtNewBank();
    void onButtRenameBank();
    void bankReturnPressed();
    void editingBankFinished();

    void midiMapChanged(QString);
    void presetChanged(QString);
    void bankChanged(int);
    void bankChanged(QString);
    void algoChanged(int);
    void editAlgo();
    void algoTypeChanged();
    void mainVolChanged(int);
    void updateCbPresets();
    void onPlayTest();
    void onStopTest();
    void onButtPanic();
    void diapasonChanged(int);
    void cbFilterToggled(bool);
    void filterTypeChanged(int);
    void filterFChanged(int);
    void filterQChanged(int);
    void setAbsF(bool);
    void showFilterEnvDialog();
};

#endif // MVFRAME_H
