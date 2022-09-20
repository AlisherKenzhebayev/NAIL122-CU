#ifndef GRID_POOL_H
#define GRID_POOL_H

#include "SpritePool.h"
#include "game.h"

#include <ncine/Texture.h>
#include <ncine/TimeStamp.h>
#include <ncine/Vector2.h>

namespace nc = ncine;

// The class that is used to display the playing field grid.
class GridPool
{
  public:
	GridPool(unsigned int maxSize, nc::Texture *gridTexE, nc::Texture *gridTexB, nc::Texture *gridTexW, nc::Vector2f offset, nc::Vector2f size);

	void reset();
	void draw();
	void drawBoard(std::vector<Color> board);
	void drawSingle(Coordinate c, Color stone);

  private:
	unsigned int size_;
	int rows_;
	int columns_;
	SpritePool cells_;
	nc::Vector2f offset_;
	
	float height_, width_;
	float cellSideVer_, cellSideHor_;

	nc::Texture *gridTexE_;
	nc::Texture *gridTexB_;
	nc::Texture *gridTexW_;
	
	GridPool(const GridPool &) = delete;
	GridPool &operator=(const GridPool &) = delete;
};

#endif
