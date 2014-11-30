#ifndef PLAYBACKSTATUSOBJECT_H
#define PLAYBACKSTATUSOBJECT_H

#include <QObject>

class PlaybackStatusObject : public QObject
{
    Q_OBJECT
public:
    explicit PlaybackStatusObject(QObject *parent = 0);

signals:

public slots:

};

#endif // PLAYBACKSTATUSOBJECT_H
