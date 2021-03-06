#include "GMap.h"
#include <unordered_map>
#include <algorithm>
#include "DrawLine.h"
#include "GAnimation.h"
#include "GPlayer.h"
#include "DrawGText.h"

#define WND_WIDTH 800
#define WND_HEIGHT 600

typedef std::unordered_map<int, GWzlDraw*> GDrawMap;
typedef std::pair<int, GWzlDraw*> GDrwa;

typedef std::unordered_map<int, GAnimation*> GAniMap;
typedef std::pair<int, GAnimation*> GAni;

//资源对象池
GWzlData* Tiles;//大地砖
GWzlData* SmTiles[6];//小地砖
GWzlData* no_Smtiles;//禁止通行
GWzlData* Objects[6];//物件
GWzlData* pWzlHum;//人物资源
GPlayer* g_player;//人物
DrawLine* g_line;//坐标线
DrawGText* g_text;//文字对象

//缓存容器
GDrawMap g_TilesMap;//大地砖
GDrawMap g_SmTilesMap;//小地砖
GDrawMap g_Objects[6];//物件
GDrawMap g_MaskObjects[6];//遮罩物件
GAniMap g_Animation;//灯泡

//坐标信息
//POS pos = { 323 ,283 };//人物坐标
POS map = { 48.0f,32.0f };//地图小格子信息
POS center = { 0,0 };//地图中心点，（人物给修改）

bool keyF = false, keyFF = false;
bool keyG = false, keyGG = true;
bool keyH = false, keyHH = false;
bool keyJ = false, keyJJ = true;
bool keyK = false, keyKK = true;

GMap::GMap(char file[], LPDIRECT3DDEVICE9 d3dDevice, LPD3DXFONT d3dFont)
{
	OutputDebugString(L"GMap()构造\n");
	p_d3dDevice = d3dDevice;
	p_d3dFont = d3dFont;
	//索引对象
	//pWzx = new GWzxData(file);
	for (int i = 0; i < 5; i++)
	{
		Objects[i] = nullptr;
		SmTiles[i] = nullptr;
	}
	sprintf_s(m_MapFile, ".\\map\\%s.map", file);

	//线
	g_line = new DrawLine(p_d3dDevice);
	//初始化字体类
	g_text = new DrawGText(p_d3dFont);

	Tiles = nullptr;
	//SmTiles = nullptr;


	//加载人物图片
	char humfile[] = ".\\Data\\hum";
	//char file[] = ".\\Data\\hum2";
	//char file[] = ".\\Data\\hum3";
	//char file[] = ".\\Data\\items";
	pWzlHum = new GWzlData(humfile);
	g_player = new GPlayer(pWzlHum, 332, 291, p_d3dDevice, this);


}

GMap::~GMap()
{
	delete Tiles;
	delete[] SmTiles;
	delete[] Objects;
	delete pWzlHum;
	delete g_player;
	delete g_line;
	//delete m_Draw;
	//delete m_Tiles;
	//delete m_SmTiles;
}

void GMap::Load()
{
	//人物初始化状态
	g_player->Load();

	FILE* fp;
	//读取wzx
	fopen_s(&fp, m_MapFile, "r+b");
	if (fp == NULL) {
		OutputDebugString(TEXT("打开文件失败了\n"));
		fclose(fp);
		return;
	}

	//取出wzx头部结构体
	fread(&m_MapHeader, sizeof(MapHeader), 1, fp);

	//偏移头部信息
	fseek(fp, sizeof(MapHeader), SEEK_SET);

	//初始化数据区域大小
	ms_MapInfo = new MapInfo[m_MapHeader.width * m_MapHeader.height];

	//取出图片
	fread(ms_MapInfo, sizeof(MapInfo) * (m_MapHeader.width * m_MapHeader.height), 1, fp);

	fclose(fp);

}


