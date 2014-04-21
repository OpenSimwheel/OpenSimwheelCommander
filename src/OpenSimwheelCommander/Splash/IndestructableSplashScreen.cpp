#include "IndestructableSplashScreen.h"
#include "QEvent"

IndestructableSplashScreen::IndestructableSplashScreen(const QPixmap& pixmap)
    :QSplashScreen(pixmap)
{
    this->setPixmap(pixmap);
    this->installEventFilter(this);
}

bool IndestructableSplashScreen::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
      {
       return true;
      }
      return false;
}
