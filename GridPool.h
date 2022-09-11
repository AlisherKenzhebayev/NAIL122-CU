#ifndef GRID_POOL_H
#define GRID_POOL_H

#include "SpritePool.h"
#include "mcts.h"

#include <ncine/Texture.h>
#include <ncine/TimeStamp.h>
#include <ncine/Vector2.h>

namespace nc = ncine;

// The class that is used to display the playing field grid.
class GridPool
{
  public:
	GridPool(unsigned int maxSize, nc::Texture *gridTexB, nc::Texture *gridTexW);

	void reset();
	void draw();
//	void draw(nc::Vector2i cTL, nc::Vector2i cDR);
//	void drawSingle(nc::Vector2i cTL, nc::Vector2i cDR, Coordinate c, Color stone);
	void drawSingle(Coordinate c, Color stone);

  private:
	unsigned int size_;
	int height_;
	int width_;
	SpritePool cells_;
	
	float xMin_, xMax_;
	float yMin_, yMax_;

	nc::Texture *gridTexB_;
	nc::Texture *gridTexW_;
	
	GridPool(const GridPool &) = delete;
	GridPool &operator=(const GridPool &) = delete;
};

#endif
