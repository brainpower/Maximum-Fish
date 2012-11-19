#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "sbe/event/EventUser.hpp"

#include <SFML/System/NonCopyable.hpp>

#include "Terrain.hpp"

class Simulator : public EventUser, public sf::NonCopyable
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
