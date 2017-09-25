#include "GameScene.h"
#include "SimpleAudioEngine.h"

#include <cstring>

USING_NS_CC;
#define STR_KING "King"
#define STR_GUARD "Guard"
#define STR_BISHOP "Bishop"
#define STR_KNIGHT "Knight"
#define STR_ROOK "Rook"
#define STR_CANNON "Cannon"
#define STR_PAWN "Pawn"

#define STR_RETRY "Retry"
#define STR_RETRACT "Retract"

#define MOVE_ACTION_DURATION 0.15

Scene* GameScene::createScene() {
	return GameScene::create();
}

bool GameScene::init() {
	if (!Scene::init()) {
		return false;
	}
	
	initEngine();
	drawBoard();
	initInteraction();
}

#define IMGX(x) (img_x0 + (x - 1)*blockSize)
#define IMGY(y) (img_y0 + (y - 1)*blockSize)
#define IMGXY(x,y) IMGX(x),IMGY(y)
#define COLX(x) (int)((x-img_x0)/blockSize + 1.5)
#define COLY(y) (int)((y-img_y0)/blockSize + 1.5)
#define COLXY(x,y) COLX(x),COLY(y)
#define IDXXY(x,y) ((13 - y) * 16 + x + 2)
#define XIDX(idx) (idx % 16 - 2)
#define YIDX(idx) (13 - idx / 16)
#define XYIDX(idx)  XIDX(idx),YIDX(idx)

void GameScene::onTouchEnded(Touch * touch, Event * event)
{
	if (is_searching) return;
	auto img_x = touch->getLocation().x;
	auto img_y = touch->getLocation().y;
	auto col_x = COLX(img_x);
	auto col_y = COLY(img_y);
	auto idx = IDXXY(col_x,col_y);
	if (((int)(obj_engine->_board._board[idx] - 8)/8) == obj_engine->_board._player) {
		dNode_selected->clear();
		Vec2 vec2s[8] = { 
			Vec2(IMGX(col_x) - blockSize / 2, IMGY(col_y) + blockSize / 2),
			Vec2(IMGX(col_x), IMGY(col_y) + blockSize / 2),
			Vec2(IMGX(col_x) + blockSize / 2, IMGY(col_y) + blockSize / 2),
			Vec2(IMGX(col_x) - blockSize / 2, IMGY(col_y)),
			Vec2(IMGX(col_x) + blockSize / 2, IMGY(col_y)),
			Vec2(IMGX(col_x) - blockSize / 2, IMGY(col_y) - blockSize / 2),
			Vec2(IMGX(col_x), IMGY(col_y) - blockSize / 2),
			Vec2(IMGX(col_x) + blockSize / 2, IMGY(col_y) - blockSize / 2)
		};
		dNode_selected->drawPoints(
			vec2s, 8, 3.0f, Color4F(0, 0, 0, 1.0f));
		from_idx = idx;
	}
	else {
        try{
            to_idx = idx;
            mv = move(from_idx, to_idx);
            if (obj_engine->_board.legal_move(mv)) {
                if (doMove()) {
                    //AI engine search for next move
                    is_searching = true;
                    auto searchCallBack = CallFunc::create(CC_CALLBACK_0(GameScene::aiEngineSearchMain, this));
                    this->runAction(Sequence::create(DelayTime::create(MOVE_ACTION_DURATION + 0.1), searchCallBack, nullptr));
                    is_searching = false;
                }
            }
            else {
                ((CocosDenshion::SimpleAudioEngine *)audio)->playEffect("sound/wrong.wav");

            }
		}
        catch(...){

        }
	}
}


bool GameScene::doMove()
{
	obj_engine->_board.do_move(mv);
	auto moveChess = MoveTo::create(MOVE_ACTION_DURATION , Vec2(IMGX(XIDX(to(mv))), IMGY(YIDX(to(mv)))));
	dNode_selected->clear();
	((Sprite*)spr_chess[from(mv)])->runAction(moveChess);
	if (spr_chess[to(mv)] != nullptr)
	{
		this->removeChild((Sprite*)spr_chess[to(mv)]);
	}
	spr_chess[to(mv)] = spr_chess[from(mv)];
	spr_chess[from(mv)] = nullptr;
	if (obj_engine->_board.is_mate()) {
		switch (obj_engine->_board._player)
		{
		case 1:
			((CocosDenshion::SimpleAudioEngine *)audio)->playEffect("sound/win.wav");
			break;
		case 0:
			((CocosDenshion::SimpleAudioEngine *)audio)->playEffect("sound/loss.wav");
			break;
		default:
			break;
		}
		return false;
	}
	
	else if (obj_engine->_board.checked()) {
		((CocosDenshion::SimpleAudioEngine *)audio)->playEffect("sound/check.wav");
	}

	else if (obj_engine->_board.captured())
	{
		((CocosDenshion::SimpleAudioEngine *)audio)->playEffect("sound/captured.wav");
	}
	else {
		((CocosDenshion::SimpleAudioEngine *)audio)->playEffect("sound/move.wav");
	}
	return true;
}

