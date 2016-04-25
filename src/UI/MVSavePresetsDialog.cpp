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

#include "MVSavePresetsDialog.h"

#include "MVMainFrame.h"
#include "MVPresetManager.h"
#include "MVGlobals.h"

#include <QLabel>
#include <QCheckBox>
#include <QLayout>
#include <QStringList>
#include <QPushButton>


MVSavePresetsDialog::MVSavePresetsDialog(QWidget *parent) : QDialog(parent)
{
    setModal(true);
    setWindowTitle(tr("6PM modified presets"));
    QVBoxLayout * layout = new QVBoxLayout();
    setLayout(layout);
    table = new QTableWidget(this);
    QPalette pal(Qt::darkGray,Qt::black);
    table->setPalette(pal);

    table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    QStringList headers;
    headers << tr("Bank") << tr("Preset") << " ";
    layout->addWidget(table);

    QStringList presetsList = Globals::presetManager->modifiedPresetsList();
    int nb = presetsList.size();
    table->setRowCount(nb);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(headers);

    for(int i=0;i<nb;i++)
    {
        QString fullName = presetsList.at(i);
        QString bank = fullName.left(fullName.indexOf("/"));
        QString preset = fullName.right(fullName.length()-fullName.lastIndexOf("/")-1);
        table->setCellWidget(i, 0, new QLabel(bank));
        table->setCellWidget(i, 1, new QLabel(preset));
        QCheckBox * cb = new QCheckBox();
        cb->setChecked(true);
        table->setCellWidget(i, 2,cb);
    }
    table->adjustSize();

    QWidget * buttsWidget = new QWidget(this);
    buttsWidget->setStyleSheet("color:yellow;background-color: black");

    QHBoxLayout * buttsLayout = new QHBoxLayout();
    buttsWidget->setLayout(buttsLayout);
    QPushButton * dontSaveButton = new QPushButton(tr("Don't save"));
    QPushButton * saveButton = new QPushButton(tr("Save selected"));
    QPushButton * cancelButton = new QPushButton(tr("Cancel"));
    buttsLayout->addWidget(dontSaveButton);
    buttsLayout->addWidget(saveButton);
    buttsLayout->addWidget(cancelButton);

    layout->addWidget(buttsWidget,0);

    connect(dontSaveButton, SIGNAL(clicked()),this, SLOT(dontSaveModifiedPresets()));
    connect(saveButton, SIGNAL(clicked()),this, SLOT(saveModifiedPresets()));
    connect(cancelButton, SIGNAL(clicked()),this, SLOT(cancel()));

}
MVSavePresetsDialog::~MVSavePresetsDialog()
{}

void MVSavePresetsDialog::saveModifiedPresets()
{
    for(int i=0;i<table->rowCount();i++)
    {
        if(((QCheckBox*)table->cellWidget(i,2))->isChecked())
        {
            QString bank = ((QLabel*)table->cellWidget(i,0))->text();
            QString preset = ((QLabel*)table->cellWidget(i,1))->text();
            MVPreset * p = Globals::presetManager->getPresetByName(bank,preset);
            if(p)
            {
                p->saveData();
                if(p == Globals::presetManager->getCurrentPreset())
                    Globals::frame->setPresetModified(false);
            }
        }
    }
    accept();
}
void MVSavePresetsDialog::dontSaveModifiedPresets()
{
    accept();
}
void MVSavePresetsDialog::cancel()
{
    reject();
}
