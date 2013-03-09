#ifndef SELECTOR_H
#define SELECTOR_H

#include "sbe/event/EventUser.hpp"
#include "renderer/items/DetailsEmpty.hpp"
#include "renderer/items/DetailsCreature.hpp"
#include "renderer/items/DetailsSpecies.hpp"
#include "renderer/items/DetailsTile.hpp"

#include <SFGUI/SFGUI.hpp>

/// i have no idea
class Selector : public EventUser
{
	public:

		Selector();
		virtual ~Selector() {};

		virtual void HandleEvent( Event& e );
		virtual sfg::Box::Ptr Get();
    private:
        bool ThisNotManipulator;
        void SetDetail( const std::shared_ptr<Creature>& _creature );
        void SetDetail( const std::shared_ptr<Species>& _species );
        void SetDetail( const std::shared_ptr<Tile>& _tile );
		void SwitchToManipulator();
        void CreateBox();
		sfg::Box::Ptr WholeBox;

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

		sfg::Frame::Ptr CreatureFrame;
		sfg::Frame::Ptr SpeciesFrame;
		sfg::Frame::Ptr TileFrame;
		std::shared_ptr<Details> CurrentDetailsCreature;
		std::shared_ptr<Details> CurrentDetailsSpecies;
		std::shared_ptr<Details> CurrentDetailsTile;

        sfg::Label::Ptr Information;
};

#endif // SELECTOR_H

