#include "DetailsTile.hpp"

#include <boost/format.hpp>
using boost::format;
using boost::io::group;

DetailsTile::DetailsTile(const std::shared_ptr<Tile>& _tile)
{
    currentTile = _tile;

    DetailsLabel = sfg::Label::Create();
    DetailsLabel->SetAlignment( sf::Vector2f(0.f, 0.f) );

    UpdateLabel();
}

void DetailsTile::UpdateLabel()
{

    DetailsLabel->SetText("");
    AddToLabel( DetailsLabel, "Position", str(format("%.2f,%.2f") % currentTile->getPosition().x % currentTile->getPosition().y ));
    AddToLabel( DetailsLabel, "Height", str(format("%.2f") % currentTile->getHeight()));
    AddToLabel( DetailsLabel, "Nutrition", str(format("%.2f") % currentTile->getNutrition()));
    AddToLabel( DetailsLabel, "Humidity", str(format("%.2f / %.2f") % currentTile->getHumidity() % currentTile->getBaseHumidity() ));
    AddToLabel( DetailsLabel, "Temperature", str(format("%.2f") % currentTile->calcTemperature() ));
}

sfg::Widget::Ptr DetailsTile::Get()
{
    return DetailsLabel;
}
