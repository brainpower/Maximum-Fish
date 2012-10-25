#ifndef LOGIC_H
#define LOGIC_H

#include "sbe/Module.h"
#include "sbe/event/EventUser.h"

#include <memory>

class AssetManager;
class World;

class Logic : public Module
{
	public:
		Logic();
		virtual ~Logic();


	private:
		virtual void Init();
		virtual void DeInit();

		static Logic* mInstance;

};


#endif // LOGIC_H

