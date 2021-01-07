

#ifndef CLI_H_
#define CLI_H_

#include <string.h>
#include <map>
#include "commands.h"

using namespace std;

class CLI {
	DefaultIO* dio;
	// you can add data members
    map<float, Command*> mapping;
    Data data;

public:
	CLI(DefaultIO* dio);
	void start();
	virtual ~CLI();
};

#endif /* CLI_H_ */
