#include <iostream>

#include "gameState.h"
#include "ncine/Vector2.h"
#include "ncine/Rect.h"

namespace gamestate1{
	void ForwardModel::Simulate(const GameStateStatic &s, const GameStateDynamic &d, gc::GameAction p1Action, gc::GameAction p2Action, GameStateDynamic &out)
	{
	    out = d;
	    SimulateInPlace(s, out, p1Action, p2Action, 1);
	}

	void ForwardModel::SimulateInPlace(const GameStateStatic& s, GameStateDynamic& d, gc::GameAction p1Action, gc::GameAction p2Action, int speedUp)
	{
		// Step float disappears, as the A* is unable to forward simulate the frames.
	    // Moving the sticks
	    d.p1Y += (int)p1Action * s.stickSpeed * speedUp;
	    d.p2Y += (int)p2Action * s.stickSpeed * speedUp;

	    // Moving the ball
	    d.ballX += d.ballDirX * s.ballSpeed * speedUp;
	    d.ballY += d.ballDirY * s.ballSpeed * speedUp;

		//removed particles

	    // Checking for ball and sticks collisions
	    // Gets all 3 rectangle collision boxes, calculates their sizes based on centermass.
	    const nc::Rectf ballRect = nc::Rectf::fromCenterAndSize(nc::Vector2f(d.ballX, d.ballY), nc::Vector2f(s.ballSize, s.ballSize));
	    const nc::Rectf blueRect = nc::Rectf::fromCenterAndSize(nc::Vector2f(s.p1X, d.p1Y), nc::Vector2f(s.stickWidth, s.stickHeight));
	    const nc::Rectf redRect = nc::Rectf::fromCenterAndSize(nc::Vector2f(s.p2X, d.p2Y), nc::Vector2f(s.stickWidth, s.stickHeight));

		if (ballRect.x < blueRect.x + blueRect.w &&
	        ballRect.y + ballRect.h >= blueRect.y &&
	        ballRect.y <= blueRect.y + blueRect.h)
	    {
		    d.ballX = blueRect.x + blueRect.w + ballRect.w;
		    d.ballDirX *= -1.0f;
		    d.ballDirY = -1.0f * ((d.p1Y - d.ballY) / blueRect.h);
		}
	    else if (ballRect.x + ballRect.w > redRect.x &&
	             ballRect.y + ballRect.h >= redRect.y &&
	             ballRect.y <= redRect.y + redRect.h)
	    {
		    d.ballX = redRect.x - ballRect.w;
		    d.ballDirX *= -1.0f;
		    d.ballDirY = -1.0f * ((d.p2Y - d.ballY) / redRect.h);
	    }

	    // Ball collision with top or bottom
	    if (ballRect.y + ballRect.h > s.height)
	    {
		    d.ballY = s.height - ballRect.h * 0.5f;
		    d.ballDirY *= -1.0f;
		}
	    else if (ballRect.y < 0)
	    {
		    d.ballY = 0 + ballRect.h * 0.5f;
		    d.ballDirY *= -1.0f;
		}

	    // Scoring
	    if (ballRect.x <= 0)
	    {
		    d.p2Score++;
		    ResetRoundInPlace(s, d);
	    }
	    else if (ballRect.x + ballRect.w > s.width)
	    {
		    d.p1Score++;
		    ResetRoundInPlace(s, d);
	    }
	}

	void ForwardModel::ResetRoundInPlace(const GameStateStatic& s, GameStateDynamic& d)
    {
	    d.p1Y = d.p2Y = s.height * 0.5f;
	    d.ballX = s.width * 0.5f;
	    d.ballY = s.height * 0.5f;
	    d.ballDirX = (d.p1Score > d.p2Score) ? -1.0f : 1.0f;
	    d.ballDirY = 0.0f;
	}
}
