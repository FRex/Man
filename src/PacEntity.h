#ifndef EE_PACENTITY_H_INCLUDED
#define EE_PACENTITY_H_INCLUDED
#include <SFML/System/Vector2.hpp>
namespace edy{
	namespace pac{
		class PacEntity
		{
		public:
			enum eDirection{Up,Down,Left,Right,None};
			int speed;
			int scared;
			sf::Vector2i position;
			sf::Vector2i target;
			eDirection direction;
			eDirection nextMove;
			sf::Vector2f getFloatPos(){return sf::Vector2f(position);}
			float getFloatRotation()
			{
				switch(direction)
				{
				default:
				case Right:return 0.f;
				case Left:return 180.f;
				case Up:return 270.f;
				case Down:return 90.f;
				}
			}
			bool isAtNode()
			{
				return ((((position.x-8)%16)==0)&&(((position.y-8)%16)==0));
			}
			sf::Vector2i getVectorFromDirection()
			{
				switch(direction)
				{
				case Left:return sf::Vector2i(-1,0);
				case Right:return sf::Vector2i(1,0);
				case Up:return sf::Vector2i(0,-1);
				case Down:return sf::Vector2i(0,1);
				}
				return sf::Vector2i(0,0);
			}
			sf::Vector2i getVectorFromNext()
			{
				switch(nextMove)
				{
				case Left:return sf::Vector2i(-1,0);
				case Right:return sf::Vector2i(1,0);
				case Up:return sf::Vector2i(0,-1);
				case Down:return sf::Vector2i(0,1);
				}
				return sf::Vector2i(0,0);
			}
			sf::Vector2i getNode()
			{
				return position/16;
			}
		};
	}
}
#endif