#ifndef GAME_H
#define GAME_H

#include "sbe/GameBase.hpp"
#include <memory>

/// Main class
class Game : public sbe::GameBase
{
	public:
		Game();
		~Game();

		///
		virtual void GameInit();
		virtual void ModuleInit();
		virtual void DeInit();


	private:
		static Game* Instance;
};


#endif // GAME_H
