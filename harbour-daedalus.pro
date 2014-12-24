# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-daedalus

CONFIG += sailfishapp

QT += multimedia sql dbus

INCLUDEPATH += src

SOURCES += \
    src/maincontroller.cpp \
    src/model/albumsmodel.cpp \
    src/model/artistsmodel.cpp \
    src/model/albumtracksmodel.cpp \
    src/model/trackobject.cpp \
    src/model/playbackstatusobject.cpp \
    src/metadata/albuminformation.cpp \
    src/metadata/artistinformation.cpp \
    src/metadata/imagedatabase.cpp \
    src/metadata/imagedownloader.cpp \
    src/metadata/lastfmalbumprovider.cpp \
    src/metadata/lastfmartistprovider.cpp \
    src/metadata/qmlimageprovider.cpp \
    src/harbour-daedalus.cpp \
    src/mediaplayer/lastfmscrobbler.cpp \
    src/model/playlistmanager.cpp \
    src/model/plsparser.cpp \
    src/model/savedplaylistsmodel.cpp \
    src/model/savedplaylisttracksmodel.cpp \
    src/model/m3uparser.cpp \
    src/model/plswriter.cpp \
    src/model/playbackstate.cpp \
    src/dbus/daedalusdbusadaptor.cpp \
    src/dbus/daedalusdbusmainmprisadaptor.cpp \
    src/mediaplayer/daedalusmediaplayer.cpp

OTHER_FILES += qml/harbour-daedalus.qml \
    rpm/harbour-daedalus.changes.in \
    rpm/harbour-daedalus.yaml \
    translations/*.ts \
    harbour-daedalus.desktop \
    qml/pages/MainPage.qml \
    qml/pages/AlbumsList.qml \
    qml/components/AlbumListDelegate.qml \
    qml/components/SectionScroller.js \
    qml/components/SpeedScroller.js \
    qml/components/AlbumDelegate.qml \
    qml/components/AlbumShowDelegate.qml \
    qml/components/SectionScroller.qml \
    qml/components/SpeedScroller.qml \
    qml/pages/ArtistsList.qml \
    qml/components/ArtistDelegate.qml \
    qml/components/ArtistListDelegate.qml \
    qml/components/ArtistShowDelegate.qml \
    qml/commonfunctions/clickHandler.js \
    qml/pages/AlbumTracks.qml \
    qml/pages/AlbumInfoPage.qml \
    qml/pages/ArtistInfoPage.qml \
    qml/pages/settings/AboutPage.qml \
    qml/pages/settings/DatabaseSettings.qml \
    qml/pages/settings/GUISettings.qml \
    qml/pages/settings/SettingsPage.qml \
    qml/pages/SongDialog.qml \
    qml/components/ScrollLabel.qml \
    qml/components/ToggleImage.qml \
    qml/pages/CurrentPlaylistPage.qml \
    qml/components/PlaylistSectionDelegate.qml \
    qml/pages/CurrentSong.qml \
    qml/components/CoverPage.qml \
    qml/components/ControlPanel.qml \
    harbour-daedalus.png \
    qml/pages/settings/ScrobbleSettings.qml \
    qml/pages/SavedPlaylistsPage.qml \
    qml/pages/PlaylistTracksPage.qml \
    translations/harbour-daedalus-ru.ts

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
TRANSLATIONS += translations/harbour-daedalus-de.ts \
                translations/harbour-daedalus-ru.ts

HEADERS += \
    src/maincontroller.h \
    src/model/albumsmodel.h \
    src/global.h \
    src/model/artistsmodel.h \
    src/model/albumtracksmodel.h \
    src/model/trackobject.h \
    src/model/playbackstatusobject.h \
    src/metadata/albuminformation.h \
    src/metadata/artistinformation.h \
    src/metadata/databasestatistic.h \
    src/metadata/imagedatabase.h \
    src/metadata/imagedownloader.h \
    src/metadata/lastfmalbumprovider.h \
    src/metadata/lastfmartistprovider.h \
    src/metadata/qmlimageprovider.h \
    src/metadata/album.h \
    src/mediaplayer/lastfmscrobbler.h \
    src/model/playlistmanager.h \
    src/model/plsparser.h \
    src/model/savedplaylistsmodel.h \
    src/model/savedplaylisttracksmodel.h \
    src/model/m3uparser.h \
    src/model/plswriter.h \
    src/model/playbackstate.h \
    src/dbus/daedalusdbusadaptor.h \
    src/dbus/daedalusdbusmainmprisadaptor.h \
    src/mediaplayer/daedalusmediaplayer.h

RESOURCES += \
    images.qrc

DEFINES += QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS QT_NO_DEBUG_OUTPUT
#DEFINES += QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS



INCLUDEPATH += $$PWD/libqtsparql/include/Qt5Sparql

QMAKE_RPATHDIR += /usr/share/harbour-daedalus/lib

LIBS += -L$$PWD/libqtsparql/lib/ -lQt5Sparql

lib.files += libqtsparql/lib/libQt5Sparql.so.0 \
        libqtsparql/lib/sparqldrivers

lib.path = /usr/share/harbour-daedalus/lib
INSTALLS += lib
