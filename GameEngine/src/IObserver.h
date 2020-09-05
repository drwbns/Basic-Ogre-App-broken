#ifndef IOBSERVER_H
#define IOBSERVER_H

#include <map>
#include <string>
#include <vector>

class IObserver
{
	public:
		virtual ~IObserver(){}
		virtual void Update(int message) = 0;
		std::string GetName(){ return mName; }
	private:
		std::string mName;
};

#endif