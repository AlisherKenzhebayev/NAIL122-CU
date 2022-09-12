#include "main.h"
#include "mcts.h"

#include <chrono>
#include <ncine/Application.h>
#include <ncine/AppConfiguration.h>
#include <ncine/SceneNode.h>
#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <ncine/TextNode.h>
#include <ncine/IGfxDevice.h>
#include <ncine/FileSystem.h>
#include <ncine/DrawableNode.h>

const char *FontTextureFile = "DroidSans32_256.png";
const int boardWidth = 9;
const float gameWidth_ = 800.0f;
const float gameHeight_ = 800.0f;
const int gameOffsetH_ = 0;
const int gameOffsetV_ = 0;
const int windowWidth_ = 1200;
const int windowHeight_ = 800;

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
#if defined(__ANDROID__)
	config.dataPath() = "asset::";
#elif defined(__EMSCRIPTEN__)
	config.dataPath() = "/";
#else
	#ifdef NCPROJECT_DEFAULT_DATA_DIR
	config.dataPath() = NCPROJECT_DEFAULT_DATA_DIR;
	#else
	config.dataPath() = "data/";
	#endif
#endif

	config.windowTitle = "ncPong";
	config.windowIconFilename = "icon48.png";
	// Set the window resolution
	config.resolution.set(windowWidth_, windowHeight_);
}


void MyEventHandler::onInit()
{
	nc::SceneNode &rootNode = nc::theApplication().rootNode();
	dummy_ = nctl::makeUnique<nc::SceneNode>(&rootNode);

	font_ = nctl::makeUnique<nc::Font>((nc::fs::dataPath() + "DroidSans32_256.fnt").data(),
	                                   (nc::fs::dataPath() + FontTextureFile).data());

	// Init textures, sprites to be used/reused later
	boardTexture_ = nctl::makeUnique<nc::Texture>((nc::fs::dataPath() + "goTable.png").data());
	emptyTexture_ = nctl::makeUnique<nc::Texture>((nc::fs::dataPath() + "/go-stones/e.png").data());
	whiteTexture_ = nctl::makeUnique<nc::Texture>((nc::fs::dataPath() + "/go-stones/w.png").data());
	blackTexture_ = nctl::makeUnique<nc::Texture>((nc::fs::dataPath() + "/go-stones/b.png").data());
	gridTexture_ = nctl::makeUnique<nc::Texture>((nc::fs::dataPath() + "/go-stones/grid.png").data());

	// Stretch the game space to a window size
	boardSprite_ = nctl::makeUnique<nc::Sprite>(dummy_.get(), boardTexture_.get());
	
	// Reserved space 200 x 800 for debugging and scoring
	debugText_ = nctl::makeUnique<nc::TextNode>(dummy_.get(), font_.get());
	debugText_->setColor(170, 135, 181, 255);
	debugText_->setScale(0.8f);
	debugText_->setAlignment(nc::TextNode::Alignment::RIGHT);

	// TODO: Backlog- A bit of a hack with drawing grids, to make writing it a bit faster, needs rewriting?
	gridPool_ = nctl::makeUnique<GridPool>(boardWidth * boardWidth, gridTexture_.get(), gridTexture_.get(), gridTexture_.get(),
		nc::Vector2f((float)gameOffsetH_, (float)gameOffsetV_), nc::Vector2f(gameWidth_, gameHeight_));
	stonePool_ = nctl::makeUnique<GridPool>(boardWidth * boardWidth, emptyTexture_.get(), blackTexture_.get(), whiteTexture_.get(),
		nc::Vector2f((float)gameOffsetH_, (float)gameOffsetV_), nc::Vector2f(gameWidth_, gameHeight_));

	rootState_ = GameState(boardWidth);
	gameStatus_ = GameStatus(rootState_);
	gameStatus_.ResetGame();
}

void MyEventHandler::onFrameStart()
{
	const int width = nc::theApplication().widthInt();
	const int height = nc::theApplication().heightInt();

	// The update loop

	// Update placement of standard things, like scores, sprites, etc.
	boardSprite_->setSize(height, height);
	boardSprite_->setPosition(height / 2.0, height / 2.0);

	screenString_.clear();
	screenString_.formatAppend(static_cast<const char *>("M_Coord: x-%i y-%i \n"), mCoordWindow_.x, mCoordWindow_.y);
	Coordinate c = ConvertWinToGamespace(mCoordWindow_);
	screenString_.formatAppend(static_cast<const char *>("M_Coord: x-%i y-%i \n"), c.x, c.y);
	debugText_->setString(screenString_);
	debugText_->setPosition(nc::theApplication().width() - debugText_->width() * 0.5f, nc::theApplication().height() - debugText_->height() * 0.5f);

	// First draw the grid, then the stones using the information given from the game state
	gridPool_->draw();
	stonePool_->draw();
}

bool MyEventHandler::isMoveValidBounds(int x, int y) {
	if (x >= 0 && x <= 800)
	{
		if (y >= 0 && y <= 800)
		{
			return true;
		}	
	}

	return false;
}

Coordinate MyEventHandler::ConvertWinToGamespace(nc::Vector2i w)
{
	Coordinate retVal = Coordinate(0, 0);

	retVal.x = boardWidth * (w.x - gameOffsetH_) / gameWidth_;
	retVal.y= boardWidth * (w.y - gameOffsetV_) / gameHeight_;

	return retVal;
}

void MyEventHandler::onMouseMoved(const nc::MouseState &state) {
	mCoordWindow_ = nc::Vector2i(state.x, state.y);
}

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	// Check if it is within the play area for a click
	if (isMoveValidBounds(mCoordWindow_.x, mCoordWindow_.y)) {
		// Try applying a move if the move satisfies every other rule
		Coordinate c = ConvertWinToGamespace(mCoordWindow_);
		if (gameStatus_.CurrentState().isActionValid(c, gameStatus_.CurrentTurn())) {
			gameStatus_.PlayTurn(c);
		}
	}
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
}
