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
    AddToLabel( DetailsLabel, "Type/SpriteIndex", str(format("%d") % currentTile->getTileSpriteIndex() ));
    AddToLabel( DetailsLabel, "Creatures", str(format("%d") % currentTile->getCreatures().size() ));
    AddToLabel( DetailsLabel, "Position", str(format("%.2f,%.2f") % currentTile->getPosition().x % currentTile->getPosition().y ));
    AddToLabel( DetailsLabel, "Height", str(format("%.2f") % currentTile->getHeight()));
    AddToLabel( DetailsLabel, "Nutrition", str(format("%.2f") % currentTile->getNutrition()));
    AddToLabel( DetailsLabel, "Humidity", str(format("%.2f / %.2f") % currentTile->getHumidity() % currentTile->getBaseHumidity() ));
    AddToLabel( DetailsLabel, "Temperature", str(format("%.2f") % currentTile->calcTemperature() ));
    AddToLabel( DetailsLabel, "Parallelisation ID", str(format("%.2f") % currentTile->getParallelId() ));
}

void DetailsTile::UpdateBox()
{

	DetailsBox->RemoveAll();

	DetailsBox->Pack( sfg::Label::Create( "Height:" ), false, true );
		sfg::Box::Ptr heightBox( sfg::Box::Create( sfg::Box::HORIZONTAL, 0 ) );
            HeightDiplay = sfg::Entry::Create();
            //HeightDiplay->SetRequisition( sf::Vector2f( 40, 0 ) );
            HeightDiplay->SetState( sfg::Widget::State::INSENSITIVE );
            HeightDiplay->SetText( boost::lexical_cast<std::string>( currentTile->getHeight() ) );
            sfg::Button::Ptr heightDown = sfg::Button::Create( "<" );
            heightDown->SetRequisition( sf::Vector2f( 40, 0 ) );
            sfg::Button::Ptr heightUp   = sfg::Button::Create( ">" );
            heightUp->SetRequisition( sf::Vector2f( 40, 0 ) );
                //heightDown->GetSignal( sfg::Button::OnLeftClick ).Connect( &Control::BtnHeightDownClick, this );
                //heightUp->GetSignal(   sfg::Button::OnLeftClick ).Connect( &Control::BtnHeightUpClick, this );

            heightBox->Pack( heightDown, false, false );
            heightBox->Pack( HeightDiplay, true, true );
            heightBox->Pack( heightUp, false, false );
	DetailsBox->Pack( heightBox, false, true );

	DetailsBox->Pack( sfg::Label::Create( "Nutrition:" ), false, true );
		sfg::Box::Ptr nutritionBox( sfg::Box::Create( sfg::Box::HORIZONTAL, 0 ) );
            NutritionDiplay = sfg::Entry::Create();
            //NutritionDiplay->SetRequisition( sf::Vector2f( 40, 0 ) );
            NutritionDiplay->SetState( sfg::Widget::State::INSENSITIVE );
            NutritionDiplay->SetText( boost::lexical_cast<std::string>( currentTile->getNutrition() ) );
            sfg::Button::Ptr nutritionDown = sfg::Button::Create( "<" );
            nutritionDown->SetRequisition( sf::Vector2f( 40, 0 ) );
            sfg::Button::Ptr nutritionUp   = sfg::Button::Create( ">" );
            nutritionUp->SetRequisition( sf::Vector2f( 40, 0 ) );
                //nutritionDown->GetSignal( sfg::Button::OnLeftClick ).Connect( &Control::BtnNutritionDownClick, this );
                //nutritionUp->GetSignal(   sfg::Button::OnLeftClick ).Connect( &Control::BtnNutritionUpClick, this );

            nutritionBox->Pack( nutritionDown, false, false );
            nutritionBox->Pack( NutritionDiplay, true, true );
            nutritionBox->Pack( nutritionUp, false, false );
	DetailsBox->Pack( nutritionBox, false, true );

	DetailsBox->Pack( sfg::Label::Create( "Humidity:" ), false, true );
		sfg::Box::Ptr humidityBox( sfg::Box::Create( sfg::Box::HORIZONTAL, 0 ) );
            HumidityDiplay = sfg::Entry::Create();
            //HumidityDiplay->SetRequisition( sf::Vector2f( 40, 0 ) );
            HumidityDiplay->SetState( sfg::Widget::State::INSENSITIVE );
            HumidityDiplay->SetText( boost::lexical_cast<std::string>( currentTile->getHumidity() ) );
            sfg::Button::Ptr humidityDown = sfg::Button::Create( "<" );
            humidityDown->SetRequisition( sf::Vector2f( 40, 0 ) );
            sfg::Button::Ptr humidityUp   = sfg::Button::Create( ">" );
            humidityUp->SetRequisition( sf::Vector2f( 40, 0 ) );
                //humidityDown->GetSignal( sfg::Button::OnLeftClick ).Connect( &Control::BtnHumidityDownClick, this );
                //humidityUp->GetSignal(   sfg::Button::OnLeftClick ).Connect( &Control::BtnHumidityUpClick, this );

            humidityBox->Pack( humidityDown, false, false );
            humidityBox->Pack( HumidityDiplay, true, true );
            humidityBox->Pack( humidityUp, false, false );
	DetailsBox->Pack( humidityBox, false, true );

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
