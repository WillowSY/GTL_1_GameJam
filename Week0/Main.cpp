#include <windows.h>
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#include <d3d11.h>
#include <d3dcompiler.h>

#include <d2d1.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Define.h"
#include "GameMode.h"
#include "Player.h"
#include "CollisionMgr.h"
#include "Ball.h"
#include "SharkShark.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ���� �޽����� ó���� �Լ�
struct FVertexSimple {
	// �� ������ ��ġ ������ ���� ������ �Բ� ����.
	float x, y, z;			// Position(��ġ)
	float r, g, b, a;		// Color (����)
};

FVertexSimple box_vertices[] =
{
	{ -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // �»�
	{  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // ���
	{ -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // ����
	{ -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // ����
	{  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // ���
	{  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }  // ����
};


float GetRandomFloat(float min, float max);

FVector3 MultVector3(FVector3 v1, float f);
float DotProductVector3(FVector3 v1, FVector3 v2);
FVector3 SumVector3(FVector3 v1, FVector3 v2);
FVector3 SubVector3(FVector3 v1, FVector3 v2);
FVector3 DivideVector3(FVector3 v1, float f);
float SqVector3(FVector3 diff);

void HandleCollisions(UBall* headBall, float restitution);

void UpdateMousePosition(HWND hWnd);
FVector3 ComputeRepulsiveForce(UBall* ball, const FVector3& mousePos, float strength);



#include "Sphere.h"
UINT numVerticesSphere = sizeof(sphere_vertices) / sizeof(FVertexSimple);
UINT numVerticesBox = sizeof(box_vertices) / sizeof(FVertexSimple);

/* �ڱ�� ���� ������ */
// ���콺 ��ġ�� ������ ����
FVector3 MousePosition;

// ��� ���
const float kCoulomb = 0.005f;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

class URenderer {
public:
	
	ID3D11Device* Device = nullptr; // GPU�� ����ϱ� ���� Direct3D ��ġ
	ID3D11DeviceContext* DeviceContext = nullptr; // GPU ��� ������ ����ϴ� ���ؽ�Ʈ
	IDXGISwapChain* SwapChain = nullptr; // ������ ���۸� ��ü�ϴ� �� ���Ǵ½��� ü��

	// �������� �ʿ��� ���ҽ� �� ���¸� �����ϱ� ���� ������
	ID3D11Texture2D* FrameBuffer = nullptr;
	//FrameBufferRTv(���� Ÿ�� ��) : FrameBuffer �� ȭ�� ��� ������� �����ϱ� ���� ��ü.
	ID3D11RenderTargetView* FrameBufferRTV = nullptr;
	// RasterizeState : 3D ���� ȭ�鿡 �׸� ��, �ø��̳� ���̾������� ������ �����ϴ� ��ü.
	ID3D11RasterizerState* RasterizerState = nullptr;
	// ��� ���� : ���̴��� �����͸� �����ϴ� ����. -> ���� ���, ���� ����, ī�޶� ��ġ ���� �����͸� ���̴��� �Ѱ��� �� ���.
	ID3D11Buffer* ConstantBuffer = nullptr;

	FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025, 1.0f }; // ȭ���� �ʱ�ȭ(clear) �� �� ����� ����(RGBA)
	D3D11_VIEWPORT ViewportInfo; // ������ ������ �����ϴ� ����Ʈ ����

	ID2D1Factory* pD2DFactory = nullptr;
	ID2D1RenderTarget* pRenderTarget = nullptr;
	IDWriteFactory* pDWriteFactory = nullptr;
	IDWriteTextFormat* pTextFormat = nullptr;
	ID2D1SolidColorBrush* pWhiteBrush = nullptr;

public:
	// ������ �ʱ�ȭ �Լ�
	void Create(HWND hWindow) {
		//Direct3D ��ġ �� ���� ü�� ����
		CreateDeviceAndSwapChain(hWindow);

		// ������ ���� ����
		CreateFrameBuffer();

		// �����Ͷ����� ���� ����
		CreateRasterizerState();

		CreateTextFactory(hWindow);
	}

	//Direct3D ��ġ �� ���� ü���� �����ϴ� �Լ�
	void CreateDeviceAndSwapChain(HWND hWindow) {
		// �����ϴ� Direct3D ��� ������ ����
		D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

		// ���� ü�� ���� ����ü �ʱ�ȭ
		DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
		swapchaindesc.BufferDesc.Width = 0; // â ũ�⿡ �°� �ڵ����� ����
		swapchaindesc.BufferDesc.Height = 0; // â ũ�⿡ �°� �ڵ����� ����
		swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // ���� ����
		swapchaindesc.SampleDesc.Count = 1; // ��Ƽ ���ø� ��Ȱ��ȭ
		swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // ���� Ÿ������ ���
		swapchaindesc.BufferCount = 2; // ���� ���۸�
		swapchaindesc.OutputWindow = hWindow; // �������� â �ڵ�
		swapchaindesc.Windowed = TRUE; // â ���
		swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // ���� ���

		// Direct3D ��ġ�� ���� ü���� ����
		D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
			featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
			&swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);

		// ������ ���� ü���� ���� ��������
		SwapChain->GetDesc(&swapchaindesc);

		// ����Ʈ ���� ����
		ViewportInfo = { 0.0f, 0.0f, (float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height, 0.0f, 1.0f };
	}

	// Direct3D ��ġ �� ���� ü���� �����ϴ� �Լ�
	void ReleaseDeviceAndSwapChain()
	{
		if (DeviceContext)
		{
			DeviceContext->Flush(); // �����ִ� GPU ��� ����
		}

		if (SwapChain)
		{
			SwapChain->Release();
			SwapChain = nullptr;
		}

		if (Device)
		{
			Device->Release();
			Device = nullptr;
		}

		if (DeviceContext)
		{
			DeviceContext->Release();
			DeviceContext = nullptr;
		}
	}

	// ������ ���۸� �����ϴ� �Լ�
	void CreateFrameBuffer()
	{
		// ���� ü�����κ��� �� ���� �ؽ�ó ��������
		SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

		// ���� Ÿ�� �� ����
		D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
		framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // ���� ����
		framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D �ؽ�ó

		Device->CreateRenderTargetView(FrameBuffer, &framebufferRTVdesc, &FrameBufferRTV);
	}

	// ������ ���۸� �����ϴ� �Լ�
	void ReleaseFrameBuffer()
	{
		if (FrameBuffer)
		{
			FrameBuffer->Release();
			FrameBuffer = nullptr;
		}

		if (FrameBufferRTV)
		{
			FrameBufferRTV->Release();
			FrameBufferRTV = nullptr;
		}
	}

	// �����Ͷ����� ���¸� �����ϴ� �Լ�
	void CreateRasterizerState()
	{
		D3D11_RASTERIZER_DESC rasterizerdesc = {};
		rasterizerdesc.FillMode = D3D11_FILL_SOLID; // ä��� ���
		rasterizerdesc.CullMode = D3D11_CULL_BACK; // �� ���̽� �ø�

		Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState);
	}

	// �����Ͷ����� ���¸� �����ϴ� �Լ�
	void ReleaseRasterizerState()
	{
		if (RasterizerState)
		{
			RasterizerState->Release();
			RasterizerState = nullptr;
		}
	}

	void CreateTextFactory(HWND hWindow)
	{
		// Direct2D ���丮 ����
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
		// DWrite ���丮 ����
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));

		// �ؽ�Ʈ ���� ����
		pDWriteFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 32.0f, L"en-us", &pTextFormat);
		//pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		
		// ���� Ÿ�� ����
		IDXGISurface* pDXGISurface;
		SwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&pDXGISurface);

		D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
		pD2DFactory->CreateDxgiSurfaceRenderTarget(pDXGISurface, &props, &pRenderTarget);

		// �귯�� ����
		pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pWhiteBrush);

	}

	void ReleaseTextFactory()
	{
		if (pTextFormat)
		{
			pTextFormat->Release();
			pTextFormat = nullptr;
		}
		if (pWhiteBrush)
		{
			pWhiteBrush->Release();
			pWhiteBrush = nullptr;
		}
		if (pRenderTarget)
		{
			pRenderTarget->Release();
			pRenderTarget = nullptr;
		}
		if (pD2DFactory)
		{
			pD2DFactory->Release();
			pD2DFactory = nullptr;
		}
		if (pDWriteFactory)
		{
			pDWriteFactory->Release();
			pDWriteFactory = nullptr;
		}
	}

	// �������� ���� ��� ���ҽ��� �����ϴ� �Լ�
	void Release()
	{
		RasterizerState->Release();

		// ���� Ÿ���� �ʱ�ȭ
		DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

		ReleaseFrameBuffer();
		ReleaseDeviceAndSwapChain();
		ReleaseTextFactory();
	}

	// ���� ü���� �� ���ۿ� ����Ʈ ���۸� ��ü�Ͽ� ȭ�鿡 ���
	void SwapBuffer()
	{
		SwapChain->Present(1, 0); // 1: VSync Ȱ��ȭ
	}

