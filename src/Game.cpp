#include "Game.h"

#include "game/modules/LocalGameView.h"
#include "game/modules/Logic.h"
#include "game/AssetManager.h"

Game* Game::Instance;

Game::Game()
{
	Instance = this;
}

Game::~Game()
{
	Instance = nullptr;
}

std::shared_ptr<AssetManager>	Game::GetAssetMgr()
{
	return Instance->AssetMgr;
}

void Game::GameInit()
{
	Engine::out() << "Game Assets..." << std::endl;
	AssetMgr.reset	( new AssetManager );
}

void Game::DeInit()
{
	AssetMgr.reset();
}

void Game::ModuleInit()
{

	Engine::out() << "View..." << std::endl;

	RegisterModule( new LocalGameView,
					ModuleStartInfo(
						"LocalGameview",
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
