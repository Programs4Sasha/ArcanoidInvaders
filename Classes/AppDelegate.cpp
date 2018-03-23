#include "AppDelegate.h"
#include "GraphicsScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching() {
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if (!glview) {
		glview = GLViewImpl::create("Arcanoid VS Invaders");
		glview->setFrameSize(640, 480);
		director->setOpenGLView(glview);
	}

	//auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
	//audio->setEffectsVolume(1.0);

	//audio->preloadEffect("hit1.wav");
	//audio->playEffect("explosion.wav");
//	SimpleAudioEngine::sharedEngine()->preloadEffect("hit.wav");
//	SimpleAudioEngine::sharedEngine()->preloadEffect("score.wav");

	auto scene = GraphicsScene::createScene();
	
	director->runWithScene(scene);
	

	return true;
}

void AppDelegate::applicationDidEnterBackground() {
}

void AppDelegate::applicationWillEnterForeground() {
}