public:
	// ���� URenderer Classd�� �Ʒ� �ڵ带 �߰� �ϼ���.

	ID3D11VertexShader* SimpleVertexShader;
	ID3D11PixelShader* SimplePixelShader;
	ID3D11InputLayout* SimpleInputLayout;
	unsigned int Stride;
	void CreateShader()
	{
		ID3DBlob* vertexshaderCSO;
		ID3DBlob* pixelshaderCSO;

		D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);

		Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &SimpleVertexShader);

		D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);

		Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &SimplePixelShader);

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &SimpleInputLayout);

		Stride = sizeof(FVertexSimple);

		vertexshaderCSO->Release();
		pixelshaderCSO->Release();
	}

	void ReleaseShader()
	{
		if (SimpleInputLayout)
		{
			SimpleInputLayout->Release();
			SimpleInputLayout = nullptr;
		}

		if (SimplePixelShader)
		{
			SimplePixelShader->Release();
			SimplePixelShader = nullptr;
		}

		if (SimpleVertexShader)
		{
			SimpleVertexShader->Release();
			SimpleVertexShader = nullptr;
		}
	}

	void Prepare()
	{
		DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);

		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DeviceContext->RSSetViewports(1, &ViewportInfo);
		DeviceContext->RSSetState(RasterizerState);

		DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, nullptr);
		DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	}

	// ���� �����͸� GPU�� �����ϰ�, ������ ȭ�鿡 �׸���(Render)�Լ�.
	void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices)
	{
		
		UINT offset = 0;
		DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);
		DeviceContext->Draw(numVertices, 0);

	}
	void RenderSpheres(ID3D11Buffer* pBuffer, UINT numVertices, int sphereCount)
	{
		UINT offset = 0;


		for (int i = 0; i < sphereCount; ++i) {
			DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);
			DeviceContext->Draw(numVertices, i * numVertices);
		}

	}

	ID3D11Buffer* CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth)
	{
		D3D11_BUFFER_DESC vertexbufferdesc = {};
		vertexbufferdesc.ByteWidth = byteWidth;
		vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated 
		vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexbufferSRD = { vertices };

		ID3D11Buffer* vertexBuffer;

		Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);

		return vertexBuffer;
	}

	void ReleaseVertexBuffer(ID3D11Buffer* vertexBuffer)
	{
		vertexBuffer->Release();
	}

	struct FConstants
	{
		FVector3 Offset;
		float Radius;
		float Pad;
	};

	void CreateConstantBuffer()
	{
		D3D11_BUFFER_DESC constantbufferdesc = {};
		constantbufferdesc.ByteWidth = sizeof(FConstants) + 0xf & 0xfffffff0;
		constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
		constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		Device->CreateBuffer(&constantbufferdesc, nullptr, &ConstantBuffer);
	}

	void ReleaseConstantBuffer()
	{
		if (ConstantBuffer)
		{
			ConstantBuffer->Release();
			ConstantBuffer = nullptr;
		}
	}
	void UpdateConstant(FVector3 Offset, float Radius)
	{
		if (ConstantBuffer)
		{
			D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

			DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR);
			FConstants* constants = (FConstants*)constantbufferMSR.pData;
			{
				constants->Offset = Offset;
				constants->Radius = Radius;
			}
			DeviceContext->Unmap(ConstantBuffer, 0);
		}
	}

	void PrepareShader()
	{
		DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
		DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
		DeviceContext->IASetInputLayout(SimpleInputLayout);

		if (ConstantBuffer)
		{
			DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
		}
	}

	void RenderText(const wchar_t* text, float x, float y, float width, float height)
	{
		pRenderTarget->BeginDraw();
		//������ ��ĥ�ϸ� �����ϴ�!!
		//pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.f));

		pRenderTarget->DrawText(
			text,
			wcslen(text),
			pTextFormat,
			D2D1::RectF(x, y, x + width, y + height),
			pWhiteBrush
		);

		pRenderTarget->EndDraw();
	}
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	
	WCHAR WindowClass[] = L"JungleWindowClass";
	WCHAR Title[] = L"Game Tech Lab";
	WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };

	if (!RegisterClassW(&wndclass)) {
		MessageBoxW(nullptr, L"������ Ŭ���� ��� ����!", L"����", MB_OK | MB_ICONERROR);
		return -1;
	}
	HWND hWnd = CreateWindowExW(0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1024, 1024, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) {
		MessageBoxW(nullptr, L"������ ���� ����!", L"����", MB_OK | MB_ICONERROR);
		return -1;
	}

	// Renderer �� Direct3D ���� �ʱ�ȭ
	URenderer renderer;

	// D3D11 ���� �Լ� ȣ��.
	renderer.Create(hWnd);
	renderer.CreateShader();
	renderer.CreateConstantBuffer();

	// �� ���� ���� 1ȸ ����
	ID3D11Buffer* vertexBufferSphere = renderer.CreateVertexBuffer(sphere_vertices, numVerticesSphere * sizeof(FVertexSimple));

	ID3D11Buffer* vertexBufferBox = renderer.CreateVertexBuffer(box_vertices, numVerticesBox * sizeof(FVertexSimple));


	// IMGUI �ʱ�ȭ
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(renderer.Device, renderer.DeviceContext);

