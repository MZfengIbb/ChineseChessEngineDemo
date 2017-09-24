#pragma once
#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#include <ChineseChessEngine/engine.h>
#include "cocos2d.h"
#include "ui/CocosGUI.h"

class GameScene :
	public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(GameScene);
private:
	Engine * obj_engine;
	cocos2d::Sprite * spr_chess[256];
	cocos2d::ui::Button * btn_retry, * btn_retract;
	cocos2d::DrawNode * drawNode;
	cocos2d::DrawNode * dNode_selected;
	void * audio; // SimpleAudioEngine
	long blockSize;
	long img_x0, img_y0;
	long mv, from_idx, to_idx;
	enum AI_LEVEL {
		EASY = 1,
		HARD = 6,
		EXPERT = 15
	};
	void setAILevelEasy(cocos2d::Ref * ref);
	void setAILevelHard(cocos2d::Ref * ref);
	void setAILevelExpert(cocos2d::Ref * ref);
	void retry(cocos2d::Ref * ref);
	void retract(cocos2d::Ref * ref);
	long ai_level;
	bool is_searching;
	void initEngine();
	void drawBoard();
	void initInteraction();
	void updateBoard();
	bool doMove();
	void aiEngineSearchMain();
};

#endif // _GAMESCENE_H_