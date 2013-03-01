#ifndef SIMVIEW_H
#define SIMVIEW_H

#include <SFGUI/Widget.hpp>

#include <SFML/System/Vector2.hpp>

#include "sbe/event/EventUser.hpp"
#include "sbe/util/QuadTree.hpp"

#include "sbe/gfx/Camera.hpp"
#include "sbe/gfx/Renderer.hpp"

#include "renderer/SimActors.hpp"

#include <string>
#include <vector>
#include <memory>
#include <random>

class SimView : public EventUser
{
	public:

		SimView();

		void HandleEvent( Event& e );
		void HandleSfmlEvent ( const sf::Event& e);

		/// loads all Textures, Shaders etc required for rendering
		bool LoadResources();
		void setCameraViewport ( const sf::FloatRect& FR) { Cam->setViewport( FR ); }

		void Render();
	private:

		/// the renderer
		Renderer Picasso;
		/// our mediator between Sim and Renderer, creates and updates various Actors from the simevents
		std::shared_ptr<SimActors> ISim;
		std::shared_ptr<Camera> Cam;
		void PostDebugInfo();
		const std::string Name;

};

#endif // SIMVIEW_H

