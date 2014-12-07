#ifndef M3UPARSER_H
#define M3UPARSER_H

#include "trackobject.h"

#include <QObject>
#include <QUrl>
#include <QFile>
#include <QSparqlConnection>
#include <QSparqlQueryModel>
#include <QList>


class M3UParser : public QObject
{
    Q_OBJECT
public:
    explicit M3UParser(QObject *parent = 0);
    QList<QUrl>* parsePlaylist(QUrl filename);


signals:

public slots:

private:
    QFile *mPLFile;
    QList<TrackObject> *mList;
    QList<QUrl> *mTrackURLs;

};

#endif // M3UPARSER_H
