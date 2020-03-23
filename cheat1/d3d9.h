#ifndef D3D9
#define D3D9

#include <d3dx9.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

extern IDirect3DDevice9Ex* p_Device; //
#define PI 3.14159265 ///

void DrawFilledRectangle(float x, float y, float w, float h, int a, int r, int g, int b)
{
	D3DCOLOR color = D3DCOLOR_ARGB(a, r, g, b);
	D3DRECT rect = { x, y, w, h };
	p_Device->Clear(1, &rect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, color, 0, 0);
}

void DrawFilledRectangle(float x, float y, float w, float h, D3DCOLOR color)
{
	D3DRECT rect = { x, y, w, h };
	p_Device->Clear(1, &rect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, color, 0, 0);
}

void DrawBorderBox(int x, int y, int w, int h, int thickness, int a, int r, int g, int b)
{
	DrawFilledRectangle(x, y, x + w, y + thickness, a, r, g, b);
	DrawFilledRectangle(x, y, x + thickness, y + h, a, r, g, b);
	DrawFilledRectangle(x, y + h, x + w, y + h + thickness, a, r, g, b);
	DrawFilledRectangle(x + w, y, x + w + thickness, y + h + thickness, a, r, g, b);
}

void DrawBorderBox(int x, int y, int w, int h, int thickness, D3DCOLOR color)
{
	DrawFilledRectangle(x, y, x + w, y + thickness, color);
	DrawFilledRectangle(x, y, x + thickness, y + h, color);
	DrawFilledRectangle(x, y + h, x + w, y + h + thickness, color);
	DrawFilledRectangle(x + w, y, x + w + thickness, y + h + thickness, color);
}

void DrawString(char* String, int x, int y, int a, int r, int g, int b, ID3DXFont* font)
{
	RECT FontPos;
	FontPos.left = x;
	FontPos.top = y;
	font->DrawTextA(0, String, strlen(String), &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(a, r, g, b));
}


void DrawString(char* String, int x, int y, D3DCOLOR color, ID3DXFont* font)
{
	RECT FontPos;
	FontPos.left = x;
	FontPos.top = y;
	font->DrawTextA(0, String, strlen(String), &FontPos, DT_NOCLIP, color);
}

void DrawString(char* String, int x, int y, int len, D3DCOLOR color, ID3DXFont* font)
{
	RECT FontPos;
	FontPos.left = x;
	FontPos.top = y;
	font->DrawTextA(0, String, len, &FontPos, DT_NOCLIP, color);
}

LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;    // the pointer to the vertex buffer
struct vertex {
	FLOAT x, y, z,
		rhw;
	DWORD color;
};

void DrawCircle(float x, float y, float rad, float rotate, int resolution, DWORD color)
{
	vector<vertex> circle(resolution + 2);
	float angle = rotate * PI / 180;

	circle[0].x = x - rad;
	circle[0].y = y;
	circle[0].z = 0;
	circle[0].rhw = 1;
	circle[0].color = color;

	for (int i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - rad * cos(PI*((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - rad * sin(PI*((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0;
		circle[i].rhw = 1;
		circle[i].color = color;
	}

	// Rotate matrix
	int _res = resolution + 2;
	for (int i = 0; i < _res; i++)
	{
		circle[i].x = x + cos(angle)*(circle[i].x - x) - sin(angle)*(circle[i].y - y);
		circle[i].y = y + sin(angle)*(circle[i].x - x) + cos(angle)*(circle[i].y - y);
	}

	p_Device->CreateVertexBuffer((resolution + 2) * sizeof(vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &v_buffer, NULL);

	VOID* pVertices;
	v_buffer->Lock(0, (resolution + 2) * sizeof(vertex), (void**)&pVertices, 0);
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(vertex));
	v_buffer->Unlock();

	p_Device->SetTexture(0, NULL);
	p_Device->SetPixelShader(NULL);
	p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	p_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	p_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	p_Device->SetStreamSource(0, v_buffer, 0, sizeof(vertex));
	p_Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	p_Device->DrawPrimitive(D3DPT_LINESTRIP, 0, resolution);
	if (v_buffer != NULL) v_buffer->Release();
}

extern int Width, Height;
IDirect3D9Ex* p_Object = 0;
D3DPRESENT_PARAMETERS p_Params;
LPDIRECT3DTEXTURE9 tex;
LPD3DXSPRITE sprite;
std::string file = "SpyModule.png";


class Resource {
public:
	std::size_t size_bytes = 0;
	void* ptr = nullptr;
	HRSRC hResource = nullptr;
	HGLOBAL hMemory = nullptr;
public:
	Resource(int resource_id, const std::string &resource_class) {
		hResource = FindResource(nullptr, MAKEINTRESOURCEA(resource_id), resource_class.c_str());
		hMemory = LoadResource(nullptr, hResource);

		size_bytes = SizeofResource(nullptr, hResource);  
		ptr = LockResource(hMemory);
	}
};

#include "resource.h"

int DirectXInit(HWND hWnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(1);

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = hWnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = Width;
	p_Params.BackBufferHeight = Height;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&p_Params,
		0,
		&p_Device)))
		exit(1);

	Resource my(IDB_PNG1, "PNG");

	D3DXCreateTextureFromFileInMemory(p_Device, my.ptr, my.size_bytes, &tex);
	D3DXCreateSprite(p_Device, &sprite);

#ifdef DEBUG
	cout << "Radar PNG at 0x" << hex << my.ptr << " , its size " << my.size_bytes << endl;
#endif

	return 0;
}


#endif