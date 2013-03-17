#ifndef Frontend_H
#define Frontend_H

#include "sbe/Module.hpp"

#include <memory>

namespace sbe {
	class Screen;
}

class SimView;

/// Renderer module for the Frontend
class MRenderer : public sbe::Module
{
	public:
		MRenderer();
		virtual ~MRenderer();


	private:
		virtual void Init();
		virtual void DeInit();

		std::shared_ptr<sbe::Screen> Scr;
		std::shared_ptr<SimView> Sim;

};

#endif // Frontend_H

