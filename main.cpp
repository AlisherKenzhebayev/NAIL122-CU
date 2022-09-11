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
	config.resolution.set(1000, 800);
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
	gridTexture_ = nctl::makeUnique<nc::Texture>((nc::fs::dataPath() + "/go-stones/grid.png").data());
	whiteTexture_ = nctl::makeUnique<nc::Texture>((nc::fs::dataPath() + "/go-stones/w.png").data());
	blackTexture_ = nctl::makeUnique<nc::Texture>((nc::fs::dataPath() + "/go-stones/b.png").data());

	// Stretch the game space to a window size
	boardSprite_ = nctl::makeUnique<nc::Sprite>(dummy_.get(), boardTexture_.get());
	
	// Reserved space 200 x 800 for debugging and scoring
	debugText_ = nctl::makeUnique<nc::TextNode>(dummy_.get(), font_.get());
	debugText_->setColor(170, 135, 181, 255);
	debugText_->setScale(0.8f);
	debugText_->setAlignment(nc::TextNode::Alignment::RIGHT);

	gridPool_ = nctl::makeUnique<GridPool>(9 * 9, blackTexture_.get(), whiteTexture_.get());

	rootState_ = GameState(9);
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
	screenString_.format(static_cast<const char *>("Debug Info HERE\nDebug Info HERE\nDebug Info HERE"));
	debugText_->setString(screenString_);
	debugText_->setPosition(nc::theApplication().width() - debugText_->width() * 0.5f, nc::theApplication().height() - debugText_->height() * 0.5f);

	gridPool_->draw();
}


void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	event.x;
	nc::theApplication().quit();
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
}
