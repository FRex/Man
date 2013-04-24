#ifndef EE_MENUSTATE_H_INCLUDED
#define EE_MENUSTATE_H_INCLUDED
#include "State.h"
#include <SFML/Graphics/Text.hpp>
namespace edy{
	namespace state{
		class MenuState : public core::State
		{
		public:
			virtual void run(core::PointerPack& pack);
		private:
			sf::Text mTitle,mPlay,mHigh,mQuit;
		};
	}
}
#endif