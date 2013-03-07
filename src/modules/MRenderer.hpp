#ifndef Frontend_H
#define Frontend_H

#include "sbe/Module.hpp"

#include <memory>

class Screen;
class SimView;

class MRenderer : public Module
{
	public:
		MRenderer();
		virtual ~MRenderer();


	private:
		virtual void Init();
		virtual void DeInit();

		std::shared_ptr<Screen> Scr;
		std::shared_ptr<SimView> Sim;

};

#endif // Frontend_H

