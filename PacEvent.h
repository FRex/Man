#ifndef EE_PACEVENT_H_INCLUDED
#define EE_PACEVENT_H_INCLUDED
namespace edy{
	namespace pac{
		namespace EventType{
			enum eType
			{
				PacDied,ScoreChange,PillsExhausted,GameOver
			};
		}
		class PacEvent
		{
		public:
			PacEvent(){}
			PacEvent(EventType::eType gtype,int gdata):type(gtype),data(gdata){}
			EventType::eType type;
			int data;
		};
	}
}
#endif