#pragma region Seyoungs
	/* Ball ���� ������ */
// ��ũ�帮��Ʈ ��� ��
//UBall* HeadBall = nullptr;    

//static int targetBallCount = 0;
//// �� ���� ��� �� ��� ���
//static int k = 1;

//// �����ϴ� ���� ������ ��ȸ�ϸ� ī��Ʈ.
//auto CountBalls = [&HeadBall]() -> int {
//	int count = 0;
//	for (UBall* cur = HeadBall; cur != nullptr; cur = cur->NextBall)
//		count++;
//	return count;
//	};

//// �� �߰�
//auto AddBall = [&HeadBall]() {
//	UBall* newBall = new UBall();
//	
//	newBall->Radius = GetRandomFloat(0.05f, 0.2f);
//	newBall->Location = FVector3(
//		GetRandomFloat(-1.0f, 1.0f),
//		GetRandomFloat(-0.5f, 0.5f),
//		0.0f
//	);
//	newBall->Velocity = FVector3(0, 0, 0);

//	newBall->Mass = newBall->Radius * newBall->Radius * newBall->Radius * k;

//	newBall->NextBall = HeadBall;
//	HeadBall = newBall;
//	};

//// �� ����.
//auto RemoveRandomBall = [&HeadBall, &CountBalls]() {
//	int count = CountBalls();
//	if (count == 0) return;
//	// 0 ~ count-1 ������ ���� �ε��� ����
//	int index = static_cast<int>(GetRandomFloat(0, static_cast<float>(count -1)));
//	UBall* current = HeadBall;
//	UBall* prev = nullptr;
//	for (int i = 0; i < index; i++) {
//		prev = current;
//		current = current->NextBall;
//	}
//	if (prev == nullptr) {  // HeadBall�� ����
//		HeadBall = current->NextBall;
//	}
//	else {
//		prev->NextBall = current->NextBall;
//	}
//	delete current;
//	};

