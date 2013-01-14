#include "DetailsCreature.hpp"

#include <boost/format.hpp>
using boost::format;
using boost::io::group;

DetailsCreature::DetailsCreature(const std::shared_ptr<Creature>& _creature)
{
    currentCreature = _creature;

    DetailsBox = sfg::Box::Create( sfg::Box::VERTICAL, 3.0f );
        sfg::Label::Ptr spec(sfg::Label::Create("Spezies: " + currentCreature->getSpeciesString() ));
        sfg::Box::Ptr ub(sfg::Box::Create( sfg::Box::HORIZONTAL,3.0f ));
            sfg::Scale::Ptr u( sfg::Scale::Create(0,255,1,sfg::Range::HORIZONTAL) );
            u->SetValue(200);
            sfg::Label::Ptr ul( sfg::Label::Create( str( format( "Age: %d") % currentCreature->getAge() ) ));
            //ub->Pack(u);
            ub->Pack(ul);
        sfg::Box::Ptr vb(sfg::Box::Create( sfg::Box::HORIZONTAL, 3.0f ));
            sfg::Scale::Ptr v( sfg::Scale::Create(0,255,1,sfg::Range::HORIZONTAL) );
            v->SetValue(200);
            sfg::Label::Ptr vl( sfg::Label::Create( str( format("Health: %d" ) % currentCreature->getCurrentHealth() )));
            //vb->Pack(v);
            vb->Pack(vl);
        sfg::Box::Ptr wb(sfg::Box::Create( sfg::Box::HORIZONTAL, 3.0f ));
            sfg::Scale::Ptr w( sfg::Scale::Create(0,255,1,sfg::Range::HORIZONTAL) );
            w->SetValue(200);
            sfg::Label::Ptr wl( sfg::Label::Create( str( format("Position: %.2f,%.2f" ) % currentCreature->getPosition().x %  currentCreature->getPosition().y ) ));
            //wb->Pack(w);
            wb->Pack(wl);

    DetailsBox->Pack(spec);
    DetailsBox->Pack(ub);
    DetailsBox->Pack(vb);
    DetailsBox->Pack(wb);
}

sfg::Box::Ptr DetailsCreature::Get()
{
    return DetailsBox;
}

void DetailsCreature::HandleEvent( Event& e )
{
	//if (e.Is("EVENTNAME"))
	{

    }
}
