/*
    6PM
     Copyright (c) 2015, Michel Vergnaud

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

class MVController;
class MVControlmanager;
class MVAlgoDrawer;
class MVOscillator_UI;
class MVEnvelope_UI;
class MVEnvelopeDrawer;
class MVMeter;
class MVBackground;
class MVSlider;
class MVDial;
class MVPushButton;
class QComboBox;
class QLabel;
class QCheckBox;
class QPushButton;
class QSpinBox;
class QRadioButton;

class MVFrame : public QSplitter
{
    Q_OBJECT
public:
    MVFrame(QWidget *parent = 0);
    ~MVFrame();

    bool areAssignNumsVisible();
    void updateUIs();
    void updateAmpEnvelope(int);
    void updateFreqEnvelope(int);
    void loadPreset(int);
    void bankSelect(int);

private:

    QComboBox * cbAlgo;
    MVAlgoDrawer * algoDrawer;
    QRadioButton * radioPreDef;
    QRadioButton * radioUser;
    MVPushButton * butEditAlgo;
    MVPushButton * butCancelEditAlgo;
    MVPushButton * butSaveAlgo;
    MVPushButton * butAddAlgo;

    QComboBox * cbPreset;
    QComboBox * cbBank;
    MVPushButton * butNewBank;
    MVPushButton * butRenameBank;
    QComboBox * cbMidiMap;
    void updateCbMidiMap();
    void updateCbBank();
    QString currentBank;

    MVPushButton * butSavePreset;
    MVPushButton * butSavePresetAs;
    MVPushButton * butRenamePreset;
    MVPushButton * butDeletePreset;

    MVPushButton * butSaveMidiMap;
    MVPushButton * butSaveMidiMapAs;
    MVPushButton * butRenameMidiMap;
    MVPushButton * butDeleteMidiMap;

    MVPushButton * editSender;
    QString oldFileName;

    QList<QFileInfo>  presetList;
    QList<QFileInfo>  bankList;
    QList<QFileInfo>  midiMapList;

    MVDial * dialDiapason;
    MVPushButton * butTest;

    QPushButton * butPanic;

    MVOscillator_UI ** oscUI;
    MVEnvelope_UI ** envAmpUI;
    MVEnvelope_UI ** envFreqUI;
    MVMeter * meterL;
    MVMeter * meterR;
    MVSlider * volSlider;
    MVBackground * oscBg[NB_OSC];
    //QWidget * oscBg[NB_OSC];
    void loadPreset(QString);
    void loadMidiMap(QString);
    void updateCbAlgo();

public slots:
    void onButtSaveMidiMap();
    void onButtSaveMidiMapAs();
    void onButtRenameMidiMap();
    void onButtDeleteMidiMap();
    void midiMapReturnPressed();
    void editingMidiMapFinished();
    void saveMidiMap(QString);

    void onButtSavePreset();
    void onButtSavePresetAs();
    void onButtRenamePreset();
    void onButtDeletePreset();
    void presetReturnPressed();
    void editingPresetFinished();
    void savePreset(QString);

    void onButtNewBank();
    void onButtRenameBank();
    void bankReturnPressed();
    void editingNewBankFinished();

    void setTimeScales();

private slots:
  void midiMapChanged(int);
  void presetChanged(int);
  void bankChanged(int);
  void algoChanged(int);
  void startEditAlgo();
  void cancelEditAlgo();
  void saveAlgo();
  void addAlgo();
  void algoTypeChanged();
  void mainVolChanged(int);
  void updateCbPresets();
  void onPlayTest();
  void onStopTest();
  void onButtPanic();
  void diapasonChanged(int);
};

#endif // MVFRAME_H
