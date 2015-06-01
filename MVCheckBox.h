#ifndef MVCHECKBOX_H
#define MVCHECKBOX_H

#include "MVController.h"

#include <QCheckBox>

class QPixmap;

class MVCheckBox: public QCheckBox, public MVController
{
    Q_OBJECT
public:
    MVCheckBox(QString strOn, QString strOff);
    ~MVCheckBox();
    void updateWidget(){update();}

private slots:
    void contextMenu(QPoint);
    void midiLearn();
    void startWaitingMessage();
    void stopWaitingMessage();
    void removeAssign();
    void setParamValue(int);
    void invertMidiCC(bool);

protected:
   void paintEvent(QPaintEvent *);
    bool hitButton(const QPoint & pos) const;

private:
   QPixmap * pixmapOn;
   QPixmap * pixmapOff;
};

#endif // MVCHECKBOX_H
