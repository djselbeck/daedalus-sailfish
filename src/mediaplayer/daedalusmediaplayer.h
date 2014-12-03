#ifndef DAEDALUSMEDIAPLAYER_H
#define DAEDALUEMEDIAPLAYER_H

#include <QObject>
#include <QMediaPlayer>

class DaedalusMediaPlayer : public QObject
{
    Q_OBJECT
public:
    explicit DaedalusMediaPlayer(QObject *parent = 0);

signals:

public slots:

private:

};

#endif // DAEDALUSMEDIAPLAYER_H