// ȭ�� ��� ���� ����.
//const float leftBorder = -1.0f;
//const float rightBorder = 1.0f;
//const float topBorder = -1.0f;
//const float bottomBorder = 1.0f;
//bool bBoundBallToScreen = true;
//bool bPinballMovement = true;
//bool bApplyGravity = true;
//���ӵ� �̿�
//bool bApplyAngularVelocity = false;
//bool bMagnetic = false;

	//const float GravityAcceleration = -0.005f;
	//float e = 0.8f;
#pragma endregion

	SharkShark* pMainGame = new SharkShark;
	pMainGame->Initialize();

	CGameMode* gameMode = new CGameMode();
	gameMode->Initialize();
	//UPlayer* pPlayer = new UPlayer;

	UBall* HeadBall = new UBall;
	//HeadBall->CreateBall();
	static int numBalls = 0;  // ���� ���� �ʱⰪ

	const int targetFPS = 60;
	const double targetFrameTime = 1000.0 / targetFPS; // �� �������� ��ǥ �ð� (�и��� ����)
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER startTime, endTime;
	double elapsedTime = 1.0;
	bool bIsExit = false;

	while (bIsExit == false)
	{
		QueryPerformanceCounter(&startTime);

		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); // Ű���� �Է� �޽����� ���ڸ޽����� ����
			DispatchMessage(&msg); // �޽����� WndProc�� ����

			if (msg.message == WM_QUIT)
			{
				bIsExit = true;
				break;
			}

		}
		UpdateMousePosition(hWnd);

		UPlayer* player = (UPlayer*)pMainGame->GetPlayer();
		gameMode->bGameOver = player->IsDead();
		if (gameMode->bHasInit) gameMode->bStageClear = numBalls == 0;
		if (!gameMode->bHasInit)
		{
			/* �÷��̾� */
			gameMode->bGameOver ? player->Initialize() : player->Reposition();
			/* ���� */
			// LevelLoader
			/* �� (UBall) */
			numBalls = gameMode->stage;
			while (numBalls > pMainGame->GetpObejectList()[OL_BALL].size())
			{
				pMainGame->CreateBall();
			}
			while (numBalls < pMainGame->GetpObejectList()[OL_BALL].size())
			{
				pMainGame->DeleteRandomBall(numBalls);
			}
			gameMode->bHasInit = true;
		}

		gameMode->Update(elapsedTime);
		pMainGame->Update(elapsedTime);
		pMainGame->FixedUpdate();
		numBalls = pMainGame->GetBallList().size();

		// �غ� �۾�
		renderer.Prepare();
		renderer.PrepareShader();


		// ball Rendering
		for (auto iter = pMainGame->GetBallList().begin(); iter != pMainGame->GetBallList().end(); iter++)
		{
			renderer.UpdateConstant(static_cast<UBall*>(*iter)->Location, static_cast<UBall*>(*iter)->Radius);
			renderer.RenderPrimitive(vertexBufferSphere, numVerticesSphere);
		}
		
		//Player Rendering
		renderer.UpdateConstant(static_cast<UPlayer*>(pMainGame->GetPlayer())->GetLoc(), static_cast<UPlayer*>(pMainGame->GetPlayer())->GetScale());
		renderer.RenderPrimitive(vertexBufferBox, numVerticesBox);
		
		// �ؽ�Ʈ ������
		renderer.RenderText(L"Shark, Shark", 8, 8, 400, 200);
		wchar_t numBallsText[50];
		swprintf(numBallsText, 50, L"Number of Balls: %d", numBalls);
		renderer.RenderText(numBallsText, 8, 48, 400, 200);
		wchar_t stageText[50];
		swprintf(stageText, 50, L"Stage: %d", gameMode->stage);
		renderer.RenderText(stageText, 8, 88, 400, 200);
		wchar_t scoreText[50];
		swprintf(scoreText, 50, L"Score: %d", gameMode->score);
		renderer.RenderText(scoreText, 8, 128, 400, 200);

		if (gameMode->bGameOver)
		{
			renderer.RenderText(L"Game Over",	512 - 70,	424, 400, 200);
			renderer.RenderText(L"Press 'R'",	460,		464, 400, 200);
			renderer.RenderText(scoreText,		460,		512, 400, 200);
		}
			

		// ImGui ������
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// ���� ImGui UI ��Ʈ�� �߰��� ImGui::NewFrame()�� ImGui::Render() ������ ���⿡ ��ġ�մϴ�.
		ImGui::Begin("Jungle Property Window");

		ImGui::Text("Hello Jungle World!");

		ImGui::Text("%d", pMainGame->GetBallList().size());


		ImGui::PushItemWidth(80);
		ImGui::InputInt("##balls", &numBalls);
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::Text("Number of Balls");

		ImGui::End();
		/////////////////////////////////////////////////////////////////////////
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		//���� ��ü
		renderer.SwapBuffer();
		do
		{
			Sleep(0);

			// ���� ���� �ð� ���
			QueryPerformanceCounter(&endTime);

			// �� �������� �ҿ�� �ð� ��� (�и��� ������ ��ȯ)
			elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;

		} while (elapsedTime < targetFrameTime);
	}
