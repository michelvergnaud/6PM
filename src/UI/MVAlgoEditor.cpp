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

#include "MVAlgoEditor.h"

#include "Synth/MVNote.h"
#include "MVMainFrame.h"
#include "MVPresetManager.h"

#include <QLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QMenu>

MVAlgoEditorDialog::MVAlgoEditorDialog(QWidget *parent) :  QDialog(parent)
{
    setWindowTitle(tr("6PM Algorithm Editor"));
    setMinimumSize(300,300);
    algoEditor = new MVAlgoEditor;
    QVBoxLayout * layout = new QVBoxLayout();
    setLayout(layout);
    layout->addWidget(algoEditor,10);

    QWidget * buttsWidget = new QWidget(this);
    buttsWidget->setStyleSheet("color:yellow;background-color: black");

    QHBoxLayout * buttsLayout = new QHBoxLayout();
    buttsWidget->setLayout(buttsLayout);
    QPushButton * cancelButton = new QPushButton(tr("Cancel"));
    QPushButton * saveButton = new QPushButton(tr("Save"));
    saveButton->setDefault(true);
    QPushButton * addButton = new QPushButton(tr("Add"));
    buttsLayout->addWidget(cancelButton);
    buttsLayout->addWidget(saveButton);
    buttsLayout->addWidget(addButton);

    layout->addWidget(buttsWidget,0);
    bCancelIfClose = true;
    connect(cancelButton, SIGNAL(clicked()),this, SLOT(cancelAlgo()));
    connect(saveButton, SIGNAL(clicked()),this, SLOT(saveAlgo()));
    connect(addButton, SIGNAL(clicked()),this, SLOT(addAlgo()));

}
void MVAlgoEditorDialog::closeEvent(QCloseEvent * e)
{
    if(bCancelIfClose)
      cancelAlgo();
    e->accept();
}

void MVAlgoEditorDialog::init(int numAlgo)
{
    bCancelIfClose = true;
    saveNumAlgo = numAlgo;
    algoEditor->init();
    algoEditor->update();
}
MVAlgoEditor::MVAlgoEditor() : MVAlgoDrawer()
{
    bFirst = true;
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));

}
MVAlgoEditor::~MVAlgoEditor()
{

}
void MVAlgoEditor::init()
{
    bFirst = true;
}
void MVAlgoEditorDialog::saveAlgo()
{
    //int i = MVNote::userAlgoIndex;
    bCancelIfClose = false;
    if(saveNumAlgo >= 0 && saveNumAlgo < MVPresetManager::userAlgoDescriptors.size())
        MVPresetManager::userAlgoDescriptors[saveNumAlgo] = algoEditor->buildAlgoDescriptor();
     else
        addAlgo();
    close();
}
void MVAlgoEditorDialog::addAlgo()
{
    bCancelIfClose = false;
    int i = MVPresetManager::addUserAlgo(algoEditor->buildAlgoDescriptor());
    Globals::presetManager->setUserAlgoIndex(i);
    Globals::frame->updateAlgos();
    MVNote::createAlgo();
    close();
}

void MVAlgoEditorDialog::cancelAlgo()
{
    close();
    Globals::frame->cancelEditAlgo();
}
void MVAlgoEditor::mouseMoveEvent(QMouseEvent * e)
{
    QPoint p = e->pos();

    if(startConnectIn > -1 || startConnectOut > -1)
    {
        endDrawingLine = e->pos();
        update();
    }

    if(movingRect>-1)
    {
        graphicOscs[movingRect]->moveCenter(p);
        update();
    }
}
void MVAlgoEditor::mouseReleaseEvent(QMouseEvent * e)
{
    for(int i=0;i<NB_OSC;i++)
    {
        if(graphicOscs[i]->isOnPin(e->pos()) && startConnectOut > -1)
        {
            if( ! MVNote::modulatorsData[i].contains(startConnectOut))
            {
                MVNote::modulatorsData[i].append(startConnectOut);
                qSort(MVNote::modulatorsData[i]);
            }
        }
        else
        if(graphicOscs[i]->isOnPout(e->pos()) && startConnectIn > -1)
        {
            if( ! MVNote::modulatorsData[startConnectIn].contains(i))
            {
                MVNote::modulatorsData[startConnectIn].append(i);
                qSort(MVNote::modulatorsData[startConnectIn]);
            }
        }
    }

    if(movingRect > -1)
    {
        int px = (e->x()+stepX/2)/stepX;
        int py = (height()- e->y()+stepY/2)/stepY;
        if(px<1)px=1;
        if(py<1)py=1;
        if(px>NB_OSC)px=NB_OSC;
        if(py>NB_OSC)py=NB_OSC;
        graphicOscs[movingRect]->setPosX(px);
        graphicOscs[movingRect]->setPosY(py);

        QPointF p = QPointF(px*stepX,height()-py*stepY);
        graphicOscs[movingRect]->moveCenter(p);
    }
    MVNote::createAlgo(buildAlgoDescriptor());
    startConnectOut = -1;
    startConnectIn = -1;
    movingRect = -1;
    update();
    Globals::frame->updateUIs();
}

