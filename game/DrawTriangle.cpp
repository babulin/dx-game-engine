#include "DrawTriangle.h"

DrawTriangle::DrawTriangle(LPDIRECT3DDEVICE9 d3dDevice)
{
    OutputDebugString(L"DrawTriangle()构造\n");
	p_d3dDevice = d3dDevice;
}

DrawTriangle::~DrawTriangle()
{
    OutputDebugString(L"~DrawTriangle()析构\n");
}

/*+----------------------------------------------------
 *|d3dpp.EnableAutoDepthStencil = TRUE;//自动维护深度缓存
 *|d3dpp.AutoDepthStencilFormat = D3DFMT_D16;//深度缓存像素格式[clear时清除ZBUFFER]
 *|m_d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);//D3DCLEAR_ZBUFFER清除深度内容
 *|如果开启自动深度维护，则就会启用深度，配合：clear使用 深度清屏 则绘制的点，线，三角形等z值就起作用
 *+----------------------------------------------------
 */
HRESULT DrawTriangle::CreateVectex()
{
    // Initialize three Vertices for rendering a triangle
    VertexTriangle Vertices[] =
    {
        { 300.0f,  100.0f, 1.0f, 1.0f, 0xffff0000, }, // x, y, z, rhw, color
        { 400.0f, 200.0f, 1.0f, 1.0f, 0xff00ff00, },
        { 150.0f, 200.0f, 1.0f, 1.0f, 0xff00ffff, },
    };

    // Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom Vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if (FAILED(p_d3dDevice->CreateVertexBuffer(sizeof(Vertices),0, D3DFVF_TRIANGLE,D3DPOOL_DEFAULT, &g_pVB, NULL)))
    {
        return E_FAIL;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the Vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
    VOID* pVertices;
    if (FAILED(g_pVB->Lock(0, sizeof(Vertices), &pVertices, 0)))
        return E_FAIL;
    memcpy(pVertices, Vertices, sizeof(Vertices));
    g_pVB->Unlock();

    return S_OK;
}

HRESULT DrawTriangle::Create3DVectex()
{
    // Initialize three Vertices for rendering a triangle
    Vertex3DTriangle Vertices[] =
    {
        { -1.0f,-1.0f, 0.0f, 0xffff0000, },
        {  0.0f, 1.0f, 0.0f, 0xffffffff, },
        {  1.0f,-1.0f, 0.0f, 0xff0000ff, },
        {  0.0f, -1.0f, -1.0f, 0xffffffff, },
    };

    // Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom Vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if (FAILED(p_d3dDevice->CreateVertexBuffer(sizeof(Vertices), 0, D3DFVF_3D_TRIANGLE, D3DPOOL_DEFAULT, &g_pVB, NULL)))
    {
        return E_FAIL;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the Vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
    VOID* pVertices;
    if (FAILED(g_pVB->Lock(0, sizeof(Vertices), &pVertices, 0)))
        return E_FAIL;
    memcpy(pVertices, Vertices, sizeof(Vertices));
    g_pVB->Unlock();

    // Turn off culling, so we see the front and back of the triangle
    p_d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // Turn off D3D lighting, since we are providing our own vertex colors
    p_d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    return S_OK;
}


void DrawTriangle::Draw()
{
    p_d3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(VertexTriangle));
    p_d3dDevice->SetFVF(D3DFVF_TRIANGLE);
    p_d3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
}

void DrawTriangle::Draw3D()
{
    p_d3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(Vertex3DTriangle));
    p_d3dDevice->SetFVF(D3DFVF_3D_TRIANGLE);
    p_d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
