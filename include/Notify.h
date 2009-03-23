#pragma once

#include <windows.h>

class Notify {
private:
    Notify() {};
public:
    static void PlayNotify(int x);

    static BOOL vibraOn;
    //enum NOTIFY_TYPE {
	static const int	MSG_IN=45000;
	static const int	MSG_OUT=45001;
	static const int	MSG_MUC_IN=45002;
	static const int	MSG_MUC_OUT=45003;
	static const int	MSG_SUBSCRIBE=45004;
	static const int	MSG_NEW=45005;
	static const int	MSG_FILE_IN=45006;
	static const int	MSG_FILE_OUT=45007;
	static const int	MSG_ON_LINE=45008;
	static const int	MSG_OFF_LINE=45009;
    //};
};