#ifndef MVPUSHBUTTON_H
#define MVPUSHBUTTON_H

#include <QPushButton>

class QRadialGradient;

class MVPushButton : public QPushButton
{

public:
    MVPushButton(QString , QWidget*);
    ~MVPushButton();

    static void setColors(QColor c1, QColor c2){color1=c1;color2=c2;}

protected:
    void paintEvent(QPaintEvent*);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent * );

private:
    QString label;
    bool bMouseOver;
    bool bPressed;
    QRadialGradient * gradient;
    static QColor color1;
    static QColor color2;
};

#endif // MVPUSHBUTTON_H
