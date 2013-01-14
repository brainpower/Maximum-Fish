#include "DetailsSpecies.hpp"

DetailsSpecies::DetailsSpecies(const std::shared_ptr<Species>& _species)
{
    currentSpecies = _species;

    DetailsBox = sfg::Box::Create( sfg::Box::VERTICAL, 3.0f );
        sfg::Label::Ptr title(sfg::Label::Create("Details Species"));
        sfg::Box::Ptr ub(sfg::Box::Create( sfg::Box::HORIZONTAL,3.0f ));
            sfg::Scale::Ptr u( sfg::Scale::Create(0,255,1,sfg::Range::HORIZONTAL) );
            u->SetValue(200);
            sfg::Label::Ptr ul( sfg::Label::Create( "Max Age: " + std::to_string(currentSpecies->getMaxAge())));
            ub->Pack(u);
            ub->Pack(ul);
        sfg::Box::Ptr vb(sfg::Box::Create( sfg::Box::HORIZONTAL, 3.0f ));
            sfg::Scale::Ptr v( sfg::Scale::Create(0,255,1,sfg::Range::HORIZONTAL) );
            v->SetValue(200);
            sfg::Label::Ptr vl( sfg::Label::Create("ValueChanger2" ));
            vb->Pack(v);
            vb->Pack(vl);
        sfg::Box::Ptr wb(sfg::Box::Create( sfg::Box::HORIZONTAL, 3.0f ));
            sfg::Scale::Ptr w( sfg::Scale::Create(0,255,1,sfg::Range::HORIZONTAL) );
            w->SetValue(200);
            sfg::Label::Ptr wl( sfg::Label::Create("ValueChanger3" ));
            wb->Pack(w);
            wb->Pack(wl);
        sfg::Box::Ptr xb(sfg::Box::Create( sfg::Box::HORIZONTAL, 3.0f ));
            sfg::Scale::Ptr x( sfg::Scale::Create(0,255,1,sfg::Range::HORIZONTAL) );
            x->SetValue(200);
            sfg::Label::Ptr xl( sfg::Label::Create("ValueChanger4" ));
            xb->Pack(x);
            xb->Pack(xl);
        sfg::Box::Ptr yb(sfg::Box::Create( sfg::Box::HORIZONTAL, 3.0f ));
            sfg::Scale::Ptr y( sfg::Scale::Create(0,255,1,sfg::Range::HORIZONTAL) );
            y->SetValue(200);
            sfg::Label::Ptr yl( sfg::Label::Create("ValueChanger5" ));
            yb->Pack(y);
            yb->Pack(yl);
        sfg::Box::Ptr zb(sfg::Box::Create( sfg::Box::HORIZONTAL, 3.0f ));
            sfg::Scale::Ptr z( sfg::Scale::Create(0,255,1,sfg::Range::HORIZONTAL) );
            z->SetValue(200);
            sfg::Label::Ptr zl( sfg::Label::Create("ValueChanger6" ));
            zb->Pack(z);
            zb->Pack(zl);

    DetailsBox->Pack(title);
    DetailsBox->Pack(ub);
    DetailsBox->Pack(vb);
    DetailsBox->Pack(wb);
    DetailsBox->Pack(xb);
    DetailsBox->Pack(yb);
    DetailsBox->Pack(zb);
}

sfg::Box::Ptr DetailsSpecies::Get()
{
    return DetailsBox;
}

void DetailsSpecies::HandleEvent( Event& e )
{
	//if (e.Is("EVENTNAME"))
	{

    }
}
