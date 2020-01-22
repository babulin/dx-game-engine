#include "Graphics.h"

//初始化
Graphics::Graphics(GameWind& wnd):wnd(wnd){
	OutputDebugString(L"Graphics()构造\n");
	InitD3D();

	//初始化摄影机
	g_camera = new Camera(m_d3dDevice);

	//点
	g_point = new DrawPoint(m_d3dDevice);

	//线
	g_line = new DrawLine(m_d3dDevice);

	//初始化字体类
	g_text = new DrawGText(m_d3dFont);

	//初始化
	g_triangle = new DrawTriangle(m_d3dDevice);

	//初始化
	g_3dtriangle = new DrawTriangle(m_d3dDevice);
}

Graphics::~Graphics()
{
	if (m_d3dDevice != nullptr)
	{
		m_d3dDevice->Release();
	}

	if (m_d3d9 != nullptr)
	{
		m_d3d9->Release();
	}

	delete g_point;
	delete g_line;
	delete g_camera;
	delete g_text;
	delete g_triangle;
	delete g_3dtriangle;

	OutputDebugString(L"~Graphics()析构\n");
}

HRESULT Graphics::InitD3D()
{
	//创建D3D对象
	m_d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_d3d9 == NULL) {
		return E_FAIL;
	}

	///获取显卡模式
	D3DDISPLAYMODE d3ddm;
	if (FAILED(m_d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	//校验硬件顶点运算
	D3DCAPS9 caps;
	m_d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else {
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	//d3dpp.EnableAutoDepthStencil = TRUE;//自动维护深度缓存
	//d3dpp.AutoDepthStencilFormat = D3DFMT_D16;//深度缓存像素格式[clear时清除ZBUFFER] D3DCLEAR_ZBUFFER

	//创建device设备
	if (FAILED(m_d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd.mHwnd, vp, &d3dpp, &m_d3dDevice))) {
		return E_FAIL;
	}

	if (FAILED(D3DXCreateFont(m_d3dDevice, 20, 0, 0, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"微软雅黑", &m_d3dFont))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Graphics::InitVertex()
{
	//创建顶点缓存
	g_point->CreateVectex();

	//创建顶点缓存
	g_line->CreateVectex();

	//创建三角形顶点缓存
	g_triangle->CreateVectex();

	//创建3D三角形
	g_3dtriangle->Create3DVectex();

	return S_OK;
}

void Graphics::Render()
{

	m_d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);

	m_d3dDevice->BeginScene();
	//-----------------------------

	//视图
	g_camera->SetMatrices();

	//绘制FPS
	g_text->DrawFps(Getfps(),&wnd.clientRect,0xffffffff);

	//绘制鼠标
	wchar_t pos[50] = {0};
	wsprintf(pos, L"坐标：[%d,%d]", wnd.pt.x, wnd.pt.y);
	g_text->Draw(pos, 0, 20, 800, 600, 0x88ffffff);

	//画点列表
	g_point->Draw();

	//画线列表
	g_line->Draw();

	//画三角形
	g_triangle->Draw();

	//画3d三角形
	g_3dtriangle->Draw3D();

	//-----------------------------
	m_d3dDevice->EndScene();

	m_d3dDevice->Present(NULL, NULL, NULL, NULL);
}

//计算帧数
float Graphics::Getfps()
{
	static float fps = 0;   //我们需要计算的FPS值
	static int frameCount = 0;//帧数
	static float currentTime = 0.0f;//当前时间
	static float lastTime = 0.0f;//持续时间
	frameCount++;
	currentTime = timeGetTime() * 0.001f;
	if (currentTime - lastTime > 1.0f) {
		fps = (float)frameCount / (currentTime - lastTime);
		lastTime = currentTime;
		frameCount = 0;
	}
	return fps;
}
