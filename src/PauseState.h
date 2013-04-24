#ifndef EE_PAUSESTATE_H_INCLUDED
#define EE_PAUSESTATE_H_INCLUDED
#include "State.h"
#include "StateManager.h"
#include <string>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
namespace edy{
	namespace state{
		class PauseState : public core::State
		{
		public:
			virtual void run(core::PointerPack& pack)
			{
				sf::Text text("Paused, press p to resume",*pack.Font);
				sf::Event eve;
				while(1)
				{
					while(pack.Window->pollEvent(eve))
					{
						if(eve.type==sf::Event::KeyPressed&&eve.key.code==sf::Keyboard::P)
						{
							pack.Manager->popTop(1);
							return;
						}
					}
					pack.Window->clear();
					pack.Window->draw(text);
					pack.Window->display();
				}
			}
		};
	}
}
#endif