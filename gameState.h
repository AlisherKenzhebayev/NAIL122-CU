#ifndef GAMESTATE_HEADER
#define GAMESTATE_HEADER

#include <ncine/Vector2.h>
#include <nctl/String.h>
#include <nctl/UniquePtr.h>

namespace gamecommon {

	enum class GameAction
	{
		UP = 1,
		NONE = 0,
		DOWN = -1,
	};
}

namespace nc = ncine;
namespace gc = gamecommon;

namespace gamestate1 {
	class ForwardModel; //responsible for advancing the state
	class GameStateStatic;
    class GameStateDynamic;

	class ForwardModel
	{
	  public:
		void Simulate(const GameStateStatic &s, const GameStateDynamic &d, gc::GameAction p1Action, gc::GameAction p2Action, GameStateDynamic &out);

		void SimulateInPlace(const GameStateStatic &s, GameStateDynamic &d, gc::GameAction p1Action, gc::GameAction p2Action, int speedUp);

		void ResetRoundInPlace(const GameStateStatic &s, GameStateDynamic &d);
	};

	class GameStateStatic
	{
	  public:
	    float width;	// Application width
	    float height;	// Application height
	    float ballSpeed;
	    float stickSpeed;
	    float stickHeight;
	    float stickWidth;
	    float ballSize;
	    float p1X;
	    float p2X;
	};

	class GameStateDynamic
    {
	  public:
	    float p1Y; //bluestick, to the left
	    float p2Y; //redstick, to the right
	    float ballX, ballY;
	    float ballDirX, ballDirY;
	    int p1Score, p2Score;

		GameStateDynamic() {
		    p1Y = 0;
		    p2Y = 0;
		    ballX = 0;
		    ballY = 0;
		    ballDirX = 0;
		    ballDirY = 0;
		    p2Score = 0;
		    p1Score = 0;
		}

		GameStateDynamic &operator=(GameStateDynamic const &copy) {	
		    p1Y = copy.p1Y;
		    p2Y = copy.p2Y;
		    ballX = copy.ballX;
		    ballY = copy.ballY;
		    ballDirX = copy.ballDirX;
		    ballDirY = copy.ballDirY;
			p1Score = copy.p1Score;
		    p2Score = copy.p2Score;
		    return *this;
		}
	};
}

#endif