#pragma region Seyoungs
	// ������ ���� ����.
//const int targetFPS = 60;
//const double targetFrameTime = 1000.0 / targetFPS;
//LARGE_INTEGER startTime, endTime, frequency;
//QueryPerformanceFrequency(&frequency);
//double elapsedTime = 0.0;
//bool bIsExit = false;

//// ������ ����.
//while (!bIsExit) {
//	QueryPerformanceCounter(&startTime);

//	// �޽��� ó��
//	MSG msg;
//	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//		if (msg.message == WM_QUIT) {
//			bIsExit = true;
//			break;
//		}
//	}

//	// ���콺 ��ġ ������Ʈ
//	UpdateMousePosition(hWnd);

//	if (bPinballMovement) {
//		for (UBall* curBall = HeadBall; curBall != nullptr; curBall = curBall->NextBall) {
//			if (bMagnetic) {
//				// ô�� ��� �� ����
//				FVector3 repulsiveForce = ComputeRepulsiveForce(curBall, MousePosition, 0.01f);
//				curBall->Velocity = SumVector3(curBall->Velocity, repulsiveForce);
//			}
//			curBall->Location.x += curBall->Velocity.x;
//			curBall->Location.y += curBall->Velocity.y;
//			curBall->Location.z += curBall->Velocity.z;

