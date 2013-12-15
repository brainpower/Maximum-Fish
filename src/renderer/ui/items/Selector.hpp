#ifndef SELECTOR_H
#define SELECTOR_H

#include "sbe/event/EventUser.hpp"
#include "renderer/ui/items/DetailsEmpty.hpp"
#include "renderer/ui/items/DetailsCreature.hpp"
#include "renderer/ui/items/DetailsSpecies.hpp"
#include "renderer/ui/items/DetailsTile.hpp"


namespace sfg {
	class Box;
	class Frame;
	class Label;
}

/// i have no idea
class Selector : public sbe::EventUser
{
	public:

		Selector();
		virtual ~Selector() {};

		virtual void HandleEvent( Event& e );
		virtual std::shared_ptr<sfg::Box> Get();
    private:
        bool ThisNotManipulator;
        void SetDetail( const std::shared_ptr<Creature>& _creature );
        void SetDetail( const std::shared_ptr<Species>& species );
        void SetDetail( const std::shared_ptr<Tile>& _tile );
		void SwitchToManipulator();
        void CreateBox();
		std::shared_ptr<sfg::Box> WholeBox;

        void SwitchToCreatureManipulator();
        void ExtendCreatureLabel();
        void ReduceCreatureLabel();

        void SwitchToSpeciesManipulator();
        void ExtendSpeciesLabel();
        void ReduceSpeciesLabel();

        void SwitchToTileManipulator();
        void ExtendTileLabel();
        void ReduceTileLabel();

        bool CreatureEmpty;
        bool SpeciesEmpty;
        bool TileEmpty;

		std::shared_ptr<sfg::Frame> CreatureFrame;
		std::shared_ptr<sfg::Frame> SpeciesFrame;
		std::shared_ptr<sfg::Frame> TileFrame;
		std::shared_ptr<Details> CurrentDetailsCreature;
		std::shared_ptr<Details> CurrentDetailsSpecies;
		std::shared_ptr<Details> CurrentDetailsTile;

        std::shared_ptr<sfg::Label> Information;
};

#endif // SELECTOR_H

