#include "Game.h"

#include "modules/Renderer.h"
#include "modules/Logic.h"

Game* Game::Instance;

Game::Game()
{
	Instance = this;
}

Game::~Game()
{
	Instance = nullptr;
}

void Game::GameInit()
{
}

void Game::DeInit()
{
}

void Game::ModuleInit()
{

	Engine::out() << "View..." << std::endl;

	RegisterModule( new Renderer,
					ModuleStartInfo(
						"Renderer",
						60,
						std::shared_ptr<Event> (new Event("EVT_FRAME"))
					)
				  );

	Engine::out() << "Logic..." << std::endl;
	RegisterModule( new Logic,
					ModuleStartInfo(
						"Logik",
						20,
						std::shared_ptr<Event> (new Event( "EVT_TICK" ))
					)
				  );

	Engine::out() << "Engine Initialisation done..." << std::endl;
}
