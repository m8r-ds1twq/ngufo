//#include "stdafx.h"
#include "JabberStream.h"

#include "JabberAccount.h"
#include "XmppError.h"
//#include <boost/thread.hpp>
//#include <boost/bind.hpp>
#include <stack>
#include <exception>

#include <utf8.hpp>
#include <windows.h>

JabberStream::JabberStream(void){
    isRunning=false;
}

void JabberStream::run(JabberStream * _stream){
	Log::getInstance()->msg("Reader thread strated");

	try {
        if (_stream->connection==NULL) {
            _stream->jabberListener->connect();
        }
        _stream->parser->bindStream( _stream->connection );
	} catch (std::exception ex) {
        _stream->jabberListener->endConversation(&ex);
        _stream->isRunning=false;
        Log::getInstance()->msg("Reader thread stopped");
        _stream->rc->jabberStream=JabberStreamRef();
        return;
	}

    _stream->isRunning=true;
}

void JabberStream::parseStream() {
    if (!isRunning) return;
    try {
        parser-> parseStream();
    } catch (std::exception ex) {
        jabberListener->endConversation(&ex);
        isRunning=false;
        Log::getInstance()->msg("Reader thread stopped");
        rc->jabberStream=JabberStreamRef();
    }
}

void JabberStream::tagStart(const std::string & tagname, const StringMap &attr) {

	if (tagname=="xml") return;
	
    JabberDataBlockRef blk=JabberDataBlockRef( new JabberDataBlock( tagname, attr ) );

	if (tagname=="stream:stream") {

		//begin conversation
		JabberListener * listener=jabberListener.get();
		if (listener!=NULL) listener->beginConversation( blk );
		
		return;
	}

	// stanzas
	xmlStack.push( blk);
}

bool JabberStream::tagEnd(const std::string & tagname) {
    if (xmlStack.empty()) {
        if (tagname=="stream:stream") throw std::exception("Stream normal shutdown");
        else throw std::exception("XML Stack underflow");
    }
	JabberDataBlockRef stanza=xmlStack.top();
	xmlStack.pop();
	if (xmlStack.empty()) {
		//full stanza arrived
        
        if (stanza->getTagName()!=tagname) {
            throw std::exception("XML: Tag mismatch");
        }

        if (tagname=="stream:error") {
            XmppError::ref xe=XmppError::decodeStreamError(stanza);
            std::string err("Stream error: ");
            err+=xe->toString();
            throw std::exception(err.c_str());
        }

		JabberStanzaDispatcher * dispatcher= rc->jabberStanzaDispatcherRT.get();
		if (dispatcher!=NULL) 
            if (! dispatcher->dispatchDataBlock(stanza)) {
                //all lesteners rejected this stanza
                if (tagname=="iq") {

                    std::string &type=stanza->getAttribute("type");
                    if (type=="error") return false; //don't reply to error stanza
                    if (type=="result") return false; //don't reply to result stanza

                    //client sould reject unknown iq-stanza
                    JabberDataBlock iqError("iq");
                    iqError.setAttribute("type", "error");
                    iqError.setAttribute("to", stanza->getAttribute("from"));
                    iqError.setAttribute("id", stanza->getAttribute("id"));
                    //todo: optional iq child blocks
                    JabberDataBlockRef err=iqError.addChild("error", NULL);
                        err->setAttribute("type","cancel");
                        err->addChildNS("feature-not-implemented", "urn:ietf:params:xml:ns:xmpp-stanzas");
                    sendStanza(iqError);
                }
            }

		//puts(element->toXML()->c_str());
	} else {
		xmlStack.top()->addChild(stanza);
	}
    return false;
}

void JabberStream::plainTextEncountered(const std::string & body){
    if (xmlStack.empty()) return;
	xmlStack.top()->setRawText(body);
}

#ifdef _WIN32_WCE
DWORD jabberStreamThread(LPVOID param) {
#else
DWORD WINAPI jabberStreamThread(LPVOID param) {
#endif
	JabberStream::run((JabberStream *)param);
	return 1;
}

JabberStream::JabberStream(ResourceContextRef rc, JabberListenerRef listener){

	parser=XMLParserRef(new XMLParser(this));

	this->rc=rc;
    this->jabberListener=listener;

    isRunning=false;

	HANDLE thread=CreateThread(NULL, 0, jabberStreamThread, this, 0, NULL);
    SetThreadPriority(thread, THREAD_PRIORITY_IDLE);
	//boost::thread test( boost::bind(run, this) );
}

JabberStream::~JabberStream(void){

	Log::getInstance()->msg("JabberStream destructor called");
}

void JabberStream::sendStanza(JabberDataBlockRef stanza){
    try {
	    connection->write( stanza->toXML() );
    } catch (std::exception ex) {
        jabberListener->endConversation(&ex);
        connection->close();
    }
}

void JabberStream::sendStanza(JabberDataBlock &stanza){
    try {
	    connection->write( stanza.toXML() );
    } catch (std::exception ex) {
        jabberListener->endConversation(&ex);
        connection->close();
    }
}

void JabberStream::sendXmlVersion(void){
	connection->write("<?xml version='1.0'?>", 21);
}

void JabberStream::sendXmppBeginHeader(){
	std::string header=
		"<stream:stream "
		"xmlns:stream='http://etherx.jabber.org/streams' "
		"xmlns='jabber:client' "
		"to='";
	header+=rc->account->getServer();
	header+='\'' ;
	if (rc->account->useSASL) header+=" version='1.0'";
	header+=" >";

	connection->write(header);
}

void JabberStream::sendXmppEndHeader(void){
	connection->write(std::string("</stream:stream>"));
}