//			if (bApplyGravity) {
//				curBall->Velocity.y += GravityAcceleration;
//			}

//			// �ʹ� ������ �������� ������ ���� ����
//			if (curBall->Location.y >= bottomBorder - curBall->Radius && fabs(curBall->Velocity.y) < 0.01f) {
//				curBall->Velocity.y = 0.0f;
//			}

//			if (curBall->Location.y <= topBorder + curBall->Radius) {
//				curBall->Location.y = topBorder + curBall->Radius;

//				if (curBall->Velocity.y < 0) {
//					curBall->Velocity.y *= -e;
//				}

//				//if (fabs(curBall->Velocity.y) < 0.01f) {
//				//	curBall->Velocity.y = 0.01f;
//				//}
//			}

//			if (curBall->Location.x < leftBorder + curBall->Radius) {
//				curBall->Location.x = leftBorder + curBall->Radius;
//				curBall->Velocity.x *= -e;
//				
//				//���ӵ� �̿�
//				//curBall->AngularVelocity.z += curBall->Velocity.y * 0.1f;
//			}
//			else if (curBall->Location.x > rightBorder - curBall->Radius) {
//				curBall->Location.x = rightBorder - curBall->Radius;
//				curBall->Velocity.x *= -e;

//				//���ӵ� �̿�
//				//curBall->AngularVelocity.z -= curBall->Velocity.y * 0.1f;
//			}

//			// ���� �� �浹 (Y�� ����)
//			if (curBall->Location.y < topBorder + curBall->Radius) {
//				curBall->Location.y = topBorder + curBall->Radius;
//				curBall->Velocity.y *= -e;

