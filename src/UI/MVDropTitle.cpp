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

#include "UI/MVDropTitle.h"

#include "MVCheckBox.h"

#include <QLabel>
#include <QLayout>
#include <QPainter>

MVDropTitle::MVDropTitle(QWidget * w, QString s, QWidget * parent , bool initialState) : QWidget(parent)
{
    label = new QLabel(s);
    QFont font = label->font();
    font.setPointSize(7);
    label->setFont(font);
    checkBox = new MVCheckBox(this,":/images/up",":/images/down","");
    checkBox->setAssignable(false);
    checkBox->setChecked(initialState);

    dropWidget = w;
    w->setVisible(initialState);
    QHBoxLayout * layout = new QHBoxLayout();
    setLayout(layout);
    layout->setContentsMargins(2,2,2,2);
    layout->addWidget(label,5);
    layout->addWidget(checkBox,1,Qt::AlignRight);

    connect(checkBox,SIGNAL(toggled(bool)),this,SLOT(checkBoxToggled(bool)));
}

void MVDropTitle::checkBoxToggled(bool b)
{
    dropWidget->setVisible(b);
    QFont font = label->font();
    font.setItalic( ! b);
    label->setFont(font);
}
void MVDropTitle::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setBackground(Qt::black);
    p.eraseRect(rect());
}
