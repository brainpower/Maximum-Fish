#include "DetailsTile.hpp"

#include <boost/format.hpp>
using boost::format;
using boost::io::group;

DetailsTile::DetailsTile(const std::shared_ptr<Tile>& _tile)
{
    currentTile = _tile;
    DetailsBox = sfg::Box::Create( sfg::Box::VERTICAL, 3.0f );
    Update();
}

void DetailsTile::Update()
{
    DetailsBox->RemoveAll();
    AddLabel( DetailsBox, "Position: ", str(format("%.2f,%.2f") % currentTile->getPosition().x % currentTile->getPosition().y ));
    AddLabel( DetailsBox, "Height: ", str(format("%.2f") % currentTile->getHeight()));
    AddLabel( DetailsBox, "Nutrition: ", str(format("%.2f") % currentTile->getNutrition()));
    AddLabel( DetailsBox, "Humidity : ", str(format("%.2f / %.2f") % currentTile->getHumidity() % currentTile->getBaseHumidity() ));

}

sfg::Box::Ptr DetailsTile::Get()
{
    return DetailsBox;
}