//				//���ӵ� �̿�
//				//curBall->AngularVelocity.x += curBall->Velocity.x * 0.1f;
//			}
//			else if (curBall->Location.y > bottomBorder - curBall->Radius) {
//				curBall->Location.y = bottomBorder - curBall->Radius;
//				curBall->Velocity.y *= -e;

//				//���ӵ� �̿�
//				//curBall->AngularVelocity.x -= curBall->Velocity.x * 0.1f;
//			}

//			//���ӵ� �̿�
//			/*if (bApplyAngularVelocity) {
//				curBall->Velocity.x += curBall->AngularVelocity.y * 0.01f;
//				curBall->Velocity.y += curBall->AngularVelocity.x * 0.01f;

//				curBall->AngularVelocity = MultVector3(curBall->AngularVelocity, 0.98f);
//			}*/
//		}
//	}
//	UBall* pBall = HeadBall;
//	while (pBall != nullptr)
//	{
//		CollisionMgr::CollisionPlayerAndBall(pPlayer, pBall);
//		pBall = pBall->NextBall;
//	}
//	pPlayer->Update(elapsedTime);

//	HandleCollisions(HeadBall, e);

//	// ������ �غ� (������ Ŭ����, ����Ʈ ���� ��)
//	renderer.Prepare();
//	renderer.PrepareShader();

//	// ImGui �ʱ�ȭ
//	
//	ImGui_ImplDX11_NewFrame();
//	ImGui_ImplWin32_NewFrame();
//	ImGui::NewFrame();
//	ImGui::Begin("Jungle Property Window");
//	ImGui::Text("Hello Jungle World!");
//	ImGui::SliderFloat("COR", &e, 0.0f, 1.0f);
//	ImGui::Checkbox("Gravity", &bApplyGravity);
//	//���ӵ� �̿�
//	//ImGui::Checkbox("Angular Velocity", &bApplyAngularVelocity);
//	ImGui::Checkbox("Magnetic Repulsion", &bMagnetic);

//	// �� ���� ī����
//	ImGui::InputInt("Number of Balls", &targetBallCount);
//	if (targetBallCount < 0)
//		targetBallCount = 0;
//	

//	// �� ������ ���� ����.
//	int currentCount = CountBalls();
//	while (currentCount < targetBallCount) {
//		AddBall();
//		currentCount++;
//	}
//	while (currentCount > targetBallCount) {
//		RemoveRandomBall();
//		currentCount--;
//	}

//	// �� ���� ������
//	for (UBall* curBall = HeadBall; curBall != nullptr; curBall = curBall->NextBall) {
//		renderer.UpdateConstant(curBall->Location, curBall->Radius);
//		renderer.RenderPrimitive(vertexBufferSphere, numVerticesSphere);
//	}

//	//Player Rendering
//	renderer.UpdateConstant(pPlayer->GetLoc(), pPlayer->GetScale());
//	renderer.RenderPrimitive(vertexBufferBox, numVerticesBox);

//	// Player Rendering ����
//	ImGui::End();
//	ImGui::Render();
//	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

//	// ������ ���� ����
//	do {
//		Sleep(0);
//		QueryPerformanceCounter(&endTime);
//		elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;
//	} while (elapsedTime < targetFrameTime);
//	
//	renderer.SwapBuffer();
//}

#pragma endregion


	// �ڿ����� �� ����.
	while (HeadBall) {
		UBall* temp = HeadBall;
		HeadBall = HeadBall->NextBall;
		delete temp;
	}
	//delete pPlayer;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	renderer.ReleaseVertexBuffer(vertexBufferSphere);
	renderer.ReleaseConstantBuffer();
	renderer.ReleaseShader();
	renderer.Release();

	return 0;
}

float GetRandomFloat(float min, float max) {
	return min + (rand() / (float)RAND_MAX) * (max - min);
}

