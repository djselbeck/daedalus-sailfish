#ifndef PLSPARSER_H
#define PLSPARSER_H

#include "trackobject.h"

#include <QObject>
#include <QUrl>
#include <QFile>
#include <QSparqlConnection>
#include <QSparqlQueryModel>
#include <QList>

class PLSParser : public QObject
{
    Q_OBJECT
public:
    explicit PLSParser(QObject *parent = 0);
    QList<QUrl>* parsePlaylist(QUrl filename);

signals:

public slots:

private:
    QFile *mPLFile;
    QList<TrackObject> *mList;
    QList<QUrl> *mTrackURLs;

};

#endif // PLSPARSER_H
