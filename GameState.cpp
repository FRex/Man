#include "GameState.h"
#include "EEToString.h"
#include <SFML/Window/Event.hpp>
#include "StateManager.h"
#include "ScoreState.h"
#include "ErrState.h"
#include "PauseState.h"
#include "QuitState.h"
namespace edy{
	namespace state{
		GameState::GameState():currentMap(0),mScore(0),mPack(0x0)
		{
			pacframe=0;
			mAtlas.loadFromFile("testpac.tga");
			//mAtlas.setSmooth(true);
			for(int i=0;i<=4;++i)
			{
				guys[i].setTexture(mAtlas);
				guys[i].setOrigin(12.5f,12.5f);
				guys[i].setTextureRect(sf::IntRect(25,26,25,25));
			}
			leftEye.setTexture(mAtlas);
			rightEye.setTexture(mAtlas);
			leftEye.setTextureRect(sf::IntRect(0,26,4,4));
			rightEye.setTextureRect(sf::IntRect(0,26,4,4));
			leftEye.setOrigin(2.f,2.f);
			rightEye.setOrigin(2.f,2.f);
			guys[0].setTextureRect(sf::IntRect(0,0,25,25));
			guys[1].setColor(sf::Color::Red);
			guys[2].setColor(sf::Color::Magenta);
			guys[3].setColor(sf::Color::Yellow);
			guys[4].setColor(sf::Color::Cyan);
			mMapNames[0]="Level1.txt";
			mMapNames[1]="Level2.txt";
			mMapNames[2]="Level3.txt";


		}
		bool GameState::loadMap()
		{
			if(currentMap==3)
			{
				mPack->Manager->pushTop(new ScoreState(mScore));
				return false;
			}
			if(!mEngine.loadMap(mMapNames[currentMap++]))
			{
				mPack->Manager->pushTop(new ErrState("Failed Map Loading"));
				return false;
			}
			return true;
		}
		void GameState::run(core::PointerPack& pack)
		{
			if(!mPack)//first time run() call inits:
			{
				mPack=&pack;
				mapOk=loadMap();
				lives.setFont(*pack.Font);
				score.setFont(*pack.Font);
				lives.setString("3UP");
				score.setString("Score:0");
				lives.setCharacterSize(32);
				score.setCharacterSize(32);
				lives.setPosition(0.f,500.f);
				score.setPosition(80.f,500.f);
			}
			sf::Time delta(sf::seconds(1.f/60.f)),stock;
			sf::Clock clock;
			sf::Event seve;
			pac::PacEvent peve;
			mAnimClock.restart();
			while(mapOk)
			{
				while(pack.Window->pollEvent(seve))
				{
					if(seve.type==sf::Event::Closed)
					{
						pack.Manager->popTop(100);
						return;
					}
					if(seve.type==sf::Event::KeyPressed)
					{
						switch(seve.key.code)
						{
						case sf::Keyboard::P:
							pack.Manager->pushTop(new PauseState());
							return;
							break;
						case sf::Keyboard::Escape:
							pack.Manager->pushTop(new QuitState());
							return;
							break;
						case sf::Keyboard::W:
						case sf::Keyboard::Up:
							mEngine.setPacDirection(pac::PacEntity::Up);
							break;
						case sf::Keyboard::S:
						case sf::Keyboard::Down:
							mEngine.setPacDirection(pac::PacEntity::Down);
							break;
						case sf::Keyboard::A:
						case sf::Keyboard::Left:
							mEngine.setPacDirection(pac::PacEntity::Left);
							break;
						case sf::Keyboard::D:
						case sf::Keyboard::Right:
							mEngine.setPacDirection(pac::PacEntity::Right);
							break;
						}
					}
				}//seve
				while(mEngine.getEvent(peve))
				{
					switch(peve.type)
					{
					case pac::EventType::GameOver:
						pack.Manager->pushTop(new ScoreState(mScore));
						return;
						break;
					case pac::EventType::PacDied:
						lives.setString(sup::toString(peve.data)+"UP");
						break;
					case pac::EventType::PillsExhausted:
						mapOk=loadMap();
						break;
					case pac::EventType::ScoreChange:
						mScore+=peve.data;
						score.setString("Score:"+sup::toString(mScore));
						break;
					}
				}//peve
				pack.Window->clear();
				stock+=clock.restart();

				while(stock>=delta)
				{
					stock-=delta;
					mEngine.update();
				}
				drawAll();

				pack.Window->draw(score);
				pack.Window->draw(lives);

				pack.Window->display();
			}
		}
		void GameState::drawCherry(int x,int y)
		{
			sf::Sprite spr(mAtlas);
			spr.setPosition(x*16.f,y*16.f);
			spr.setTextureRect(sf::IntRect(75,26,25,25));
			mPack->Window->draw(spr);
		}
		void GameState::drawBooster(int x,int y)
		{
			sf::Vertex arr[4];
			arr[0].position=sf::Vector2f(16*x+1.f,y*16+1.f);
			arr[1].position=sf::Vector2f(16*x+1.f,y*16+10.f);
			arr[2].position=sf::Vector2f(16*x+10.f,y*16+10.f);
			arr[3].position=sf::Vector2f(16*x+10.f,y*16+1.f);
			mPack->Window->draw(arr,4,sf::Quads);
		}
		void GameState::drawPill(int x,int y)
		{
			sf::Vertex arr[4];
			arr[0].position=sf::Vector2f(16*x+3.f,y*16+3.f);
			arr[1].position=sf::Vector2f(16*x+3.f,y*16+7.f);
			arr[2].position=sf::Vector2f(16*x+7.f,y*16+7.f);
			arr[3].position=sf::Vector2f(16*x+7.f,y*16+3.f);
			mPack->Window->draw(arr,4,sf::Quads);
		}
		void GameState::drawGhosts()
		{
			if(mAnimClock.getElapsedTime()>sf::seconds(0.2f))
			{
				mAnimClock.restart();
				++pacframe;
				pacframe%=4;
				guys[0].setTextureRect(sf::IntRect(25*(3-pacframe),0,25,25));

				//scared/nonscared color switches,
				guys[1].setColor(mEngine.getScareStatus(1)==pac::PacEngine::Brave?sf::Color::Red:sf::Color::Blue);
				guys[2].setColor(mEngine.getScareStatus(2)==pac::PacEngine::Brave?sf::Color::Magenta:sf::Color::Blue);
				guys[3].setColor(mEngine.getScareStatus(3)==pac::PacEngine::Brave?sf::Color::Yellow:sf::Color::Blue);
				guys[4].setColor(mEngine.getScareStatus(4)==pac::PacEngine::Brave?sf::Color::Cyan:sf::Color::Blue);
				for(int i=1;i<=4;++i)
				{
					guys[i].setTextureRect(sf::IntRect(25+(pacframe%2)*25,26,25,25));
					if(mEngine.getScareStatus(i)==pac::PacEngine::Blinking)//blinking color switches
					{
						guys[i].setColor(pacframe%2?sf::Color::White:sf::Color::Blue);
					}
				}
			}
		}
		void GameState::drawAll()
		{
			drawGhosts();
			mEngine.makeWallsMap(*mPack->Window);
			for(int i=0;i<=4;++i)
			{
				guys[i].setPosition(mEngine.getPosition(i));
			}
			guys[0].setRotation(mEngine.getRotation(0));
			for(int x=0;x<28;++x)for(int y=0;y<31;++y)
			{
				switch(mEngine.mMap[x][y])
				{
				case pac::PacEngine::Cherry:
					drawCherry(x,y);
					break;
				case pac::PacEngine::Booster:
					drawBooster(x,y);
					break;
				case pac::PacEngine::Pill:
					drawPill(x,y);
					break;
				}
			}
			mPack->Window->draw(guys[0]);
			//fake pacmans on the right and left of map for tunnel smooth passing
			guys[0].move(16.f*28.f,0.f);
			mPack->Window->draw(guys[0]);
			guys[0].move(-2.f*16.f*28.f,0.f);
			mPack->Window->draw(guys[0]);
			for(int i=1;i<=4;++i)
			{
				if(mEngine.getScareStatus(i)!=pac::PacEngine::Dead) mPack->Window->draw(guys[i]);
				leftEye.setPosition(guys[i].getPosition());
				rightEye.setPosition(guys[i].getPosition());
				leftEye.move(-4.f,-3.f);
				rightEye.move(4.f,-3.f);
				if(mEngine.getScareStatus(i)==pac::PacEngine::Brave)
				{
					leftEye.setRotation(mEngine.getRotation(i));
					rightEye.setRotation(mEngine.getRotation(i));
				}
				else
				{
					leftEye.setRotation(90);//90 is down in 2d coord system with reversed y axis
					rightEye.setRotation(90);
				}
				mPack->Window->draw(leftEye);
				mPack->Window->draw(rightEye);
			}
		}
	}
}