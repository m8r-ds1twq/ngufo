#include "MucBookmarks.h"
#include "JabberStream.h"
#include "TimeFunc.h"
#include "Log.h"

//===
#include "ProcessMUC.h"
#include "config.h"
//===

ProcessResult MucBookmarks::blockArrived( JabberDataBlockRef block, const ResourceContextRef rc ) {
    std::string &type=block->getAttribute("type");
    if (type=="error") {
        return LAST_BLOCK_PROCESSED;
    }

    bookmarksAvailable=true;
    if (type=="result") {
        JabberDataBlockRef pvt=block->findChildNamespace("query","jabber:iq:private");
        if (!pvt) return LAST_BLOCK_PROCESSED;
        JabberDataBlockRef storage=pvt->findChildNamespace("storage","storage:bookmarks");

        JabberDataBlockRefList::iterator i=storage->getChilds()->begin();
        while (i!=storage->getChilds()->end()) {
            JabberDataBlockRef item=*(i++);
            const std::string &tagName=item->getTagName();

            MucBookmarkItem::ref b=addNewBookmark();
            b->name=item->getAttribute("name");

            if (tagName=="url") {
                b->url=item->getAttribute("url");
                if (b->name.empty()) b->name=b->url;
            }

			if (tagName=="conference" & Config::getInstance()->autoMUC) {
                b->jid=item->getAttribute("jid");
                b->nick=item->getChildText("nick");
                b->password=item->getChildText("password");
                const std::string &autoJoin=item->getAttribute("autojoin");
                b->autoJoin=(autoJoin=="1" || autoJoin=="true"); 
				if (b->autoJoin) 
				{
					Jid roomNode;
					roomNode.setJid(b->jid);
					roomNode.setResource(b->nick);
					ProcessMuc::initMuc(roomNode.getJid(), b->password, rc);	
					JabberDataBlockRef joinPresence=constructPresence(
						roomNode.getJid().c_str(), 
						rc->status, 
						rc->presenceMessage, 
						rc->priority); 
					JabberDataBlockRef xMuc=joinPresence->addChildNS("x", "http://jabber.org/protocol/muc");
					if (b->password!="") xMuc->addChild("password",b->password.c_str());
	
		            if (rc->isLoggedIn()) // ну это больше дл€ пор€дку, хот€ может ведь и дисконнект быть неожиданно ? :)
				        rc->jabberStream->sendStanza(joinPresence);	// а вот это как € пон€л уже входим в конференцию с заданными данными			
					}
            }
        }
		//—ортировать?
		if (Config::getInstance()->autoMUCBS) std::stable_sort(bookmarks.begin(), bookmarks.end(), MucBookmarkItem::compare);
    }
    Log::getInstance()->msg("Bookmarks received successfully");
    return LAST_BLOCK_PROCESSED;
}

MucBookmarkItem::ref MucBookmarks::addNewBookmark() {
    bookmarks.push_back(MucBookmarkItem::ref(new MucBookmarkItem()));
    return bookmarks.back();
}
void MucBookmarks::doQueryBookmarks( ResourceContextRef rc ) {
    bookmarksAvailable=false;
    id=strtime::getRandom();
    JabberDataBlock getBm("iq");
    getBm.setAttribute("type", "get");
    getBm.setAttribute("id", id.c_str());
    getBm.addChildNS("query","jabber:iq:private")->
          addChildNS("storage","storage:bookmarks");
    rc->jabberStanzaDispatcherRT->addListener(rc->bookmarks);
    rc->jabberStream->sendStanza(getBm);
}
MucBookmarkItem::ref MucBookmarks::get( int i ) { return bookmarks[i]; }

void MucBookmarks::set(int i,MucBookmarkItem::ref bm) { 
	bookmarks[i]=bm;
	//getDm.
}

void MucBookmarks::save() { 
	Log::getInstance()->msg("Starting Bookmarks saving...");
	id=strtime::getRandom();
	JabberDataBlock SetBm("id");
    SetBm.setAttribute("type", "set");
    SetBm.setAttribute("id", id.c_str());
    SetBm.addChildNS("query","jabber:iq:private")->
          addChildNS("storage","storage:bookmarks");
/*
	for (int i=0; i<getBookmarkCount(); i++)
	{
		MucBookmarkItem::ref bm=bookmarks[i];
		JabberDataBlockRef item;
		//const std::string &tagName=item->getTagName();
			
			if (!bm->url.empty()) {
				item->setTagName("url");
				item->setAttribute("url",bm->url);
            } else {
				item->setTagName("conference");
				item->setAttribute("jid",bm->jid);
				item->setAttribute("password",bm->password);
				//item->setAttribute("autojoin",(const std::string&)bm->autoJoin);
				//item->addChild("nick",(const char*&)bm->nick);
			}
		SetBm.addChild(item);
	}
*/
	Log::getInstance()->msg(SetBm.getText());
	Log::getInstance()->msg("Bookmarks save successfully");

}
int MucBookmarks::getBookmarkCount() const { return bookmarks.size(); }

bool MucBookmarkItem::compare( MucBookmarkItem::ref l, MucBookmarkItem::ref r ) {
    return (l->name.compare(r->name) < 0);

}