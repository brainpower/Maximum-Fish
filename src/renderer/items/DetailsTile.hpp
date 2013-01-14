#ifndef DETAILS_TILE_H
#define DETAILS_TILE_H

#include "sbe/event/EventUser.hpp"
#include "renderer/items/Details.hpp"
#include "simulator/Tile.hpp"

#include <SFGUI/SFGUI.hpp>
#include <boost/lexical_cast.hpp>

class DetailsTile : public Details
{
	public:

		DetailsTile(const std::shared_ptr<Tile>& _tile);
		virtual ~DetailsTile() {}
        virtual sfg::Widget::Ptr Get();
        virtual void Update();
    private:
        sfg::Label::Ptr DetailsLabel;

		std::shared_ptr<Tile> currentTile;
};

#endif // DETAILS_TILE_H

