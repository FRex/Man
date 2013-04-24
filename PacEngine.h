#ifndef EE_PACENGINE_H_INCLUDED
#define EE_PACENGINE_H_INCLUDED
#include "PacEntity.h"
#include "PacEvent.h"
#include <SFML/System/Vector2.hpp>
#include <string>
#include <deque>
namespace sf{
	class RenderTarget;
}
namespace edy{
	namespace pac{
		enum eEntsID{Pac=0,Blinky=1,Inky=2,Pinky=3,Clyde=4};
		class PacEngine
		{
		public:
			enum TileType
			{
				Cherry=0,
				Pill=1,
				Booster=2,
				Wall=3,
				Tunnel=7,
				Empty=8,
				GhostSpawn=9
			};
			enum ScareStatus{Brave,Scared,Blinking,Dead};
			PacEngine();
			sf::Vector2f getPosition(int who);
			float getRotation(int who);
			bool loadMap(const std::string& path);
			void update();
			bool getEvent(PacEvent& event);
			void setPacDirection(PacEntity::eDirection direction);
			void makeWallsMap(sf::RenderTarget& target);
			//16x16 tiles, top 3 and bottom 2 rows go for texts,array in interface=ugly
			unsigned char mMap[28][31];
			ScareStatus getScareStatus(int who)const;
		private:
			//methods:
			void updatePac();
			void checkCollisions();
			void resetPositions();
			void checkPills();
			void updateGhost(int who);
			int fetchTileAt(sf::Vector2i pos,sf::Vector2i off);
			PacEntity::eDirection getNextMove(sf::Vector2f pos,sf::Vector2i targ,PacEntity::eDirection cur);
			sf::Vector2i getTarg(int who);
			//vars:
			sf::Vector2i startPos[4],cherryPos;
			int mLives,mTotalPills,mCherryCountDown,mGhostKillStreak;
			PacEntity guys[5];
			std::deque<PacEvent> mEventsList;
			PacEntity::eDirection getNextMove(PacEntity& ent);
			

		};
	}
}
#endif