void GMap::Show(int pX, int pY)
{
	//p_d3dDevice->BeginScene();
	//pos.x = pX;
	//pos.y = pY;

	center.x = g_player->mPos.x;
	center.y = g_player->mPos.y;

	//按索引绘制地图
	int width = m_MapHeader.width;
	int height = m_MapHeader.height;
	int VIEW = 20;
	int left	= center.x - VIEW < 0 ? 0 : center.x - VIEW;
	int top		= center.y - VIEW < 0 ? 0 : center.y - VIEW;
	int right	= center.x + VIEW > width ? width : center.x + VIEW;
	int bottom	= center.y + VIEW > height ? height : center.y + VIEW + 10;

	rect = { left,top,right,bottom };
	//line
	if (keyKK)
	{
		DrawWorldLine();
	}

	//Tiles
	if (keyFF)
	{
		DrawTiles();
	}

	//SmTiles
	if (keyGG)
	{
		DrawSmTiles();
	}

	//Objects
	if (keyHH)
	{
		DrawObjects();
	}

	g_line->Draw(400.0f, 284.0f, 400.0f, 316.0f, 0xffffffff);
	g_line->Draw(376.0f, 300.0f, 424.0f, 300.0f, 0xffffffff);

	//绘制人物
	g_player->Move();

	//人物坐标
	wchar_t buf[50] = { 0 };
	wsprintf(buf, L"xy：[%d,%d]", static_cast<int>(center.x), static_cast<int>(center.y));
	g_text->Draw(buf, 380, 300, 800, 600, 0xffffffff);

	if (keyHH)
	{
		//player-mask
		DrawPlayer();
	}
	//light
	if (keyJJ)
	{
		DrawAnimation();
	}

	//控制绘制
	BtnState();
}


bool GMap::GetLightWorldXY(float X, float Y, float& mX, float& mY)
{
	//(0,0)=>(24,16) (1,1)=>(72,48)
	mX = X * map.x + 400.0f - (center.x * map.x) - map.x/2;
	mY = Y * map.y + 300.0f - (center.y * map.y) - map.y/2;
	return true;
}

void GMap::DrawTiles()
{
	//Tiles
	float offsetX = 0, offsetY = 0;
	int map_sort = 0;
	int tiles_sort = 0;
	int file_area = 0;

	for (int x = rect.left; x < rect.right; x++)
	{
		for (int y = rect.top; y < rect.bottom; y++)
		{
			map_sort = x * m_MapHeader.height + y;
			//位与运算 0&1 = 0 去掉最高位

			if (x == 331 && y == 289)
			{
				int i = 0;
			}

			//Tiles
			tiles_sort = (ms_MapInfo[map_sort].wBkImg & 0b0111111111111111) - 1;
			//SmTiles
			//sm_tiles_sort = (ms_MapInfo[map_sort].wMidImg & 0b0111111111111111) - 1;

			//object
			//object_sort = (ms_MapInfo[map_sort].wFrImg & 0b0111111111111111) - 1;

			//文件号 +1
			file_area = ms_MapInfo[map_sort].btArea;

			if (tiles_sort >= 0 && (x % 2 == 0 && y % 2 == 0)) {

				//加载WzxData;
				char szMapFile[100] = ".\\Data\\Tiles";

				//加载WzlData
				if (Tiles == nullptr) {
					Tiles = new GWzlData(szMapFile);
				}

				//初始化一个
				GDrawMap::const_iterator tilesit = g_TilesMap.find(tiles_sort);
				if (tilesit == g_TilesMap.end())
				{
					GWzlDraw* tTiles = new GWzlDraw(p_d3dDevice);
					Tiles->LoadWzl(tiles_sort, tTiles);
					if (tTiles->data == nullptr)
					{
						//跳过本次循环
						delete tTiles;
						continue;
					}
					tTiles->CreateTexture();
					g_TilesMap.insert(GDrwa(tiles_sort, tTiles));
				}
				GWzlDraw* tObject = g_TilesMap.at(tiles_sort);
				GetMapWorldXY(tObject, x, y + 2, offsetX, offsetY);
				tObject->Draw(offsetX, offsetY,COLOR_ARGB);
			}
		}

	}
}