void GameScene::aiEngineSearchMain()
{
	obj_engine->search_main(ai_level / 10.0f);
	mv = obj_engine->_mv_result;
	doMove();

}

void GameScene::menuCloseCallback(Ref* pSender)
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

void GameScene::initEngine()
{
	if (obj_engine == nullptr) {
		obj_engine = new Engine();
	}
	if (drawNode == nullptr)
	{
		drawNode = CCDrawNode::create();
	}
	if (dNode_selected == nullptr)
	{
		dNode_selected = DrawNode::create();
	}
	if (audio == nullptr) {
		audio = CocosDenshion::SimpleAudioEngine::getInstance();
	}
	ai_level = AI_LEVEL::HARD;
	is_searching = false;
	//if (btn_retry == nullptr) {
	//	btn_retry = ui::Button::create();
	//}
	//if (btn_retract == nullptr) {
	//	btn_retract = ui::Button::create();
	//}
	obj_engine->init();
	this->addChild(drawNode,1);
	this->addChild(dNode_selected, 4);
}

void GameScene::drawBoard()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	if (visibleSize.width > visibleSize.height) {
		blockSize = visibleSize.height / 10;
		img_x0 = origin.x + visibleSize.width / 2 - blockSize * 4;
		img_y0 = origin.y + blockSize / 2;
	}
	else {
		blockSize = visibleSize.width / 10;
		img_x0 = origin.x + blockSize / 2;
		img_y0 = origin.y + visibleSize.height / 2 - blockSize * 5;
	}
	auto drawGridNode = CCDrawNode::create();
	for (int y = 1; y <= 10; y++)
	{
		for (int x = 1; x <= 9; x++) {
			auto block = Sprite::create("image/Background.png");
			block->setContentSize(Size(blockSize, blockSize));
			block->setPosition(IMGXY(x,y));
			this->addChild(block, 0);
		}
		drawGridNode->drawSegment(Vec2(IMGXY(1, y)), Vec2(IMGXY(9, y)), 1.0f, Color4F(0, 0, 0, 1.0f));
	}
	for (int x = 1; x <= 9; x++) {
		drawGridNode->drawSegment(Vec2(IMGXY(x, 1)), Vec2(IMGXY(x, 5)), 1.0f, Color4F(0, 0, 0, 1.0f));
		drawGridNode->drawSegment(Vec2(IMGXY(x, 6)), Vec2(IMGXY(x, 10)), 1.0f, Color4F(0, 0, 0, 1.0f));
	}
	drawGridNode->drawSegment(Vec2(IMGXY(1, 1)), Vec2(IMGXY(1, 10)), 2.0f, Color4F(0, 0, 0, 1.0f));
	drawGridNode->drawSegment(Vec2(IMGXY(1, 1)), Vec2(IMGXY(9, 1)), 2.0f, Color4F(0, 0, 0, 1.0f));
	drawGridNode->drawSegment(Vec2(IMGXY(9, 10)), Vec2(IMGXY(1, 10)), 2.0f, Color4F(0, 0, 0, 1.0f));
	drawGridNode->drawSegment(Vec2(IMGXY(9, 10)), Vec2(IMGXY(9, 1)), 2.0f, Color4F(0, 0, 0, 1.0f));
	drawGridNode->drawSegment(Vec2(IMGXY(4, 1)), Vec2(IMGXY(6, 3)), 1.0f, Color4F(0, 0, 0, 1.0f));
	drawGridNode->drawSegment(Vec2(IMGXY(6, 1)), Vec2(IMGXY(4, 3)), 1.0f, Color4F(0, 0, 0, 1.0f));
	drawGridNode->drawSegment(Vec2(IMGXY(4, 10)), Vec2(IMGXY(6, 8)), 1.0f, Color4F(0, 0, 0, 1.0f));
	drawGridNode->drawSegment(Vec2(IMGXY(6, 10)), Vec2(IMGXY(4, 8)), 1.0f, Color4F(0, 0, 0, 1.0f));

	drawNode->addChild(drawGridNode, 1);
	updateBoard();
}

