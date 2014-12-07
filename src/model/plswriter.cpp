#include "plswriter.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

PLSWriter::PLSWriter(QObject *parent) :
    QObject(parent)
{
}

bool PLSWriter::writePlaylist(QList<TrackObject *> *list, QUrl filename, QString name)
{
    qDebug() << "Writing playlist: " << filename.toLocalFile();
    QFile plsFile(filename.toLocalFile());
    if ( !plsFile.open(QIODevice::WriteOnly | QIODevice::Text) ) {
        return false;
    }
    QTextStream outStream(&plsFile);
    outStream << "[playlist]"  << '\n' ;
    outStream << "X-GNOME-Title=" << name << '\n';
    outStream << "NumberOfEntries=" << list->size() << '\n';
    for ( int i = 0 ; i < list->size(); i++ ) {
        outStream << "Title" << QString::number(i + 1) << "=" << list->at(i)->getTitle() << '\n';
        outStream << "File" << QString::number(i + 1) << "=" << list->at(i)->getURL().toString(QUrl::PrettyDecoded | QUrl::DecodeReserved)  << '\n';
    }
    outStream.flush();
    qDebug() << "Wrote playlist: " << plsFile.fileName();
    plsFile.close();
    return true;
}
