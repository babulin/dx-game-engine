#pragma once
#include "Core.h"
#include "GWzlOffset.h"

struct GTextureVertex
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};

struct GTextureVertexRHW
{
	float x, y, z, rhw;
	float nx, ny, nz;
	float u, v;
	static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_NORMAL | D3DFVF_TEX1;
};

class GWzlDraw {
public:
	WzlImage sImage;
	BYTE* data;
	HUM_STATE mState; //当前动画状态
	DIRECTION mDir;//当前帧方向
	int frame;//当前帧数
	IDirect3DSurface9* m_d3dSurface = nullptr;
	D3DSURFACE_DESC m_d3dSurfaceDesc;

	IDirect3DVertexBuffer9* m_d3dBuffer;

private:
	//8位图 256调色板
	PALETTEENTRY m_color[256] = {
		PALETTEENTRY{0, 0, 0},
		PALETTEENTRY{128, 0, 0},
		PALETTEENTRY{0, 128, 0},
		PALETTEENTRY{128, 128, 0},
		PALETTEENTRY{0, 0, 128},
		PALETTEENTRY{128, 0, 128},
		PALETTEENTRY{0, 128, 128},
		PALETTEENTRY{192, 192, 192},
		PALETTEENTRY{85, 128, 151},
		PALETTEENTRY{157, 185, 200},
		PALETTEENTRY{123, 115, 115},
		PALETTEENTRY{45, 41, 41},
		PALETTEENTRY{90, 82, 82},
		PALETTEENTRY{99, 90, 90},
		PALETTEENTRY{66, 57, 57},
		PALETTEENTRY{29, 24, 24},
		PALETTEENTRY{24, 16, 16},
		PALETTEENTRY{41, 24, 24},
		PALETTEENTRY{16, 8, 8},
		PALETTEENTRY{242, 121, 113},
		PALETTEENTRY{225, 103, 95},
		PALETTEENTRY{255, 90, 90},
		PALETTEENTRY{255, 49, 49},
		PALETTEENTRY{214, 90, 82},
		PALETTEENTRY{148, 16, 0},
		PALETTEENTRY{148, 41, 24},
		PALETTEENTRY{57, 8, 0},
		PALETTEENTRY{115, 16, 0},
		PALETTEENTRY{181, 24, 0},
		PALETTEENTRY{189, 99, 82},
		PALETTEENTRY{66, 24, 16},
		PALETTEENTRY{255, 170, 153},
		PALETTEENTRY{90, 16, 0},
		PALETTEENTRY{115, 57, 41},
		PALETTEENTRY{165, 74, 49},
		PALETTEENTRY{148, 123, 115},
		PALETTEENTRY{189, 82, 49},
		PALETTEENTRY{82, 33, 16},
		PALETTEENTRY{123, 49, 24},
		PALETTEENTRY{45, 24, 16},
		PALETTEENTRY{140, 74, 49},
		PALETTEENTRY{148, 41, 0},
		PALETTEENTRY{189, 49, 0},
		PALETTEENTRY{198, 115, 82},
		PALETTEENTRY{107, 49, 24},
		PALETTEENTRY{198, 107, 66},
		PALETTEENTRY{206, 74, 0},
		PALETTEENTRY{165, 99, 57},
		PALETTEENTRY{90, 49, 24},
		PALETTEENTRY{42, 16, 0},
		PALETTEENTRY{21, 8, 0},
		PALETTEENTRY{58, 24, 0},
		PALETTEENTRY{8, 0, 0},
		PALETTEENTRY{41, 0, 0},
		PALETTEENTRY{74, 0, 0},
		PALETTEENTRY{157, 0, 0},
		PALETTEENTRY{220, 0, 0},
		PALETTEENTRY{222, 0, 0},
		PALETTEENTRY{251, 0, 0},
		PALETTEENTRY{156, 115, 82},
		PALETTEENTRY{148, 107, 74},
		PALETTEENTRY{115, 74, 41},
		PALETTEENTRY{82, 49, 24},
		PALETTEENTRY{140, 74, 24},
		PALETTEENTRY{136, 68, 17},
		PALETTEENTRY{74, 33, 0},
		PALETTEENTRY{33, 24, 16},
		PALETTEENTRY{214, 148, 90},
		PALETTEENTRY{198, 107, 33},
		PALETTEENTRY{239, 107, 0},
		PALETTEENTRY{255, 119, 0},
		PALETTEENTRY{165, 148, 132},
		PALETTEENTRY{66, 49, 33},
		PALETTEENTRY{24, 16, 8},
		PALETTEENTRY{41, 24, 8},
		PALETTEENTRY{33, 16, 0},
		PALETTEENTRY{57, 41, 24},
		PALETTEENTRY{140, 99, 57},
		PALETTEENTRY{66, 41, 16},
		PALETTEENTRY{107, 66, 24},
		PALETTEENTRY{123, 74, 24},
		PALETTEENTRY{148, 74, 0},
		PALETTEENTRY{140, 132, 123},
		PALETTEENTRY{107, 99, 90},
		PALETTEENTRY{74, 66, 57},
		PALETTEENTRY{41, 33, 24},
		PALETTEENTRY{70, 57, 41},
		PALETTEENTRY{181, 165, 148},
		PALETTEENTRY{123, 107, 90},
		PALETTEENTRY{206, 177, 148},
		PALETTEENTRY{165, 140, 115},
		PALETTEENTRY{140, 115, 90},
		PALETTEENTRY{181, 148, 115},
		PALETTEENTRY{214, 165, 115},
		PALETTEENTRY{239, 165, 74},
		PALETTEENTRY{239, 198, 140},
		PALETTEENTRY{123, 99, 66},
		PALETTEENTRY{107, 86, 57},
		PALETTEENTRY{189, 148, 90},
		PALETTEENTRY{99, 57, 0},
		PALETTEENTRY{214, 198, 173},
		PALETTEENTRY{82, 66, 41},
		PALETTEENTRY{148, 99, 24},
		PALETTEENTRY{239, 214, 173},
		PALETTEENTRY{165, 140, 99},
		PALETTEENTRY{99, 90, 74},
		PALETTEENTRY{189, 165, 123},
		PALETTEENTRY{90, 66, 24},
		PALETTEENTRY{189, 140, 49},
		PALETTEENTRY{53, 49, 41},
		PALETTEENTRY{148, 132, 99},
		PALETTEENTRY{123, 107, 74},
		PALETTEENTRY{165, 140, 90},
		PALETTEENTRY{90, 74, 41},
		PALETTEENTRY{156, 123, 57},
		PALETTEENTRY{66, 49, 16},
		PALETTEENTRY{239, 173, 33},
		PALETTEENTRY{24, 16, 0},
		PALETTEENTRY{41, 33, 0},
		PALETTEENTRY{156, 107, 0},
		PALETTEENTRY{148, 132, 90},
		PALETTEENTRY{82, 66, 24},
		PALETTEENTRY{107, 90, 41},
		PALETTEENTRY{123, 99, 33},
		PALETTEENTRY{156, 123, 33},
		PALETTEENTRY{222, 165, 0},
		PALETTEENTRY{90, 82, 57},
		PALETTEENTRY{49, 41, 16},
		PALETTEENTRY{206, 189, 123},
		PALETTEENTRY{99, 90, 57},
		PALETTEENTRY{148, 132, 74},
		PALETTEENTRY{198, 165, 41},
		PALETTEENTRY{16, 156, 24},
		PALETTEENTRY{66, 140, 74},
		PALETTEENTRY{49, 140, 66},
		PALETTEENTRY{16, 148, 41},
		PALETTEENTRY{8, 24, 16},
		PALETTEENTRY{8, 24, 24},
		PALETTEENTRY{8, 41, 16},
		PALETTEENTRY{24, 66, 41},
		PALETTEENTRY{165, 181, 173},
		PALETTEENTRY{107, 115, 115},
		PALETTEENTRY{24, 41, 41},
		PALETTEENTRY{24, 66, 74},
		PALETTEENTRY{49, 66, 74},
		PALETTEENTRY{99, 198, 222},
		PALETTEENTRY{68, 221, 255},
		PALETTEENTRY{140, 214, 239},
		PALETTEENTRY{115, 107, 57},
		PALETTEENTRY{247, 222, 57},
		PALETTEENTRY{247, 239, 140},
		PALETTEENTRY{247, 231, 0},
		PALETTEENTRY{107, 107, 90},
		PALETTEENTRY{90, 140, 165},
		PALETTEENTRY{57, 181, 239},
		PALETTEENTRY{74, 156, 206},
		PALETTEENTRY{49, 132, 181},
		PALETTEENTRY{49, 82, 107},
		PALETTEENTRY{222, 222, 214},
		PALETTEENTRY{189, 189, 181},
		PALETTEENTRY{140, 140, 132},
		PALETTEENTRY{247, 247, 222},
		PALETTEENTRY{0, 8, 24},
		PALETTEENTRY{8, 24, 57},
		PALETTEENTRY{8, 16, 41},
		PALETTEENTRY{8, 24, 0},
		PALETTEENTRY{8, 41, 0},
		PALETTEENTRY{0, 82, 165},
		PALETTEENTRY{0, 123, 222},
		PALETTEENTRY{16, 41, 74},
		PALETTEENTRY{16, 57, 107},
		PALETTEENTRY{16, 82, 140},
		PALETTEENTRY{33, 90, 165},
		PALETTEENTRY{16, 49, 90},
		PALETTEENTRY{16, 66, 132},
		PALETTEENTRY{49, 82, 132},
		PALETTEENTRY{24, 33, 49},
		PALETTEENTRY{74, 90, 123},
		PALETTEENTRY{82, 107, 165},
		PALETTEENTRY{41, 57, 99},
		PALETTEENTRY{16, 74, 222},
		PALETTEENTRY{41, 41, 33},
		PALETTEENTRY{74, 74, 57},
		PALETTEENTRY{41, 41, 24},
		PALETTEENTRY{74, 74, 41},
		PALETTEENTRY{123, 123, 66},
		PALETTEENTRY{156, 156, 74},
		PALETTEENTRY{90, 90, 41},
		PALETTEENTRY{66, 66, 20},
		PALETTEENTRY{57, 57, 0},
		PALETTEENTRY{89, 89, 0},
		PALETTEENTRY{202, 53, 44},
		PALETTEENTRY{107, 115, 33},
		PALETTEENTRY{41, 49, 0},
		PALETTEENTRY{49, 57, 16},
		PALETTEENTRY{49, 57, 24},
		PALETTEENTRY{66, 74, 0},
		PALETTEENTRY{82, 99, 24},
		PALETTEENTRY{90, 115, 41},
		PALETTEENTRY{49, 74, 24},
		PALETTEENTRY{24, 33, 0},
		PALETTEENTRY{24, 49, 0},
		PALETTEENTRY{24, 57, 16},
		PALETTEENTRY{99, 132, 74},
		PALETTEENTRY{107, 189, 74},
		PALETTEENTRY{99, 181, 74},
		PALETTEENTRY{99, 189, 74},
		PALETTEENTRY{90, 156, 74},
		PALETTEENTRY{74, 140, 57},
		PALETTEENTRY{99, 198, 74},
		PALETTEENTRY{99, 214, 74},
		PALETTEENTRY{82, 132, 74},
		PALETTEENTRY{49, 115, 41},
		PALETTEENTRY{99, 198, 90},
		PALETTEENTRY{82, 189, 74},
		PALETTEENTRY{16, 255, 0},
		PALETTEENTRY{24, 41, 24},
		PALETTEENTRY{74, 136, 74},
		PALETTEENTRY{74, 231, 74},
		PALETTEENTRY{0, 90, 0},
		PALETTEENTRY{0, 136, 0},
		PALETTEENTRY{0, 148, 0},
		PALETTEENTRY{0, 222, 0},
		PALETTEENTRY{0, 238, 0},
		PALETTEENTRY{0, 251, 0},
		PALETTEENTRY{74, 90, 148},
		PALETTEENTRY{99, 115, 181},
		PALETTEENTRY{123, 140, 214},
		PALETTEENTRY{107, 123, 214},
		PALETTEENTRY{119, 136, 255},
		PALETTEENTRY{198, 198, 206},
		PALETTEENTRY{148, 148, 156},
		PALETTEENTRY{156, 148, 198},
		PALETTEENTRY{49, 49, 57},
		PALETTEENTRY{41, 24, 132},
		PALETTEENTRY{24, 0, 132},
		PALETTEENTRY{74, 66, 82},
		PALETTEENTRY{82, 66, 123},
		PALETTEENTRY{99, 90, 115},
		PALETTEENTRY{206, 181, 247},
		PALETTEENTRY{140, 123, 156},
		PALETTEENTRY{119, 34, 204},
		PALETTEENTRY{221, 170, 255},
		PALETTEENTRY{240, 180, 42},
		PALETTEENTRY{223, 0, 159},
		PALETTEENTRY{227, 23, 179},
		PALETTEENTRY{255, 251, 240},
		PALETTEENTRY{160, 160, 164},
		PALETTEENTRY{128, 128, 128},
		PALETTEENTRY{255, 0, 0},
		PALETTEENTRY{0, 255, 0},
		PALETTEENTRY{255, 255, 0},
		PALETTEENTRY{0, 0, 255},
		PALETTEENTRY{255, 0, 255},
		PALETTEENTRY{0, 255, 255},
		PALETTEENTRY{255, 255, 255}
	};
public:
	GWzlDraw();
	~GWzlDraw();
	HRESULT CreateVectex(LPDIRECT3DDEVICE9 d3dDevice);
	HRESULT CreateVectexRHW(LPDIRECT3DDEVICE9 d3dDevice);
	void Draw(LPDIRECT3DDEVICE9 d3dDevice);
	void DrawTexture(LPDIRECT3DDEVICE9 d3dDevice);
	void DrawTextureRHW(LPDIRECT3DDEVICE9 d3dDevice);
};
