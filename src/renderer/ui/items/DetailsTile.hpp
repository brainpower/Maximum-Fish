#ifndef DETAILS_TILE_H
#define DETAILS_TILE_H

#include "renderer/ui/items/Details.hpp"
#include "simulator/world/Tile.hpp"

#include <SFGUI/Entry.hpp>

/// Shows details about a Tile
class DetailsTile : public Details
{
	public:

		DetailsTile(const std::shared_ptr<Tile>& _tile);
		virtual ~DetailsTile() {}
        virtual sfg::Widget::Ptr GetLabel();
        virtual sfg::Widget::Ptr GetBox();
        virtual void UpdateLabel();
        virtual void UpdateBox();
    private:

		std::shared_ptr<Tile> currentTile;

		sfg::Entry::Ptr HeightDiplay;
		sfg::Entry::Ptr NutritionDiplay;
		sfg::Entry::Ptr HumidityDiplay;
};

#endif // DETAILS_TILE_H

