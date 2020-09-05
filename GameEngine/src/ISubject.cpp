#include "ISubject.h"

void ISubject::ObserverSubscribe(int message, IObserver *observer)
{
	if (observer)
	{
		ObserverMap::iterator it = mObservers.find(message);
		if (it == mObservers.end())
		{
			mObservers[message] = ObserverList();
		}
		mObservers[message].push_back(observer);
	}
}

void ISubject::ObserverUnsubscribe(int message, IObserver *observer)
{
	ObserverMap::iterator it = mObservers.find(message);
	if (it != mObservers.end())
	{
		ObserverList &list = mObservers[message];
		ObserverList::iterator li;
		for (li = list.begin(); li != list.end();)
		{
			if ((*li) == observer)
			{
				list.erase(li);
			}
			else
			{
				++li;
			}
		}
	}
}

void ISubject::Notify(int message)
{
	ObserverMap::iterator it = mObservers.find(message);
	if (it != mObservers.end())
	{
		ObserverList &list = mObservers[message];
		ObserverList::iterator li;
		for (li = list.begin(); li != list.end(); ++li)
		{
			(*li)->Update(message);
		}
	}
}