void GMap::DrawSmTiles()
{
	//SmTiles
	float offsetX = 0, offsetY = 0;
	int map_sort = 0;
	int sm_tiles_sort = 0;
	int file_area = 0;

	for (int x = rect.left; x < rect.right; x++)
	{
		for (int y = rect.top; y < rect.bottom; y++)
		{
			map_sort = x * m_MapHeader.height + y;
			//位与运算 0&1 = 0 去掉最高位

			//Tiles
			//tiles_sort = (ms_MapInfo[map_sort].wBkImg & 0b0111111111111111) - 1;
			//SmTiles
			sm_tiles_sort = (ms_MapInfo[map_sort].wMidImg & 0b0111111111111111) - 1;

			//object
			//object_sort = (ms_MapInfo[map_sort].wFrImg & 0b0111111111111111) - 1;

			//文件号 +1
			file_area = ms_MapInfo[map_sort].btArea;

			if (sm_tiles_sort > 0) {

				if (sm_tiles_sort == 262)
				{
					int i = 0;
				}

				//加载WzxData;
				char szMapFile[100] = ".\\Data\\SmTiles";
				if (file_area > 0) {
					//sprintf_s(szMapFile, ".\\Data\\SmTiles%d", file_area + 1);
				}

				//加载WzlData
				if (SmTiles[file_area] == nullptr) {
					SmTiles[file_area] = new GWzlData(szMapFile);
				}

				//初始化一个
				GDrawMap::const_iterator Smtilesit = g_SmTilesMap.find(sm_tiles_sort);
				if (Smtilesit == g_SmTilesMap.end())
				{
					GWzlDraw* tTiles = new GWzlDraw(p_d3dDevice);
					SmTiles[file_area]->LoadWzl(sm_tiles_sort, tTiles);
					if (tTiles->data == nullptr)
					{
						//跳过本次循环
						delete tTiles;
						continue;
					}
					tTiles->CreateTexture();
					g_SmTilesMap.insert(GDrwa(sm_tiles_sort, tTiles));
				}
				GWzlDraw* tObject = g_SmTilesMap.at(sm_tiles_sort);

				GetMapWorldXY(tObject, x, y, offsetX, offsetY);
				tObject->Draw(offsetX, offsetY,COLOR_ARGB);
				//p_d3dDevice->EndScene();
				//p_d3dDevice->Present(NULL, NULL, NULL, NULL);
				//int i = 0;

			}

			if ( ! CheckMap(x, y))
			{
				//加载WzxData;
				char szMapFile[100] = ".\\Data\\SmTiles";

				//加载WzlData
				if (SmTiles[0] == nullptr) {
					SmTiles[0] = new GWzlData(szMapFile);
				}

				//初始化一个
				const int no_sort = 59;
				GDrawMap::const_iterator NoSmtilesit = g_SmTilesMap.find(no_sort);
				if (NoSmtilesit == g_SmTilesMap.end())
				{
					GWzlDraw* tNoTiles = new GWzlDraw(p_d3dDevice);
					SmTiles[0]->LoadWzl(no_sort, tNoTiles);
					if (tNoTiles->data == nullptr)
					{
						//跳过本次循环
						delete tNoTiles;
						continue;
					}
					tNoTiles->CreateTexture();
					g_SmTilesMap.insert(GDrwa(no_sort, tNoTiles));
				}
				GWzlDraw* tNoObject = g_SmTilesMap.at(no_sort);
				GetMapWorldXY(tNoObject, x, y + 1, offsetX, offsetY);
				tNoObject->Draw(offsetX, offsetY, COLOR_ARGB);
			}
		}
	}
}

