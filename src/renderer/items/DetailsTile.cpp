#include "DetailsTile.hpp"

#include <boost/format.hpp>
using boost::format;
using boost::io::group;

DetailsTile::DetailsTile(const std::shared_ptr<Tile>& _tile)
{
    currentTile = _tile;

    DetailsBox = sfg::Box::Create( sfg::Box::VERTICAL, 3.0f );
        sfg::Box::Ptr ub(sfg::Box::Create( sfg::Box::HORIZONTAL,3.0f ));
            sfg::Label::Ptr ul( sfg::Label::Create( "Height: " + str(format("%.2f") % currentTile->getHeight())));
            ub->Pack(ul);
        sfg::Box::Ptr vb(sfg::Box::Create( sfg::Box::HORIZONTAL, 3.0f ));
            sfg::Label::Ptr vl( sfg::Label::Create("Position: " + str(format("%.2f,%.2f") % currentTile->getPosition().x % currentTile->getPosition().y )));
            vb->Pack(vl);
        sfg::Box::Ptr wb(sfg::Box::Create( sfg::Box::HORIZONTAL, 3.0f ));
            sfg::Label::Ptr wl( sfg::Label::Create("Nutrition: " + str(format("%.2f") % currentTile->getNutrition() )));
            wb->Pack(wl);
        sfg::Box::Ptr xb(sfg::Box::Create( sfg::Box::HORIZONTAL, 3.0f ));
            sfg::Label::Ptr xl( sfg::Label::Create("Humidity : " + str(format("%.2f / %.2f") % currentTile->getHumidity() % currentTile->getBaseHumidity() )));
            xb->Pack(xl);

    DetailsBox->Pack(ub);
    DetailsBox->Pack(vb);
    DetailsBox->Pack(wb);
    DetailsBox->Pack(xb);
}

sfg::Box::Ptr DetailsTile::Get()
{
    return DetailsBox;
}

void DetailsTile::HandleEvent( Event& e )
{
	//if (e.Is("EVENTNAME"))
	{

    }
}
