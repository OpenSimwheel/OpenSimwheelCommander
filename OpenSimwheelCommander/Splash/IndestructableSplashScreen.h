#ifndef INDESTRUCTABLESPLASHSCREEN_H
#define INDESTRUCTABLESPLASHSCREEN_H

#include <QSplashScreen.h>

class IndestructableSplashScreen
        : public QSplashScreen
{
    Q_OBJECT
public:
    explicit IndestructableSplashScreen(const QPixmap& pixmap);

protected:
    virtual bool eventFilter(QObject *target, QEvent *event);

};

#endif // INDESTRUCTABLESPLASHSCREEN_H
