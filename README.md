# ChineseChessEngineDemo
## Introduction:
	A chinese chess game demo using "cocos2dx" game engine and C++ language
## Reference:
	Cocos2d-x Developers Manual: http://www.cocos2d-x.org/wiki/Cocos2d-x
	ChineseChessEngine: https://github.com/MZfengIbb/ChineseChessEngine

## Tutorial: Build You Own Project and Run It
	1. Download cocos2d-x and configure you enviroment. Please read the developers manual.
	2. Create a cocos project using python command lines. Type in : cocos.py new ChineseGameDeomo -p com.whatever.gametitle -l cpp -d C:\YourGameTitle
	3. Copy and include all source files to your project. In visual studio, just right click 'src' in 'Solution Browser' and add source codes, then you can compile. In android studio, edit 'Android.mk' in 'proj.android-studio\app\jni' directory, add all the cpp files to item 'LOCAL_SRC_FILES :='.
	4. Run the demo in your prefered platform and enjoy it.