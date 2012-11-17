#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "sbe/event/EventUser.hpp"

#include <SFML/NonCopyable.hpp>

class Simulator : public EventUser, public sf::Noncopyable
{
	public:
		Simulator();
		virtual ~Simulator() {};
		virtual void HandleEvent( Event& e);
		
		static std::shared_ptr<Terrain> GetTerrain()
		{
			return Instance->Terra;
		}
		
		private:
			std::shared_ptr<Terrain> Terra;
			
			static Simulator* Instance;
};

#endif // SIMULATOR_H
