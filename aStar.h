#ifndef AI_HEADER
#define AI_HEADER

#include <ncine/Vector2.h>
#include <nctl/String.h>
#include <nctl/UniquePtr.h>
#include "gameState.h"

using namespace gamestate1;

namespace ai
{
	class AStar;

	class AStar
    {
	  public:
	    gc::GameAction OpponentThink(ForwardModel &m, GameStateStatic &s, GameStateDynamic &d);
	    gc::GameAction PlayerThink(ForwardModel &m, GameStateStatic &s, GameStateDynamic &d);
	    gc::GameAction SimplifiedOpponent(GameStateStatic &s, GameStateDynamic &d);
	    gc::GameAction SimplifiedPlayer(GameStateStatic &s, GameStateDynamic &d);
	    float AStar::EstimateOpponentBallDistance(GameStateStatic &s, float p1Y, float p2Y,
	                                                       float ballX, float ballY,
	                                                       float ballDirX, float ballDirY);
	};
}

#endif
