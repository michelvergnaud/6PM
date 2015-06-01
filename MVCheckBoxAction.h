#ifndef MVCHECKBOXACTION_H
#define MVCHECKBOXACTION_H

#include <QWidgetAction>
#include <QCheckBox>

class MVCheckBoxAction : public QWidgetAction
{
public:
    MVCheckBoxAction (const QString& title);
    QCheckBox * getCheckBox() {return checkBox;}

private:
    QCheckBox * checkBox;
};

#endif // MVCHECKBOXACTION_H
