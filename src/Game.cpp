#include "Game.hpp"

#include "modules/MRenderer.hpp"
#include "modules/Logic.hpp"

#include "sbe/Config.hpp"
#include "sbe/Engine.hpp"

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

	Engine::out(Engine::INFO) << "[Game] MRenderer..." << std::endl;

	RegisterModule( new MRenderer(),
					ModuleStartInfo(
						"MRenderer",
						Engine::getCfg()->get<int>("renderer.maxFps"),
						std::shared_ptr<Event> (new Event("EVT_FRAME"))
					)
				  );

	Engine::out(Engine::INFO) << "[Game] Logic..." << std::endl;
	RegisterModule( new Logic(),
					ModuleStartInfo(
						"Logik",
						20,
						std::shared_ptr<Event> (new Event( "EVT_TICK" ))
					)
				  );

	Engine::out(Engine::INFO) << "[Game] Initialisation done..." << std::endl;
}
