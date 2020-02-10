#ifndef __ITASK_HPP__
#define __ITASK_HPP__

#include "../type.hpp"

class ITask {
protected:
	task_status_t *status;

public:
	ITask(task_status_t *_status) : status(_status) {
        *(this->status) = INIT;
	};
	virtual void execute();
};

#endif