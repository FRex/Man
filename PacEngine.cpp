#include "PacEngine.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
namespace {
	bool enterable(unsigned char number)
	{
		if (number==edy::pac::PacEngine::Wall)
			return false;
		return true;
	}
	void drawHorizontalLine(sf::RenderTarget& rt,int x1,int x2,int y)
	{
		sf::Vertex arr[4];
		arr[0].color=sf::Color::Blue;
		arr[1].color=sf::Color::Blue;
		arr[2].color=sf::Color::Blue;
		arr[3].color=sf::Color::Blue;
		arr[0].position=sf::Vector2f(16.f*(x1+0.5f),16.f*(y+0.5f)-1.5f);
		arr[1].position=sf::Vector2f(16.f*(x1+0.5f),16.f*(y+0.5f)+1.5f);
		arr[2].position=sf::Vector2f(16.f*(x2+0.5f),16.f*(y+0.5f)+1.5f);
		arr[3].position=sf::Vector2f(16.f*(x2+0.5f),16.f*(y+0.5f)-1.5f);
		rt.draw(arr,4,sf::Quads);
	}
	void drawVerticalLine(sf::RenderTarget& rt,int y1,int y2,int x)
	{
		sf::Vertex arr[4];
		arr[0].color=sf::Color::Blue;
		arr[1].color=sf::Color::Blue;
		arr[2].color=sf::Color::Blue;
		arr[3].color=sf::Color::Blue;
		arr[0].position=sf::Vector2f(16.f*(x+0.5f)-1.5f,16.f*(y1+0.5f));
		arr[1].position=sf::Vector2f(16.f*(x+0.5f)+1.5f,16.f*(y1+0.5f));
		arr[2].position=sf::Vector2f(16.f*(x+0.5f)+1.5f,16.f*(y2+0.5f));
		arr[3].position=sf::Vector2f(16.f*(x+0.5f)-1.5f,16.f*(y2+0.5f));
		rt.draw(arr,4,sf::Quads);
	}
	int fitBetween(int a,int b,int c)
	{
		return std::min(std::max(a,b),c);
	}
	sf::Vector2i getPosFromNode(int x,int y)
	{
		return sf::Vector2i(x*16+8,y*16+8);
	}
	int get2DistanceFromNodes(sf::Vector2i first,sf::Vector2i second)
	{
		return (first.x-second.x)*(first.x-second.x)+(first.y-second.y)*(first.y-second.y);
	}
	edy::pac::PacEntity::eDirection getDirFromVec(sf::Vector2i from)
	{
		if(from.x==1)return edy::pac::PacEntity::Right;
		if(from.x==-1)return edy::pac::PacEntity::Left;
		if(from.y==1)return edy::pac::PacEntity::Down;
		if(from.y==-1)return edy::pac::PacEntity::Up;
		return edy::pac::PacEntity::Right;//eh?
	}
	bool ClydeChase(edy::pac::PacEntity pac,edy::pac::PacEntity cly)
	{
		return get2DistanceFromNodes(pac.position,cly.position)>3*64*256;
	}
	int ghostKillPts(int& number)
	{
		++number;
		switch(number)
		{
		case 1:return 200;
		case 2:return 400;
		case 3:return 800;
		case 4:return 1600;
		}
		return -10000000;//negative to indicate errors
	}
}
namespace edy{
	namespace pac{
		PacEngine::PacEngine():mLives(3),mCherryCountDown(0)
		{

		}
		sf::Vector2f PacEngine::getPosition(int who)
		{
			return guys[who].getFloatPos();
		}
		float PacEngine::getRotation(int who)
		{
			return guys[who].getFloatRotation();
		}
		bool PacEngine::loadMap(const std::string& path)
		{
			mTotalPills=0;
			//tempS
			guys[Pac].position=getPosFromNode(1,1);
			guys[Pac].speed=3;

			guys[0].direction=PacEntity::Right;

			//tempE
			for(int i=1;i<=4;++i)
			{
				guys[i].speed=2;
				guys[i].scared=0;
			}
			std::ifstream file(path.c_str());
			if(!file.is_open()) return false;
			int ghostpos=0;
			for(int y=0;y<31;++y) for(int x=0;x<28;++x)
			{
				if(file.eof()) return false;
				int current;
				file>>current;
				switch(current)
				{
				case Pill:
					++mTotalPills;
					break;
				case GhostSpawn:
					startPos[ghostpos]=getPosFromNode(x,y);
					++ghostpos;
					break;
				}

				mMap[x][y]=current;
			}
			if(ghostpos!=4) return false;

			resetPositions();
			return true;
		}
		void PacEngine::update()
		{
			if(--mCherryCountDown==0)
			{
				mMap[cherryPos.x][cherryPos.y]=Empty;
			}
			if(mCherryCountDown<=-60*10)
			{
				cherryPos=sf::Vector2i(rand()%28,rand()%31);
				if(mMap[cherryPos.x][cherryPos.y]==Empty)
				{
					mCherryCountDown=10*60;
					mMap[cherryPos.x][cherryPos.y]=Cherry;
				}
			}
			updatePac();
			for(int i=1;i<=4;++i)updateGhost(i);
			checkCollisions();
		}
		bool PacEngine::getEvent(PacEvent& event)
		{
			if(!mEventsList.empty())
			{
				event=mEventsList.front();
				mEventsList.pop_front();
				return true;
			}
			else
			{
				return false;
			}
		}
		void PacEngine::setPacDirection(PacEntity::eDirection direction)
		{
			guys[Pac].nextMove=direction;
		}
		void PacEngine::makeWallsMap(sf::RenderTarget& target)
		{
			sf::Vertex arr[4];
			arr[0].color=sf::Color::Blue;
			arr[1].color=sf::Color::Blue;
			arr[2].color=sf::Color::Blue;
			arr[3].color=sf::Color::Blue;
			for(int x=0;x<27;++x)
			{
				if((mMap[x][0]==Wall)&&(mMap[x+1][0]==Wall)) drawHorizontalLine(target,x,x+1,0);
				if((mMap[x][30]==Wall)&&(mMap[x+1][30]==Wall)) drawHorizontalLine(target,x,x+1,30);
			}
			for(int y=0;y<30;++y)
			{
				if((mMap[0][y]==Wall)&&(mMap[0][y+1]==Wall)) drawVerticalLine(target,y,y+1,0);
				if((mMap[27][y]==Wall)&&(mMap[27][y+1]==Wall)) drawVerticalLine(target,y,y+1,27);
			}
			for(int x=1;x<27;++x) for(int y=1;y<30;++y)
			{
				if(mMap[x][y]==Wall)
				{
					if(mMap[x-1][y]==Wall) drawHorizontalLine(target,x-1,x,y);
					if(mMap[x+1][y]==Wall) drawHorizontalLine(target,x+1,x,y);
					if(mMap[x][y-1]==Wall) drawVerticalLine(target,y-1,y,x);
					if(mMap[x][y+1]==Wall) drawVerticalLine(target,y+1,y,x);
				}
			}//for x for y
		}
		void PacEngine::updatePac()
		{
			//std::cout<<guys[0].position.x<<" "<<guys[0].position.y<<"\n";
			guys[0].position.x+=16*28;//as below, to avoid problems with negative numbers modulos
			guys[0].position.x%=16*28;//for tunnel purposes
			sf::Vector2i update=guys[Pac].getVectorFromDirection();
			int i;
			for(i=0;i<guys[Pac].speed;++i)
			{
				guys[Pac].position+=update;
				if(guys[Pac].isAtNode())break;
			}
			if(guys[Pac].isAtNode())
			{
				sf::Vector2i t=guys[Pac].getNode();
				unsigned char & tmp=mMap[t.x][t.y];
				switch(tmp)
				{
				case Cherry:
					tmp=Empty;
					mEventsList.push_back(PacEvent(EventType::ScoreChange,100));
					break;
				case Pill:
					tmp=Empty;
					mEventsList.push_back(PacEvent(EventType::ScoreChange,10));
					checkPills();
					break;
				case Booster:
					tmp=Empty;
					mEventsList.push_back(PacEvent(EventType::ScoreChange,50));
					mGhostKillStreak=0;//reset kill streak
					for(int i=Blinky;i<=Clyde;++i)
					{
						if(getScareStatus(i)!=Dead)guys[i].scared=7*60;//7 secs, do not affect dead ghosts that try to go to respawn
					}
					break;
				}
				if(guys[Pac].nextMove!=PacEntity::None&&
					enterable(fetchTileAt(guys[Pac].getNode(),guys[Pac].getVectorFromNext())))
				{
					guys[Pac].direction=guys[Pac].nextMove;
					guys[Pac].nextMove=PacEntity::None;
				}
				if(!enterable(fetchTileAt(guys[Pac].getNode(),guys[Pac].getVectorFromDirection())))
				{
					guys[Pac].direction=PacEntity::None;
				}
				update=guys[Pac].getVectorFromDirection();
			}//pac @ node

			for(;i<guys[Pac].speed;++i)
			{
				guys[Pac].position+=update;
			}
		}
		void PacEngine::checkCollisions()
		{
			int doit=0;
			for(int i=1;i<=4;++i)
			{
				if(guys[Pac].getNode()==guys[i].getNode())
				{
					if(guys[i].scared==0)doit=true;
					if(guys[i].scared>0)
					{
						mEventsList.push_back(PacEvent(EventType::ScoreChange,ghostKillPts(mGhostKillStreak)));
						guys[i].scared=-1;
					}
				}
			}
			if(!doit) return;
			if(--mLives)
			{
				mEventsList.push_back(PacEvent(EventType::PacDied,mLives));
			}
			else
			{
				mEventsList.push_back(PacEvent(EventType::GameOver,0));
			}
			resetPositions();
		}
		void PacEngine::resetPositions()
		{
			guys[Blinky].position=startPos[0];
			guys[Inky].position=startPos[1];
			guys[Pinky].position=startPos[2];
			guys[Clyde].position=startPos[3];
			guys[Pac].position=getPosFromNode(1,1);
			guys[Pac].direction=PacEntity::None;
			guys[Pac].nextMove=PacEntity::None;
			for(int i=1;i<=4;++i)
			{
				guys[i].scared=0;
			}
		}
		int PacEngine::fetchTileAt(sf::Vector2i pos,sf::Vector2i off)
		{
			int x=pos.x+off.x;
			int y=pos.y+off.y;
			//tunnel special case
			if(y==fitBetween(0,y,30)&&(x==-1||x==28)&&mMap[fitBetween(0,x,27)][y]==Tunnel) 
				return Empty;
			//do not drive out of boundaries
			if(y!=fitBetween(0,y,30)||x!=fitBetween(0,x,27)) return Wall;
			return mMap[x][y];
		}
		void PacEngine::checkPills()
		{
			int tmp=0;
			for(int y=0;y<31;++y) for(int x=0;x<28;++x)
			{
				if(mMap[x][y]==Pill)++tmp;
			}
			if(tmp==0) mEventsList.push_back(PacEvent(EventType::PillsExhausted,0));
			if(tmp<0.3f*mTotalPills)guys[Blinky].speed=3;
		}
		void PacEngine::updateGhost(int who)
		{
			//std::cout<<who<<" "<<guys[who].scared<<std::endl;
			if(guys[who].scared>0)
			{
				--guys[who].scared;//go to 0, normal cooldown procedure
			}
			sf::Vector2i update=guys[who].getVectorFromDirection();
			if(rand()%3&&fetchTileAt(guys[0].getNode(),sf::Vector2i())==Tunnel)
				return;//30% chance to be paralyzed if pacman is in tunnel tile
			for(int i=0;i<guys[who].speed;++i)
			{
				guys[who].position+=update;
				if(guys[who].isAtNode())break;
			}
			if(guys[who].isAtNode())
			{
				//see if we respawned:
				if(guys[who].scared==-1)
				{
					if(guys[who].getNode()==startPos[who-1]/16) guys[who].scared=0;// /16 to get node number from pixel pos
				}
				//chose next move:
				guys[who].target=getTarg(who);
				guys[who].direction=getNextMove(guys[who]);
				//std::cout<<who<<"\n";
			}//ghost @ node
		}
		sf::Vector2i PacEngine::getTarg(int who)
		{
			if(guys[who].scared!=0)//scare mode
			{

				switch(who)
				{
				case Blinky:
					return guys[Blinky].scared>0?sf::Vector2i(0,0):startPos[0]/16;// /16 to get node
				case Inky:
					return guys[Inky].scared>0?sf::Vector2i(28,0):startPos[1]/16;
				case Pinky:
					return guys[Pinky].scared>0?sf::Vector2i(0,31):startPos[2]/16;
				case Clyde:
					return guys[Clyde].scared>0?sf::Vector2i(28,31):startPos[3]/16;//more = escape, less = respawn
				}
			}
			if(who==Inky)
			{
				switch(rand()%3)
				{
				case 0:who=Blinky;break;
				case 1:who=Pinky;break;
				case 2:who=Clyde;break;
				}
			}
			switch(who)
			{
			case Blinky:
				return guys[Pac].getNode();
			case Pinky:
				return guys[Pac].getNode()+2*guys[Pac].getVectorFromDirection();
			case Clyde:
				return ClydeChase(guys[Pac],guys[Clyde])?guys[Pac].getNode():sf::Vector2i(28,31);
			}
			return sf::Vector2i();//default, shouldnt ever happen, silences vc++ unreasonable complains
		}
		PacEntity::eDirection PacEngine::getNextMove(PacEntity& ent)
		{
			std::vector<sf::Vector2i> possibleNodes;
			sf::Vector2i tmp=ent.getVectorFromDirection();

			//if(tmp==sf::Vector2i())return PacEntity::Left;

			if(enterable(fetchTileAt(ent.getNode(),sf::Vector2i(1,0))))
				possibleNodes.push_back(ent.getNode()+sf::Vector2i(1,0));
			if(enterable(fetchTileAt(ent.getNode(),sf::Vector2i(-1,0))))
				possibleNodes.push_back(ent.getNode()+sf::Vector2i(-1,0));
			if(enterable(fetchTileAt(ent.getNode(),sf::Vector2i(0,1))))
				possibleNodes.push_back(ent.getNode()+sf::Vector2i(0,1));
			if(enterable(fetchTileAt(ent.getNode(),sf::Vector2i(0,-1))))
				possibleNodes.push_back(ent.getNode()+sf::Vector2i(0,-1));

			for(std::vector<sf::Vector2i>::iterator it=possibleNodes.begin();it!=possibleNodes.end();++it)
			{
				if(*it==(ent.getNode()-ent.getVectorFromDirection()))
				{
					possibleNodes.erase(it);
					break;
				}
			}

			int a=100000,b=-1;
			for(int i=0;i<possibleNodes.size();++i)
			{
				if(a>get2DistanceFromNodes(ent.target,possibleNodes[i]))
				{
					a=get2DistanceFromNodes(ent.target,possibleNodes[i]);
					b=i;
				}
			}
			if(b!=-1)
			{
				return getDirFromVec(possibleNodes[b]-ent.getNode());
			}
			else
			{
				return getDirFromVec(-1*ent.getVectorFromDirection());
			}
		}
		PacEngine::ScareStatus PacEngine::getScareStatus(int who)const
		{
			if(guys[who].scared>2*60)return Scared;
			if(guys[who].scared>0)return Blinking;
			if(guys[who].scared==-1)return Dead;
			return Brave;
		}
	}
}