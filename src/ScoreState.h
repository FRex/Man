#ifndef EE_SCORESTATE_H_INCLUDED
#define EE_SCORESTATE_H_INCLUDED
#include "State.h"
#include <SFML/Graphics/Text.hpp>
namespace edy{
	namespace state{
		class ScoreState : public core::State
		{
		public:
			ScoreState(int score);
			virtual ~ScoreState();
			virtual void run(core::PointerPack& pack);
		private:
			std::vector<sf::Text> mLines;
			int mWritingTo,mScore;
			std::string mWritingLine;

		};
	}
}
#endif