void HandleCollisions(UBall* headBall, float e = 0.5f) {
	for (UBall* b1 = headBall; b1 != nullptr; b1 = b1->NextBall) {
		for (UBall* b2 = b1->NextBall; b2 != nullptr; b2 = b2->NextBall) {
			// �� �� �� �Ÿ� ���
			FVector3 diff = SubVector3(b2->Location, b1->Location);
			float distanceSq = SqVector3(diff);
			float radiusSum = b1->Radius + b2->Radius;

			if (distanceSq < radiusSum * radiusSum) {
				float distance = sqrt(distanceSq);
				if (distance == 0.0f) distance = 0.001f;

				// �浹 ���� ����ȭ
				FVector3 normal = DivideVector3(diff, distance);

				// ��� �ӵ� ����
				FVector3 relativeVelocity = SubVector3(b2->Velocity, b1->Velocity);

				// �浹 ���������� ��� �ӵ� ũ��
				float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;
				if (velocityAlongNormal > 0) continue;
				
				// ��� ���� ��Ģ���� �浹 �� �ӵ� ���
				float m1 = b1->Mass;
				float m2 = b2->Mass;

				float impulseMagnitude = (-(1+e) * velocityAlongNormal) / (1 / m1 + 1 / m2);

				FVector3 impulse = MultVector3(normal, impulseMagnitude);


				b1->Velocity = SubVector3(b1->Velocity, MultVector3(impulse, 1/m1));
				b2->Velocity = SumVector3(b2->Velocity, MultVector3(impulse, 1/m2));

				// �浹 �� ��ħ ����
				float penetrationDepth = radiusSum - distance;
				FVector3 correction = MultVector3(normal, penetrationDepth / 2.0f);
				b1->Location = SubVector3(b1->Location, correction);
				b2->Location = SumVector3(b2->Location, correction);

				//���ӵ� �̿�
				//FVector3 tangential = { -normal.y, normal.x, 0 }; // ���� ����
				//float spinForce = DotProductVector3(relativeVelocity, tangential);
				
				//���ӵ� �̿�
				/*b1->AngularVelocity = SumVector3(b1->AngularVelocity, MultVector3(tangential, spinForce * 0.1f));
				b2->AngularVelocity = SubVector3(b2->AngularVelocity, MultVector3(tangential, spinForce * 0.1f));*/
			}
		}
	}
}

FVector3 MultVector3(FVector3 v1, float f) {
	return FVector3(v1.x * f, v1.y * f, v1.z * f);
}
float DotProductVector3(FVector3 v1, FVector3 v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

FVector3 SumVector3(FVector3 v1, FVector3 v2) {
	return FVector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
FVector3 SubVector3(FVector3 v1, FVector3 v2){
	float x = v1.x - v2.x;
	float y = v1.y - v2.y;
	float z = v1.z - v2.z;
	return FVector3(x, y, z);
}

FVector3 DivideVector3(FVector3 v1, float f) {
	return FVector3(v1.x / f, v1.y / f, v1.z / f);
}

float SqVector3(FVector3 diff) {
	return diff.x * diff.x + diff.y * diff.y+diff.z * diff.z;
}

// ���콺 ��ġ ������Ʈ �Լ�
void UpdateMousePosition(HWND hWnd) {
	POINT p;
	if (GetCursorPos(&p)) {
		ScreenToClient(hWnd, &p);
		RECT rect;
		GetClientRect(hWnd, &rect);
		float width = rect.right - rect.left;
		float height = rect.bottom - rect.top;
		MousePosition.x = (p.x / width) * 2.0f - 1.0f;
		MousePosition.y = 1.0f - (p.y / height) * 2.0f;
		MousePosition.z = 0.0f;
	}
}

// ô�� ��� �Լ�
FVector3 ComputeRepulsiveForce(UBall* ball, const FVector3& mousePos, float strength = 1.0f) {
	FVector3 direction = SubVector3(ball->Location, mousePos);
	float distanceSq = SqVector3(direction);

	if (distanceSq < 0.0001f) return FVector3(0, 0, 0);

	float distance = sqrt(distanceSq);

	float chargeBall = ball->Radius;
	float chargeMouse = 0.1f;

	float forceMagnitude = kCoulomb * (chargeBall * chargeMouse) / distanceSq;

	FVector3 repulsiveForce = MultVector3(DivideVector3(direction, distance), forceMagnitude);
	return repulsiveForce;
}
