/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import "components"
import "pages"

ApplicationWindow
{
    id: mainWindow
    // Controller request signals
    signal requestAlbums();
    signal requestArtistAlbums(string artist);
    signal requestArtists();
    signal requestAlbum(string albumurn);
    signal requestTrackAlbumTracks(string trackurn);
    signal requestTrackArtistAlbums(string trackurn);
    signal requestSavedPlaylists();
    signal requestSavedPlaylist(string url);
    signal requestAllTracks();

    // playlist management
    signal addAlbumTrack(int index);
    signal addAlbumTrackAfterCurrent(int index);
    signal addAlbum(string albumurn);
    signal playAlbum(string albumurn);
    signal addArtist(string artisturn);
    signal playArtist(string artisturn)
    signal playAlbumTrack(int index);
    signal addActiveAlbum();
    signal playPlaylistIndex(int index);
    signal playPlaylistSongNext(int index);
    signal playActiveAlbum();
    signal playActiveAlbumRandom();
    signal deletePlaylist();
    signal savePlaylist(string name);
    signal deletePlaylistTrack(int index);
    signal addSavedPlaylistTrack(int index);
    signal addSavedPlaylistTrackAfterCurrent(int index);
    signal playSavedPlaylistTrack(int index);
    signal addActivePlaylist();
    signal playActivePlaylist();


    // memory preasure methods
    signal clearAlbumsList();
    signal clearAlbumTrackList();
    signal clearArtistsList();
    signal clearSavedPlaylists();
    signal clearSavedPlaylistTracks();

    // basic controls
    signal next();
    signal prev();
    signal play();
    signal stop();
    signal pause();
    signal togglePlayPause();
    signal seek(int pos);
    signal setShuffle(bool random)
    signal setRepeat(bool repeat)

    // Signals change in download-size preference
    signal newDownloadSize(int size)

    // Signals changed settings key
    // [settingkey,value]
    signal newSettingKey(variant setting);
    signal newLastfmUserData(variant data);
    signal clearLastFMAuthentication();

    // Signal for db control
    signal bulkDownloadArtists();
    signal bulkDownloadAlbums();
    signal cleanupBlacklisted();
    signal cleanupArtists();
    signal cleanupAlbums();
    signal cleanupDB();

    // Wiki/Biography information requests for sqlDB
    signal requestArtistInfo(string artist);
    signal requestAlbumInfo(variant album);

    property int populateDuration: 700;
    property int listPadding: Theme.paddingMedium;
    property int bothOrientation: Orientation.Landscape + Orientation.Portrait + Orientation.PortraitInverted + Orientation.LandscapeInverted
    property bool jollaQuickscroll: false;
    property bool mDebugEnabled: false;
    property bool mPositionSliderActive: false

    // Global image sources for play/pause
    property string playbuttoniconsource : playbackstatus.playing ?  "image://theme/icon-m-pause" :  "image://theme/icon-m-play";
    property string playbuttoniconsourcecover :  playbackstatus.playing ? "image://theme/icon-cover-pause" : "image://theme/icon-cover-play";

    property string artistname
    property string albumname
    property string coverimageurl
    property string artistimageurl

    property Page mPlaylistPage
    property Page mCurrentSongPage

    // Nicely formats length values to string
    function formatLength(length)
    {
        if ( length === 0 ) {
            return "0:00"
        }

        var temphours = Math.floor(length/3600);
        var min = 0;
        var sec = 0;
        var temp="";
        if(temphours>1)
        {
            min=(length-(3600*temphours))/60;
        }
        else{
            min=Math.floor(length/60);
        }
        sec = length-temphours*3600-min*60;
        if(temphours===0)
        {
            temp=((min<10?"0":"")+min)+":"+(sec<10?"0":"")+(sec);
        }
        else
        {
            temp=((temphours<10?"0":"")+temphours)+":"+((min<10?"0":"")+min)+":"+(sec<10?"0":"")+(sec);
        }
        return temp;
    }


    function coverArtReceiver(url)
    {
        coverimageurl = url;
    }

    function coverArtistArtReceiver(url)
    {
        artistimageurl = url;
    }

    BusyIndicator {
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: mBusy
    }

    ControlPanel {
            id: quickControlPanel
    }
    bottomMargin: quickControlPanel.visibleSize

    initialPage: Qt.resolvedUrl("pages/MainPage.qml")
    cover: Qt.resolvedUrl("components/CoverPage.qml")

}


