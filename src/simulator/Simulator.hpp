#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "sbe/event/EventUser.hpp"

class Tile;
class Terrain;
class Creature;

#include <list>

#include <SFML/System/NonCopyable.hpp>

class Simulator : public EventUser, public sf::NonCopyable
{
	public:
		Simulator();
		virtual ~Simulator() {};
		virtual void HandleEvent( Event& e);

		void init();

		void tick();

		static std::shared_ptr<Terrain> GetTerrain()
		{
			return Instance->Terra;
		}

	private:

		void registerIOPlugins();


		std::list<std::shared_ptr<Creature>> Creatures;
		std::shared_ptr<Terrain> Terra;

		bool m_pause;

		static Simulator* Instance;
};

#endif // SIMULATOR_H
