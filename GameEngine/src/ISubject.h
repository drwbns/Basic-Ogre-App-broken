#ifndef ISUBJECT_H
#define ISUBJECT_H

#include <map>
#include <vector>

#include "IObserver.h"

class ISubject
{
public:
	ISubject();
	virtual ~ISubject(){}
	virtual void ObserverSubscribe(int message, IObserver*); // Add a observer for a particular message
	virtual void ObserverUnsubscribe(int message, IObserver*); // Unsubscribe an observer for a particular message
	virtual void Notify(int message);

private:
	typedef std::vector<IObserver *> ObserverList;
	typedef std::map<int, ObserverList> ObserverMap;
	ObserverMap mObservers;
};

#endif