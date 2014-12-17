#ifndef DAEDALUSDBUSMAINMPRISADAPTOR_H
#define DAEDALUSDBUSMAINMPRISADAPTOR_H

#include <QDBusAbstractAdaptor>

class DaedalusDbusMainMprisAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")
    Q_PROPERTY(bool CanQuit READ canQuit)
    Q_PROPERTY(bool CanRaise READ canRaise)
    Q_PROPERTY(bool HasTrackList READ hasTrackList)
    Q_PROPERTY(QString Identity READ getIdentity)
public:
    DaedalusDbusMainMprisAdaptor(QObject *parent = 0);

public slots:
    void Raise();
    void Quit();

private:
    bool canQuit();
    bool canRaise();
    bool hasTrackList();
    QString getIdentity();
};

#endif // DAEDALUSDBUSMAINMPRISADAPTOR_H
