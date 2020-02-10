#ifndef __ITASK_HPP__
#define __ITASK_HPP__

#include "../type.hpp"

class ITask {
public:
	virtual void execute(void*);
};

#endif