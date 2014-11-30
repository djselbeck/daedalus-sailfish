#ifndef ODYSSEYMEDIAPLAYER_H
#define ODYSSEYMEDIAPLAYER_H

#include <QObject>
#include <QMediaPlayer>

class OdysseyMediaPlayer : public QObject
{
    Q_OBJECT
public:
    explicit OdysseyMediaPlayer(QObject *parent = 0);

signals:

public slots:

private:

};

#endif // ODYSSEYMEDIAPLAYER_H
