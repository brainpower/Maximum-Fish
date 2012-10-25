#ifndef LOGIC_H
#define LOGIC_H

#include "sbe/Module.hpp"
#include "sbe/event/EventUser.hpp"

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

