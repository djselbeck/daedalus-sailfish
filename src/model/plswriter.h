#ifndef PLSWRITER_H
#define PLSWRITER_H

#include "trackobject.h"

#include <QObject>

class PLSWriter : public QObject
{
    Q_OBJECT
public:
    explicit PLSWriter(QObject *parent = 0);
    static bool writePlaylist(QList<TrackObject*> *list, QUrl filename,QString name);

signals:

public slots:

};

#endif // PLSWRITER_H
