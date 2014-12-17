#include "daedalusdbusmainmprisadaptor.h"

#include <QCoreApplication>

DaedalusDbusMainMprisAdaptor::DaedalusDbusMainMprisAdaptor(QObject *parent) : QDBusAbstractAdaptor(parent)
{

}



void DaedalusDbusMainMprisAdaptor::Raise() {

}

void DaedalusDbusMainMprisAdaptor::Quit() {
    QCoreApplication::quit();
}


bool DaedalusDbusMainMprisAdaptor::canQuit() {
    return true;
}

bool DaedalusDbusMainMprisAdaptor::canRaise() {
    return false;
}

bool DaedalusDbusMainMprisAdaptor::hasTrackList() {
    return false;
}

QString DaedalusDbusMainMprisAdaptor::getIdentity() {
    return "daedalus";
}
