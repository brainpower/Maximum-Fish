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

		void addCreature();

		void tick();

		static std::shared_ptr<Terrain> GetTerrain()
		{
			return Instance->Terra;
		}

		static std::default_random_engine GetEngine()
		{
			return Instance->gen;
		}

	private:

		void registerIOPlugins();

		std::default_random_engine gen;
		std::list<std::shared_ptr<Creature>> Creatures;
		std::shared_ptr<Terrain> Terra;

		bool m_pause;

		static Simulator* Instance;
};

#endif // SIMULATOR_H