void GameScene::initInteraction()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(GameScene::menuCloseCallback, this));

	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
		origin.y + closeItem->getContentSize().height / 2));

	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);


	auto _touchListener = EventListenerTouchOneByOne::create();
	_touchListener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
	_touchListener->onTouchMoved = [](Touch* touch, Event* event) {};
	_touchListener->onTouchCancelled = [](Touch* touch, Event* event) {};
	_touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);

	auto lbl_ai_level = Label::createWithTTF("-- AI LEVEL --", "fonts/BRLNSB.TTF", visibleSize.height / 30);
	auto lbl_ai_easy = Label::createWithTTF("EASY", "fonts/BRLNSB.TTF", visibleSize.height / 30);
	auto lbl_ai_hard = Label::createWithTTF("HARD", "fonts/BRLNSB.TTF", visibleSize.height / 30);
	auto lbl_ai_expert = Label::createWithTTF("EXPERT", "fonts/BRLNSB.TTF", visibleSize.height / 30);

	auto lbl_retract = Label::createWithTTF("RETRACT", "fonts/BRLNSB.TTF", visibleSize.height / 30);
	auto lbl_retry = Label::createWithTTF("RETRY", "fonts/BRLNSB.TTF", visibleSize.height / 30);
	auto lbl_exit = Label::createWithTTF("EXIT", "fonts/BRLNSB.TTF", visibleSize.height / 30);

	auto menuitem_ai_easy = MenuItemLabel::create(lbl_ai_easy, CC_CALLBACK_1(GameScene::setAILevelEasy, this));
	auto menuitem_ai_hard = MenuItemLabel::create(lbl_ai_hard, CC_CALLBACK_1(GameScene::setAILevelHard, this));
	auto menuitem_ai_expert = MenuItemLabel::create(lbl_ai_expert, CC_CALLBACK_1(GameScene::setAILevelExpert, this));

	auto menuitem_retract = MenuItemLabel::create(lbl_retract, CC_CALLBACK_1(GameScene::retract, this));
	auto menuitem_retry = MenuItemLabel::create(lbl_retry, CC_CALLBACK_1(GameScene::retry, this));
	auto menuitem_exit = MenuItemLabel::create(lbl_exit, CC_CALLBACK_1(GameScene::menuCloseCallback, this));

	lbl_ai_level->setPosition(IMGXY(-1,9));
	menuitem_ai_easy->setPosition(IMGXY(-1, 8));
	menuitem_ai_hard->setPosition(IMGXY(-1, 7));
	menuitem_ai_expert->setPosition(IMGXY(-1, 6));

	menuitem_retract->setPosition(IMGXY(11, 4));
	menuitem_retry->setPosition(IMGXY(11, 2));
	menuitem_exit->setPosition(IMGXY(11, 1));

	this->addChild(lbl_ai_level, 1);
	menu->addChild(menuitem_ai_easy,1);
	menu->addChild(menuitem_ai_hard, 2);
	menu->addChild(menuitem_ai_expert, 3);

	menu->addChild(menuitem_retract, 1);
	menu->addChild(menuitem_retry, 2);
	menu->addChild(menuitem_exit, 3);

	this->addChild(menu, 1);
}

void GameScene::setAILevelEasy(cocos2d::Ref * ref)
{
	ai_level = AI_LEVEL::EASY;
}

void GameScene::setAILevelHard(cocos2d::Ref * ref)
{
	ai_level = AI_LEVEL::HARD;
}

void GameScene::setAILevelExpert(cocos2d::Ref * ref)
{
	ai_level = AI_LEVEL::EXPERT;
}

void GameScene::retry(cocos2d::Ref * ref)
{
	if (!is_searching) {
		obj_engine->init();
		updateBoard();
	}
}

void GameScene::retract(cocos2d::Ref * ref)
{
	if (!is_searching) {
		obj_engine->_board.undo_move();
		obj_engine->_board.undo_move();
		updateBoard();
	}
}


void GameScene::updateBoard()
{
	std::string str_chessname;
	std::string str_chess_pic_path;
	for (int y = 1; y <= 10; y++)
	{
		for (int x = 1; x <= 9; x++)
		{
			auto idx = IDXXY(x, y);
			if (spr_chess[idx] != nullptr)
			{
				this->removeChild((Sprite*)spr_chess[idx]);
				spr_chess[idx]->release();
				spr_chess[idx] = nullptr;
			}
			if (obj_engine->_board._board[idx] >= 16)
			{
				str_chess_pic_path = "image/G_";
			}
			else if (obj_engine->_board._board[idx] >= 8) {
				str_chess_pic_path = "image/R_";
			}
			else {
				continue;
			}
			switch (obj_engine->_board._board[idx] & 7)
			{
			case KING:
				str_chessname = STR_KING;
				break;
			case GUARD:
				str_chessname = STR_GUARD;
				break;
			case BISHOP:
				str_chessname = STR_BISHOP;
				break;
			case KNIGHT:
				str_chessname = STR_KNIGHT;
				break;
			case ROOK:
				str_chessname = STR_ROOK;
				break;
			case CANNON:
				str_chessname = STR_CANNON;
				break;
			case PAWN:
				str_chessname = STR_PAWN;
				break;
			default:
				str_chessname = "";
				break;
			}
			str_chess_pic_path = str_chess_pic_path + str_chessname + ".png";
			auto chess = Sprite::create(str_chess_pic_path);
			chess->setPosition(IMGXY(x, y));
			chess->setContentSize(Size(blockSize*0.9,blockSize*0.9));
			spr_chess[idx] = chess;
			this->addChild(chess, 3);
		}
	}
}

