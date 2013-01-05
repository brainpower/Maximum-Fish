#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "sbe/event/EventUser.hpp"
#include "Terrain.hpp"
#include "Creature.hpp"
#include "Tile.hpp"

#include <list>

#include <SFML/System/NonCopyable.hpp>

class Simulator : public EventUser, public sf::NonCopyable
{
	public:
		Simulator();
		virtual ~Simulator() {};
		virtual void HandleEvent( Event& e);

		void tick();

		static std::shared_ptr<Terrain> GetTerrain()
		{
			return Instance->Terra;
		}

	private:

		std::list<Tile> m_tiles;
		std::list<std::shared_ptr<Creature>> m_creatures;
		std::shared_ptr<Terrain> Terra;

		bool m_pause;

		static Simulator* Instance;
};

#endif // SIMULATOR_H
