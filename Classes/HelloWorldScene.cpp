#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"

#include <cmath>

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...


    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("XiangQi.png");

    // position the sprite on the center of the screen
	sprite->setContentSize(sprite->getContentSize() * visibleSize.height / 320);
    sprite->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - sprite->getContentSize().height/2));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    
	// add a label shows "Hello World"
	// create and initialize a label
	
	label = Label::createWithTTF("Press To Continue", "fonts/BRLNSB.TTF", visibleSize.height/15);

	// position the label on the center of the screen
	label->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - sprite->getContentSize().height - label->getContentSize().height/2));
	auto t_now = time(0);
	auto lt_now = localtime(&t_now);

	// add the label as a child to this layer
	this->addChild(label, 1);

	auto _touchListener = EventListenerTouchOneByOne::create();
	_touchListener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
	_touchListener->onTouchMoved = [](Touch* touch, Event* event) {};
	_touchListener->onTouchCancelled = [](Touch* touch, Event* event) {};
	_touchListener->onTouchEnded = [](Touch* touch, Event* event) {
		auto gameScene = GameScene::create();
		Director::getInstance()->replaceScene(TransitionFade::create(0.5, gameScene));
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);

	this->scheduleUpdate();

    return true;
}

void HelloWorld::update(float dt) {
	alpha = alpha + 300 * dt;
	unsigned char C = 256 - std::abs(256 - ((long)alpha % 512));
	label->setColor(Color3B(C, C, C));
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
    
    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    
    
}