void MVAlgoEditor::removeModulator()
{
    int i = ((QAction*)sender())->data().toInt()/NB_OSC;
    int j = ((QAction*)sender())->data().toInt()%NB_OSC;
    MVNote::modulatorsData[i].removeAt(j);
    MVNote::createAlgo(buildAlgoDescriptor());
    update();
    Globals::frame->updateUIs();
}

void MVAlgoEditor::mousePressEvent(QMouseEvent * e)
{
    movingRect = -1;

    int i=0;
    while(i<NB_OSC && movingRect == -1)
    {
        if(graphicOscs[i]->isOnPin(e->pos()))
            startConnectIn = i;
        else
        if(graphicOscs[i]->isOnPout(e->pos()))
            startConnectOut = i;
        else
        if(graphicOscs[i]->contains(e->pos()))
            movingRect = i;
        i++;
    }
}
void MVAlgoEditor::contextMenu(QPoint p)
{
    for(int i=0;i<NB_OSC;i++)
    {
        if(graphicOscs[i]->contains(p))
        {
            QMenu * menu = new QMenu(this);
            menu->addAction(tr("Oscillator %1").arg(i+1));
            menu->addSeparator();

            QList<int> list = MVNote::modulatorsData[i];

            for(int j=0; j<list.count();j++)
            {
                QAction * action = new QAction(tr("Remove modulator %1").arg(list.at(j)+1),this);
                action->setData(i*NB_OSC+j); // Surely a better way to pass data to slot...
                connect(action,SIGNAL(triggered()),this,SLOT(removeModulator()));
                menu->addAction(action);
            }
            menu->popup(QCursor::pos());
        }
    }
}


void MVAlgoEditor::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    if (bFirst)
        MVAlgoDrawer::drawAlgo(p);
    bFirst = false;
    drawAlgo(p);
}

void MVAlgoEditor::drawAlgo(QPainter & p)
{
    w = width();
    h = height();
    p.setBackground(Qt::black);
    p.eraseRect(rect());
    p.setBrush(Qt::yellow);

    QPen penRect;
    QPen penLines;
    penLines.setColor(Qt::yellow);
    penLines.setWidth(2);
    stepX = w/(NB_OSC+1);
    stepY= h/(NB_OSC+1);
    minPosX = 1;
    maxPosX = NB_OSC;
    minPosY = 1;
    maxPosY = NB_OSC;

    // Drawing connection
    p.setPen(penLines);
    if(startConnectIn > -1)
        drawConnection(p, endDrawingLine, graphicOscs[startConnectIn]->pIn());
    if(startConnectOut > -1)
        drawConnection(p, endDrawingLine, graphicOscs[startConnectOut]->pOut());

    // Draw
    int yOps = stepY;

    // Draw grid
    p.setPen(Qt::darkGray);
    for(int i=minPosX;i<=maxPosX;i++)
        p.drawLine(i*stepX, 0, i*stepX,h);
    for(int i=1;i<=maxPosY;i++)
        p.drawLine(0,h-i*stepY, w, h-i*stepY);
    // Operators line
    p.setPen(Qt::green);
    p.drawLine(0,h-yOps,w,h-yOps);

    for(int i=0;i<NB_OSC;i++)
    {
        if(i != movingRect)
            graphicOscs[i]->moveCenter(QPointF((graphicOscs[i]->getPosX()-minPosX+1)*stepX, h-yOps-(graphicOscs[i]->getPosY()-1)*stepY));
        p.setPen(penRect);
        graphicOscs[i]->draw(p);

        // Draw connections
        p.setPen(penLines);
        // Operators towards bottom
        if(MVNote::isOp(i))
        {
            QPointF pts[2];
            pts[0] = graphicOscs[i]->pOut();
            pts[1] = QPointF(pts[0].x(), h-yOps/4);
            p.drawPolyline(pts,2);
        }
    }
    if(MVNote::nbOps>1)
        p.drawLine(graphicOscs[MVNote::ops[0]]->center().x(),h-(yOps/4),graphicOscs[MVNote::ops[MVNote::nbOps-1]]->center().x(),h-(yOps/4));

    // Modulated towards modulators
    for(int i=0;i<NB_OSC;i++)
    {
        for(int j=0;j<MVNote::modulatorsData[i].count();j++)
        {
            int n = MVNote::modulatorsData[i].at(j);
            drawConnection(p, graphicOscs[i]->pIn(),graphicOscs[n]->pOut());

        }
    }
}

