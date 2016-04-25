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

#include "MVCheckBoxAction.h"

#include <QHBoxLayout>
#include <QLabel>

MVCheckBoxAction::MVCheckBoxAction (const QString& title) : QWidgetAction (NULL)
{
    QWidget* widget = new QWidget ();
    QHBoxLayout* layout = new QHBoxLayout();
    QLabel * label = new QLabel (title);
    layout->setAlignment(Qt::AlignRight);
    layout->addWidget (label);
    checkBox = new QCheckBox();
    layout->addWidget (checkBox);
    widget->setLayout (layout);
    setDefaultWidget(widget);
}
