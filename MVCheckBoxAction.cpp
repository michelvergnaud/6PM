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
