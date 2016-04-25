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

#ifndef MVALGOEDITOR_H
#define MVALGOEDITOR_H

#include <QDialog>
#include "MVAlgoDrawer.h"

class MVAlgoEditor : public MVAlgoDrawer
{
    Q_OBJECT
public:
    MVAlgoEditor();
    ~MVAlgoEditor();
    void init();

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

     void drawAlgo(QPainter &);

private slots:
    void contextMenu(QPoint);
    void removeModulator();
private :
    bool bFirst;
};

class MVAlgoEditorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MVAlgoEditorDialog(QWidget *parent = 0);
    void init(int);
protected:
    void closeEvent(QCloseEvent *);
private :
    MVAlgoEditor * algoEditor;
    int saveNumAlgo;
    bool bCancelIfClose;

private slots:
    void saveAlgo();
    void cancelAlgo();
    void addAlgo();
};

#endif // MVALGOEDITOR_H
