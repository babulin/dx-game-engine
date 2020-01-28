#include "GWzlDraw.h"



GWzlDraw::GWzlDraw()
{

}

GWzlDraw::~GWzlDraw()
{
	wchar_t buf[50];
	swprintf_s(buf, TEXT("~GWzlDraw()析构 data => %p \n"), data);
	OutputDebugString(buf);
	delete data;
}

//创建顶点缓存
float bei = 1.42f;
HRESULT GWzlDraw::CreateVectex(LPDIRECT3DDEVICE9 p_d3dDevice)
{
	if (m_d3dBuffer)
	{
		return S_OK;
	}

	float tWidth = sImage.width / 100.0f * bei;
	float tHeight = sImage.height / 100.0f * bei;

	float wof2 = -0.35f + (sImage.x / 100.0f * bei);
	float hof2 = 1.6f + (sImage.y / 100.0f * bei);

	tWidth = tWidth + wof2;
	tHeight = (tHeight - hof2) * -1.0f;

	GTextureVertex Vertices[] = {
	//{  -0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f},
	//{  -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f},
	//{  0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f},
	//{  0.5f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f},
	{  wof2,  tHeight, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f},
	{  wof2,  hof2, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f},
	{  tWidth,  tHeight, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f},
	{  tWidth,  hof2, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f},

	//{ -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f},
	//{  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f},
	};


	//创建顶点缓存
	if (FAILED(p_d3dDevice->CreateVertexBuffer(sizeof(Vertices), 0, GTextureVertex::FVF, D3DPOOL_DEFAULT, &m_d3dBuffer, NULL)))
	{
		return E_FAIL;
	}

	VOID* pVertices;
	if (FAILED(m_d3dBuffer->Lock(0, sizeof(Vertices), &pVertices, 0))) {
		return E_FAIL;
	}
	memcpy(pVertices, Vertices, sizeof(Vertices));
	m_d3dBuffer->Unlock();

	p_d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	p_d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

//创建顶点缓存
float bei2 = 1.0f;
HRESULT GWzlDraw::CreateVectexRHW(LPDIRECT3DDEVICE9 p_d3dDevice)
{
	if (m_d3dBuffer)
	{
		//return S_OK;
	}


	float tWidth = sImage.width * bei2;
	float tHeight = sImage.height * bei2;

	float wof2 = 400.0f + sImage.x;
	float hof2 = 300.0f + sImage.y;

	tWidth = tWidth + wof2;
	tHeight = tHeight + hof2;

	GTextureVertexRHW Vertices[] = {
		{  wof2 ,  tHeight, 0.0f, 1.0f,0.0f, 0.0f, -1.0f, 0.0f, 1.0f},
		{  wof2,   hof2, 0.0f, 1.0f,0.0f, 0.0f, -1.0f, 0.0f, 0.0f},
		{  tWidth,  tHeight, 0.0f, 1.0f,0.0f, 0.0f, -1.0f, 1.0f, 1.0f},
		{  tWidth,  hof2, 0.0f, 1.0f,0.0f, 0.0f, -1.0f, 1.0f, 0.0f},
	};


	//创建顶点缓存
	if (FAILED(p_d3dDevice->CreateVertexBuffer(sizeof(Vertices), 0, GTextureVertexRHW::FVF, D3DPOOL_DEFAULT, &m_d3dBuffer, NULL)))
	{
		return E_FAIL;
	}

	VOID* pVertices;
	if (FAILED(m_d3dBuffer->Lock(0, sizeof(Vertices), &pVertices, 0))) {
		return E_FAIL;
	}
	memcpy(pVertices, Vertices, sizeof(Vertices));
	m_d3dBuffer->Unlock();

	return S_OK;
}


void GWzlDraw::Draw(LPDIRECT3DDEVICE9 d3dDevice)
{
	//OutputDebugString(TEXT("绘制完成\n"));
	///创建表面
	D3DFORMAT fmt = sImage.pixelFormat == 3 ? D3DFMT_A8R8G8B8 : D3DFMT_R5G6B5;
	if (FAILED(d3dDevice->CreateOffscreenPlainSurface(sImage.width, sImage.height, fmt, D3DPOOL_DEFAULT, &m_d3dSurface, NULL))) {
		return;
	}

	//获取表面信息
	m_d3dSurface->GetDesc(&m_d3dSurfaceDesc);

	///表面数据
	D3DLOCKED_RECT lockRect;
	m_d3dSurface->LockRect(&lockRect, 0, 0);

	DWORD* imageData3 = nullptr;
	SHORT* imageData5 = nullptr;

	if (sImage.pixelFormat == 3)
	{
		imageData3 = (DWORD*)lockRect.pBits;
	}
	else if (sImage.pixelFormat == 5)
	{
		imageData5 = (SHORT*)lockRect.pBits;
	}

	for (UINT i = 0; i < m_d3dSurfaceDesc.Height; i++)
	{
		for (UINT j = 0; j < m_d3dSurfaceDesc.Width; j++)
		{
			if (sImage.pixelFormat == 3)
			{
				UINT index = i * lockRect.Pitch / 4 + j;
				UINT sort = ((m_d3dSurfaceDesc.Height - 1) - i) * m_d3dSurfaceDesc.Width + j;
		
				byte  r = m_color[data[sort]].peRed;
				byte  g = m_color[data[sort]].peGreen;
				byte  b = m_color[data[sort]].peBlue;
				if (r != 0 && g != 0 && b != 0)
				{
					imageData3[index] = D3DCOLOR_ARGB(1, r, g, b);
				}
				else {
					imageData3[index] = D3DCOLOR_ARGB(0, 255, 255, 255);
				}
			}
			else if (sImage.pixelFormat == 5)
			{
				//D3DFMT_R5G6B5
				UINT id = (((m_d3dSurfaceDesc.Height - 1) - i) * m_d3dSurfaceDesc.Width + j) * 2;
				BYTE sh1 = data[id];
				BYTE sh2 = data[id + 1];

				USHORT sVal = (sh2 << 8) | sh1;
				UINT index = i * lockRect.Pitch / 2 + j;
				imageData5[index] = sVal;
			}

		}
	}

	m_d3dSurface->UnlockRect();


	IDirect3DSurface9* pBackBuffer = NULL;
	int offsetX = 300 + sImage.x, offsetY = 300 + sImage.y;
	RECT rect1 = { 0,0,m_d3dSurfaceDesc.Width,m_d3dSurfaceDesc.Height };
	RECT rect2 = { offsetX ,offsetY ,m_d3dSurfaceDesc.Width + offsetX,m_d3dSurfaceDesc.Height + offsetY };
	d3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	d3dDevice->StretchRect(m_d3dSurface, &rect1, pBackBuffer, &rect2, D3DTEXF_LINEAR);
}

void GWzlDraw::DrawTexture(LPDIRECT3DDEVICE9 d3dDevice)
{
	CreateVectex(d3dDevice);

	LPD3DXSPRITE pSprite = NULL;
	LPDIRECT3DTEXTURE9 pTexture = NULL;

	HRESULT hr = S_OK;
	hr = D3DXCreateSprite(d3dDevice, &pSprite);

	///创建贴图
	D3DFORMAT fmt = sImage.pixelFormat == 3 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
	hr = d3dDevice->CreateTexture(sImage.width, sImage.height, 0, D3DUSAGE_DYNAMIC, fmt, D3DPOOL_DEFAULT, &pTexture, NULL);

	//获取表面信息
	pTexture->GetLevelDesc(0,&m_d3dSurfaceDesc);

	///表面数据
	D3DLOCKED_RECT lockRect;
	pTexture->LockRect(0,&lockRect, 0, 0);

	DWORD* imageData3 = nullptr;
	SHORT* imageData5 = nullptr;

	if (sImage.pixelFormat == 3)
	{
		imageData3 = (DWORD*)lockRect.pBits;
	}
	else if (sImage.pixelFormat == 5)
	{
		imageData5 = (SHORT*)lockRect.pBits;
	}

	for (UINT i = 0; i < m_d3dSurfaceDesc.Height; i++)
	{
		for (UINT j = 0; j < m_d3dSurfaceDesc.Width; j++)
		{
			if (sImage.pixelFormat == 3)
			{
				UINT index = i * lockRect.Pitch / 4 + j;
				UINT sort = ((m_d3dSurfaceDesc.Height - 1) - i) * m_d3dSurfaceDesc.Width + j;

				byte  r = m_color[data[sort]].peRed;
				byte  g = m_color[data[sort]].peGreen;
				byte  b = m_color[data[sort]].peBlue;
				if (r != 0 && g != 0 && b != 0)
				{
					imageData3[index] = D3DCOLOR_XRGB(r, g, b);
				}
				else {
					imageData3[index] = D3DCOLOR_XRGB(255, 255, 255);
				}
			}
			else if (sImage.pixelFormat == 5)
			{
				//D3DFMT_R5G6B5
				UINT id = (((m_d3dSurfaceDesc.Height - 1) - i) * m_d3dSurfaceDesc.Width + j) * 2;
				BYTE sh1 = data[id];
				BYTE sh2 = data[id + 1];

				USHORT sVal = (sh2 << 8) | sh1;
				UINT index = i * lockRect.Pitch / 2 + j;
				imageData5[index] = sVal;
			}

		}
	}
	//解锁
	pTexture->UnlockRect(0);


	int offsetX = 300 + sImage.x, offsetY = 300 + sImage.y;
	RECT rect1 = { 0,0,m_d3dSurfaceDesc.Width,m_d3dSurfaceDesc.Height };
	//D3DXVECTOR3 pos = D3DXVECTOR3(offsetX, offsetY, 0.0f);

	//pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	//pSprite->Draw(pTexture, &rect1, NULL, &pos, 0xffffffff);

	//pSprite->End();

	//pSprite->Release();
	//单位化世界矩阵
	//D3DXMATRIX matWorld;
	//D3DXMatrixIdentity(&matWorld);

	////缩放
	//D3DXMATRIX scale;
	//float bei = 1.6f;
	//float tWidth = sImage.width / 100.0f * bei;
	//float tHeight = sImage.height / 100.0f * bei;
	//D3DXMatrixScaling(&scale, tWidth, tHeight, 0.0f);

	////平移
	//float pyX = -0.55f,pyY = 0.2f;
	//float tX = (sImage.x / 100.0f * bei) + pyX;
	//float tY = (sImage.y / 100.0f * bei) + pyY;
	//D3DXMATRIX Tworld;
	//D3DXMatrixTranslation(&Tworld, tX, tY, 0.0f);

	//matWorld = scale * Tworld * matWorld;
	//d3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);


	d3dDevice->SetTexture(0, pTexture);

	d3dDevice->SetStreamSource(0, m_d3dBuffer, 0, sizeof(GTextureVertex));
	d3dDevice->SetFVF(GTextureVertex::FVF);
	d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void GWzlDraw::DrawTextureRHW(LPDIRECT3DDEVICE9 d3dDevice)
{
	CreateVectexRHW(d3dDevice);

	LPD3DXSPRITE pSprite = NULL;
	LPDIRECT3DTEXTURE9 pTexture = NULL;

	HRESULT hr = S_OK;
	//hr = D3DXCreateSprite(d3dDevice, &pSprite);

	///创建贴图
	D3DFORMAT fmt = sImage.pixelFormat == 3 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
	hr = d3dDevice->CreateTexture(sImage.width, sImage.height, 0, D3DUSAGE_DYNAMIC, fmt, D3DPOOL_DEFAULT, &pTexture, NULL);
	
	//获取表面信息
	pTexture->GetLevelDesc(0, &m_d3dSurfaceDesc);

	///表面数据
	D3DLOCKED_RECT lockRect;
	pTexture->LockRect(0, &lockRect, 0, 0);

	DWORD* imageData3 = nullptr;
	SHORT* imageData5 = nullptr;

	if (sImage.pixelFormat == 3)
	{
		imageData3 = (DWORD*)lockRect.pBits;
	}
	else if (sImage.pixelFormat == 5)
	{
		imageData5 = (SHORT*)lockRect.pBits;
	}

	for (UINT i = 0; i < m_d3dSurfaceDesc.Height; i++)
	{
		for (UINT j = 0; j < m_d3dSurfaceDesc.Width; j++)
		{
			if (sImage.pixelFormat == 3)
			{
				UINT index = i * lockRect.Pitch / 4 + j;

				UINT sort = ((m_d3dSurfaceDesc.Height - 1) - i) * m_d3dSurfaceDesc.Width + j;

				byte  r = m_color[data[sort]].peRed;
				byte  g = m_color[data[sort]].peGreen;
				byte  b = m_color[data[sort]].peBlue;
				if (r != 0 && g != 0 && b != 0)
				{
					imageData3[index] = D3DCOLOR_XRGB(r, g, b);
				}
				else {
					imageData3[index] = D3DCOLOR_ARGB(0,0, 0, 0);
				}
			}
			else if (sImage.pixelFormat == 5)
			{
				//D3DFMT_R5G6B5
				UINT id = (((m_d3dSurfaceDesc.Height - 1) - i) * m_d3dSurfaceDesc.Width + j) * 2;
				BYTE sh1 = data[id];
				BYTE sh2 = data[id + 1];

				USHORT sVal = (sh2 << 8) | sh1;
				UINT index = i * lockRect.Pitch / 2 + j;
				imageData5[index] = sVal;
			}

		}
	}
	//解锁
	pTexture->UnlockRect(0);


	int offsetX = 300 + sImage.x, offsetY = 300 + sImage.y;
	RECT rect1 = { 0,0,m_d3dSurfaceDesc.Width,m_d3dSurfaceDesc.Height };

	d3dDevice->SetTexture(0, pTexture);

	d3dDevice->SetStreamSource(0, m_d3dBuffer, 0, sizeof(GTextureVertexRHW));
	d3dDevice->SetFVF(GTextureVertexRHW::FVF);
	d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	pTexture->Release();
}
