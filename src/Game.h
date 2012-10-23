#ifndef GAME_H
#define GAME_H

#include "GameBase.h"
#include <memory>

// this is needed on linux only
#ifndef __MINGW32__
__asm__(".symver memcpy,memcpy@GLIBC_2.2.5");
#endif
class AssetManager;

class Game : public GameBase
{
	public:
		Game();
		~Game();

		///
		virtual void GameInit();
		virtual void ModuleInit();
		virtual void DeInit();

		static std::shared_ptr<AssetManager>		GetAssetMgr();

	private:
		static Game* Instance;
		std::shared_ptr<AssetManager>	AssetMgr;
};


#endif // GAME_H
