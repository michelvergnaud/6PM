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

#ifndef MVFILTERENVDIALOG_H
#define MVFILTERENVDIALOG_H

#include  "MVFreqEnvelope_UI.h"

#include <QDialog>

class MVEnvelopeDrawer;

class MVFilterEnvDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MVFilterEnvDialog(QWidget *parent = 0);

    void updateUI();
    MVEnvelopeDrawer * getDrawer(){return envelope_UI->getDrawer();}

signals:

public slots:

private:
    MVFreqEnvelope_UI * envelope_UI;
};

#endif // MVFILTERENVDIALOG_H