void GMap::DrawObjects()
{
	//Objects
	float offsetX = 0, offsetY = 0;
	int map_sort = 0;
	int object_sort = 0;
	int file_area = 0;
	

	for (int x = rect.left; x < rect.right; x++)
	{
		for (int y = rect.top; y < rect.bottom; y++)
		{
			map_sort = x * m_MapHeader.height + y;
			//位与运算 0&1 = 0 去掉最高位

			if (x == 338 && y == 254)
			{
				int i = 0;
			}

			//Tiles
			//tiles_sort = (ms_MapInfo[map_sort].wFrImg & 0b0111111111111111) - 1;
			//SmTiles
			//sm_tiles_sort = (ms_MapInfo[map_sort].wMidImg & 0b0111111111111111) - 1;

			//object
			object_sort = (ms_MapInfo[map_sort].wFrImg & 0b0111111111111111) - 1;

			//light [移除原地图上那个破旧的灯]
			int light = (ms_MapInfo[map_sort].btAniFrame);

			//文件号 +1
			file_area = ms_MapInfo[map_sort].btArea;

			if (object_sort > 0 && light == 0) {
				//加载WzxData;
				char szMapFile[100] = ".\\Data\\Objects";
				if (ms_MapInfo[map_sort].btArea) {
					sprintf_s(szMapFile, ".\\Data\\Objects%d", file_area + 1);
				}

				//加载WzlData
				if (Objects[file_area] == nullptr)
				{
					Objects[file_area] = new GWzlData(szMapFile);
				}

				//初始化一个
				GDrawMap::const_iterator objit = g_Objects[file_area].find(object_sort);
				if (objit == g_Objects[file_area].end())
				{
					GWzlDraw* tTiles = new GWzlDraw(p_d3dDevice);
					Objects[file_area]->LoadWzl(object_sort, tTiles);
					if (tTiles->data == nullptr)
					{
						//跳过本次循环
						delete tTiles;
						continue;
					}

					//输出文件
					//char file[100] = ".\\Bmp\\0.bmp";
					//sprintf_s(file, ".\\Bmp\\%d.bmp", object_sort);
					//tTiles->SaveBmp(file, tTiles->sImage.width, tTiles->sImage.height, tTiles->sImage.width * tTiles->sImage.height, tTiles->data);
					tTiles->CreateTexture();
					g_Objects[file_area].insert(GDrwa(object_sort, tTiles));
				}
				GWzlDraw* tObject = g_Objects[file_area].at(object_sort);
				GetMapWorldXY(tObject, x, y + 1, offsetX, offsetY);
				
				tObject->Draw(offsetX, offsetY, COLOR_ARGB);
				
				//p_d3dDevice->EndScene();
				//p_d3dDevice->Present(NULL, NULL, NULL, NULL);
				//int i = 0;
			}
		}
	}
}

void GMap::DrawPlayer()
{
	//Objects
	float offsetX = 0, offsetY = 0;
	int map_sort = 0;
	int object_sort = 0;
	int file_area = 0;

	for (int x = center.x; x < center.x + 2; x++)
	{
		for (int y = center.y + 1; y < rect.bottom; y++)
		{
			map_sort = x * m_MapHeader.height + y;

			//object
			object_sort = (ms_MapInfo[map_sort].wFrImg & 0b0111111111111111) - 1;

			//light [移除原地图上那个破旧的灯]
			int light = (ms_MapInfo[map_sort].btAniFrame);

			//文件号 +1
			file_area = ms_MapInfo[map_sort].btArea;

			if (object_sort > 0) {

				//获取object的图片对象
				GDrawMap::const_iterator objit = g_Objects[file_area].find(object_sort);
				if (objit == g_Objects[file_area].end()) {
					//没找到该对象
					continue;
				}

				//获取当前图片
				GWzlDraw* tObject = g_Objects[file_area].at(object_sort);

				//计算当前与人物Y距离
				int PosHeight = (y - center.y) * map.y;
				if (tObject->sImage.height > PosHeight) {

					//初始化一个
					GDrawMap::const_iterator objit = g_MaskObjects[file_area].find(object_sort);
					if (objit == g_MaskObjects[file_area].end())
					{
						GWzlDraw* tTiles = new GWzlDraw(p_d3dDevice);
						Objects[file_area]->LoadWzl(object_sort, tTiles);
						if (tTiles->data == nullptr)
						{
							//跳过本次循环
							delete tTiles;
							continue;
						}
						tTiles->CreateTexture(0x99);
						g_MaskObjects[file_area].insert(GDrwa(object_sort, tTiles));
					}
					GWzlDraw* tObject = g_MaskObjects[file_area].at(object_sort);
					GetMapWorldXY(tObject, x, y + 1, offsetX, offsetY);
					tObject->Draw(offsetX, offsetY, COLOR_ARGB);
					//p_d3dDevice->EndScene();
					//p_d3dDevice->Present(NULL, NULL, NULL, NULL);
					//int i = 0;
				}
			}
		}
	}
}

