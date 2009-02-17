#pragma once

#include <string>
#include "boostheaders.h"
#include "Serialize.h"

class Config
{
public:
    ~Config(){};

    typedef boost::shared_ptr<Config> ref;

    static Config::ref getInstance();

    bool showOfflines;
    bool showGroups;
    bool sortByStatus;

    bool composing;
    bool delivered;
    bool history;

    bool vibra;
    bool sounds;

    bool raiseSIP;

    bool connectOnStartup;

	/* UFO START */

	bool autoMUC;	//авто вход в конференции
	bool autoLOG;	//автоматическое открытие лога
	bool autoMUCBS;	//автоматическая сортировка закладок
	bool saveClMUC;	//хранить закрытые конференции
	bool autoFEdit;	//авто фокус в поле ввода

	/* UFO END */

    void save();
private:
    void serialize(Serialize &s);
    static Config::ref instance;

	Config();
};
