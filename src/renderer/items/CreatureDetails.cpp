#include "CreatureDetails.hpp"



CreatureDetails::CreatureDetails( const Creature*)
{
	CreateBox();
}

void CreatureDetails::CreateBox()
{



    //create details

    Details = sfg::Box::Create( sfg::Box::VERTICAL, 3.0f );
        sfg::Label::Ptr title(sfg::Label::Create("Details"));
        sfg::Box::Ptr ub(sfg::Box::Create( sfg::Box::HORIZONTAL,3.0f ));
            sfg::Scale::Ptr u( sfg::Scale::Create(0,255,1,sfg::Range::HORIZONTAL) );
            u->SetValue(200);
            sfg::Label::Ptr ul( sfg::Label::Create( "ValueChanger1" ));
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

    Details->Pack(title);
    Details->Pack(ub);
    Details->Pack(vb);
    Details->Pack(wb);
    Details->Pack(xb);
    Details->Pack(yb);
    Details->Pack(zb);

}

void CreatureDetails::HandleEvent( Event& e )
{
	//if (e.Is("EVENTNAME"))
	{

    }
}

sfg::Box::Ptr CreatureDetails::Get()
{
    return Details;
}
