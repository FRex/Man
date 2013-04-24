#ifndef EE_SPECIALSTATE_H_INCLUDED
#define EE_SPECIALSTATE_H_INCLUDED
#include "State.h"
#include "StateManager.h"
#include <string>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
namespace edy{
	namespace state{
		class ErrState : public core::State
		{
		public:
			ErrState(const std::string& errortext):mTex(errortext){}
			virtual void run(core::PointerPack& pack)
			{
				sf::Text text(mTex,*pack.Font);
				sf::Event eve;
				while(1)
				{
					while(pack.Window->pollEvent(eve))
					{
						if(eve.type==sf::Event::KeyPressed)
						{
							pack.Manager->leaveBottom(1);
							return;
						}
						if(eve.type==sf::Event::Closed)
						{
							pack.Manager->popTop(100);
							return;
						}
					}
					pack.Window->clear();
					pack.Window->draw(text);
					pack.Window->display();
				}
			}
		private:
			std::string mTex;
		};
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
		class QuitState : public core::State
		{
		public:
			virtual void run(core::PointerPack& pack)
			{
				sf::Text text("Quit? y/n",*pack.Font);
				sf::Event eve;
				while(1)
				{
					while(pack.Window->pollEvent(eve))
					{
						if(eve.type==sf::Event::KeyPressed&&eve.key.code==sf::Keyboard::Y)
						{
							pack.Manager->leaveBottom(1);
							return;
						}
						if(eve.type==sf::Event::KeyPressed&&eve.key.code==sf::Keyboard::N)
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