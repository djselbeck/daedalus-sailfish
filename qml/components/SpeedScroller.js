
var itmCount = 0;

function fillSections(listview) {
    if(!listview || !listview.model) {
        console.debug("NO LISTMODEL");
        return;
    }
    console.debug("Playlist count changed");
    var count = (typeof listview.model.rowCount === 'undefined' ? listview.model.length : listview.model.rowCount());
    itmCount = count;
}

function getItemPositionFromRelative(position) {
    var index = Math.ceil(((itmCount)*position)/100);
    return index-1;
}