void GMap::DrawAnimation()
{
	//Objects
	float offsetX = 0, offsetY = 0;
	int map_sort = 0;
	int light_sort = 0;
	int object_sort = 0;
	int file_area = 0;

	for (int x = rect.left; x < rect.right; x++)
	{
		offsetX = x * 48;
		for (int y = rect.top; y < rect.bottom; y++)
		{
			map_sort = x * m_MapHeader.height + y;
			//位与运算 0&1 = 0 去掉最高位

			if (x == 338 && y == 254)
			{
				int i = 0;
			}

			//Tiles
			//tiles_sort = (ms_MapInfo[map_sort].wFrImg & 0b0111111111111111) - 1;
			//SmTiles
			//sm_tiles_sort = (ms_MapInfo[map_sort].wMidImg & 0b0111111111111111) - 1;

			//object
			object_sort = (ms_MapInfo[map_sort].wFrImg & 0b0111111111111111) - 1;
			//aniFrame
			light_sort = (ms_MapInfo[map_sort].btAniFrame);

			//文件号 +1
			file_area = ms_MapInfo[map_sort].btArea;

			if (light_sort > 0 && object_sort > 0) {
				//加载WzxData;
				char szMapFile[100] = ".\\Data\\Objects";
				if (file_area) {
					sprintf_s(szMapFile, ".\\Data\\Objects%d", file_area + 1);
				}

				//加载WzlData
				if (Objects[file_area] == nullptr)
				{
					Objects[file_area] = new GWzlData(szMapFile);
				}

				//初始化一个
				GAniMap::const_iterator lightit = g_Animation.find(object_sort);
				if (lightit == g_Animation.end())
				{
					GAnimation *tAni = new GAnimation(object_sort, 10, Objects[file_area], p_d3dDevice);
					tAni->Load();
					g_Animation.insert(GAni(object_sort, tAni));
				}
				GAnimation* tObject = g_Animation.at(object_sort);
				GetLightWorldXY(x, y - 2, offsetX, offsetY);
				tObject->Show(offsetX, offsetY);
				//p_d3dDevice->EndScene();
				//p_d3dDevice->Present(NULL, NULL, NULL, NULL);
				//int i = 0;
			}
		}

	}
}

void GMap::DrawWorldLine() {
	float oX = 0, oY = 0, oX1 = 0, oY1 = 0;
	for (int x = rect.left; x <= rect.right; x++)
	{
		GetWorldXY(x, 0, oX, oY, true);
		oY1 = oY + m_MapHeader.height * map.y;
		g_line->Draw(oX, oY, oX, oY1, 0xff00ffff);
	}

	for (int y = rect.top; y <= rect.bottom; y++)
	{
		GetWorldXY(0, y, oX, oY, true);
		oX1 = oX + m_MapHeader.width * map.x;
		g_line->Draw(oX, oY, oX1, oY, 0xff00ffff);
	}
}

bool GMap::GetWorldXY(float X, float Y, float& mX, float& mY, bool IsCenter)
{
	//(0,0)=>(24,16) (1,1)=>(72,48)
	mX = X * map.x + 400.0f - (center.x * map.x);
	mY = Y * map.y + 300.0f - (center.y * map.y);
	if (IsCenter)
	{
		mX = mX - map.x / 2;
		mY = mY - map.y / 2;
	}

	return true;
}

//绘图 传递格子坐标，返回实际绘制左上角坐标
bool GMap::GetMapWorldXY(GWzlDraw* GDraw, float X, float Y, float& mX, float& mY)
{
	//mx = 屏幕偏移 + （地图x * 格子宽度） - （人物x * 格子宽度） - （格子宽度/2）- 图像偏移x
	mX = 400.0f + (X * map.x) - (center.x * map.x) - (map.x / 2) - GDraw->sImage.x;
	mY = 300.0f + (Y * map.y) - (center.y * map.y) - (map.y / 2) - GDraw->sImage.y - GDraw->sImage.height;
	return true;
}


