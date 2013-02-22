#include "DetailsTile.hpp"

#include <boost/format.hpp>
using boost::format;
using boost::io::group;

DetailsTile::DetailsTile(const std::shared_ptr<Tile>& _tile)
{
    currentTile = _tile;

    //Label
    DetailsLabel = sfg::Label::Create();
    DetailsLabel->SetAlignment( sf::Vector2f(0.f, 0.f) );

    //Box
    DetailsBox = sfg::Box::Create( sfg::Box::VERTICAL, 3.0f );

    UpdateLabel();
    UpdateBox();
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

void DetailsTile::UpdateBox()
{

	DetailsBox->RemoveAll();
	///todo

}

sfg::Widget::Ptr DetailsTile::GetLabel()
{
    return DetailsLabel;
}

sfg::Widget::Ptr DetailsTile::GetBox()
{
    UpdateBox();
    return DetailsBox;
}
