#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "sbe/event/EventUser.hpp"
#include "Species.hpp"

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

		static std::default_random_engine& GetEngine()
		{
			return Instance->gen;
		}

	private:

		//void HandleClick( const Geom::Pointf& Pos );

		void registerIOPlugins();

		std::default_random_engine gen;
		std::list<std::shared_ptr<Creature>> Creatures;
		std::list<std::shared_ptr<Species>> SpeciesList;
		std::shared_ptr<Terrain> Terra;

		bool isPaused;

		static Simulator* Instance;
};

#endif // SIMULATOR_H