bool GMap::keyMouse(int x, int y, BUTTON_KEY bk)
{
	int px = x - WND_WIDTH/2;
	int py = y - WND_HEIGHT/2;

	//反正切(已知y/x求角度) 正切(已知角度求y/x)
	double rad = std::atan2(py, px);
	int angle = 180 * rad / PI;//弧度制转角度
	
	wchar_t buf[50] = { 0 };
	//swprintf_s(buf, TEXT("坐标：%d,%d;[%d]\n"), px, px, angle);
	//OutputDebugString(buf);

	switch (bk)
	{
	case L_MOVE_BUTTON_DOWN:
	case L_BUTTON_DOWN:
		g_player->L_KEY = L_BUTTON_DOWN;
		g_player->mState = WALK;
		g_player->SetDir(angle,1);
		swprintf_s(buf, TEXT("左键按下：%d,%d;\t角度:%d°;方向:\t%d\n"), x, y, angle, g_player->mDir);
		break;
	case L_BUTTON_UP:
		g_player->L_KEY = L_BUTTON_UP;
		//swprintf_s(buf, TEXT("左键弹起：%d,%d\n"), x, y);
		break;
	case R_MOVE_BUTTON_DOWN:
	case R_BUTTON_DOWN:
		g_player->L_KEY = R_BUTTON_DOWN;
		g_player->mState = RUN;
		g_player->SetDir(angle,2);
		swprintf_s(buf, TEXT("左键按下：%d,%d;\t角度:%d°;方向:\t%d\n"), x, y, angle, g_player->mDir);
		//swprintf_s(buf, TEXT("右键按下：%d,%d\n"), x, y);
		break;
	case R_BUTTON_UP:
		g_player->L_KEY = R_BUTTON_UP;
		//swprintf_s(buf, TEXT("右键弹起：%d,%d\n"), x, y);
		break;
	default:
		break;
	}

	OutputDebugString(buf);
	return false;
}

bool GMap::CheckMap(int x, int y)
{
	int sort = x * m_MapHeader.height + y;
	//位与运算 0&1 = 0 去掉最高位

	if (x == 331 && y == 289)
	{
		int i = 0;
	}

	//Tiles
	int bsort = ((ms_MapInfo[sort].wBkImg & 0b1000000000000000) > 15);
	if (bsort == 1){
		return false;
	}
	return true;
}


//按钮开启各种状态
void GMap::BtnState()
{
	//F
	if (::GetAsyncKeyState(0x46) & 0x8000f) {
		if (keyF)
		{
			keyFF = keyFF ? false : true;
			keyF = false;
		}
	}
	else {
		keyF = true;
	}

	//G
	if (::GetAsyncKeyState(0x47) & 0x8000f) {
		if (keyG)
		{
			keyGG = keyGG ? false : true;
			keyG = false;
		}
	}
	else {
		keyG = true;
	}

	//H
	if (::GetAsyncKeyState(0x48) & 0x8000f) {
		if (keyH)
		{
			keyHH = keyHH ? false : true;
			keyH = false;
		}

	}
	else {
		keyH = true;
	}

	//J
	if (::GetAsyncKeyState(0x4A) & 0x8000f) {
		if (keyJ)
		{
			keyJJ = keyJJ ? false : true;
			keyJ = false;
		}
	}
	else {
		keyJ = true;
	}

	//K
	if (::GetAsyncKeyState(0x4B) & 0x8000f) {
		if (keyK)
		{
			keyKK = keyKK ? false : true;
			keyK = false;
		}
	}
	else {
		keyK = true;
	}
}

bool GMap::KeyBoard(char key, BUTTON_KEY bk)
{
	wchar_t buf[50] = { 0 };
	switch (bk)
	{
	case KB_DOWN:
		swprintf_s(buf, TEXT("按下：%c\n"), key);
		break;
	case KB_UP:
		swprintf_s(buf, TEXT("弹起：%c\n"), key);
		break;
	default:
		break;
	}
	OutputDebugString(buf);
	return false;
}