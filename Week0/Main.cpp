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

// 각종 메시지를 처리할 함수
struct FVertexSimple {
	// 각 정점에 위치 정보와 색상 정보가 함께 저장.
	float x, y, z;			// Position(위치)
	float r, g, b, a;		// Color (색상)
};

FVertexSimple box_vertices[] =
{
	{ -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // 좌상
	{  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // 우상
	{ -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // 좌하
	{ -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // 좌하
	{  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }, // 우상
	{  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }  // 우하
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

/* 자기력 관련 변수들 */
// 마우스 위치를 저장할 변수
FVector3 MousePosition;

// 쿨롱 상수
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
	
	ID3D11Device* Device = nullptr; // GPU와 통신하기 위한 Direct3D 장치
	ID3D11DeviceContext* DeviceContext = nullptr; // GPU 명령 실행을 담당하는 컨텍스트
	IDXGISwapChain* SwapChain = nullptr; // 프레임 버퍼를 교체하는 데 사용되는스왑 체인

	// 렌더링에 필요한 리소스 및 상태를 관리하기 위한 변수들
	ID3D11Texture2D* FrameBuffer = nullptr;
	//FrameBufferRTv(렌더 타겟 뷰) : FrameBuffer 를 화면 출력 대상으로 설정하기 위한 객체.
	ID3D11RenderTargetView* FrameBufferRTV = nullptr;
	// RasterizeState : 3D 모델을 화면에 그릴 때, 컬링이나 와이어프레임 설정을 관리하는 객체.
	ID3D11RasterizerState* RasterizerState = nullptr;
	// 상수 버퍼 : 쉐이더에 데이터를 전달하는 역할. -> 예를 들어, 빛의 방향, 카메라 위치 같은 데이터를 쉐이더에 넘겨줄 때 사용.
	ID3D11Buffer* ConstantBuffer = nullptr;

	FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025, 1.0f }; // 화면을 초기화(clear) 할 때 사용할 색상(RGBA)
	D3D11_VIEWPORT ViewportInfo; // 렌더링 영역을 정의하는 뷰포트 정보

	ID2D1Factory* pD2DFactory = nullptr;
	ID2D1RenderTarget* pRenderTarget = nullptr;
	IDWriteFactory* pDWriteFactory = nullptr;
	IDWriteTextFormat* pTextFormat = nullptr;
	ID2D1SolidColorBrush* pWhiteBrush = nullptr;

public:
	// 렌더러 초기화 함수
	void Create(HWND hWindow) {
		//Direct3D 장치 및 스왑 체인 생성
		CreateDeviceAndSwapChain(hWindow);

		// 프레임 버퍼 생성
		CreateFrameBuffer();

		// 래스터라이저 상태 생성
		CreateRasterizerState();

		CreateTextFactory(hWindow);
	}

	//Direct3D 장치 및 스왑 체인을 생성하는 함수
	void CreateDeviceAndSwapChain(HWND hWindow) {
		// 지원하는 Direct3D 기능 레벨을 정의
		D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

		// 스왑 체인 설정 구조체 초기화
		DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
		swapchaindesc.BufferDesc.Width = 0; // 창 크기에 맞게 자동으로 설정
		swapchaindesc.BufferDesc.Height = 0; // 창 크기에 맞게 자동으로 설정
		swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // 색상 포맷
		swapchaindesc.SampleDesc.Count = 1; // 멀티 샘플링 비활성화
		swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 렌더 타겟으로 사용
		swapchaindesc.BufferCount = 2; // 더블 버퍼링
		swapchaindesc.OutputWindow = hWindow; // 렌더링할 창 핸들
		swapchaindesc.Windowed = TRUE; // 창 모드
		swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 스왑 방식

		// Direct3D 장치와 스왑 체인을 생성
		D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
			featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
			&swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);

		// 생성된 스왑 체인의 정보 가져오기
		SwapChain->GetDesc(&swapchaindesc);

		// 뷰포트 정보 설정
		ViewportInfo = { 0.0f, 0.0f, (float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height, 0.0f, 1.0f };
	}

	// Direct3D 장치 및 스왑 체인을 해제하는 함수
	void ReleaseDeviceAndSwapChain()
	{
		if (DeviceContext)
		{
			DeviceContext->Flush(); // 남아있는 GPU 명령 실행
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

	// 프레임 버퍼를 생성하는 함수
	void CreateFrameBuffer()
	{
		// 스왑 체인으로부터 백 버퍼 텍스처 가져오기
		SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

		// 렌더 타겟 뷰 생성
		D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
		framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // 색상 포맷
		framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처

		Device->CreateRenderTargetView(FrameBuffer, &framebufferRTVdesc, &FrameBufferRTV);
	}

	// 프레임 버퍼를 해제하는 함수
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

	// 래스터라이저 상태를 생성하는 함수
	void CreateRasterizerState()
	{
		D3D11_RASTERIZER_DESC rasterizerdesc = {};
		rasterizerdesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드
		rasterizerdesc.CullMode = D3D11_CULL_BACK; // 백 페이스 컬링

		Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState);
	}

	// 래스터라이저 상태를 해제하는 함수
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
		// Direct2D 팩토리 생성
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
		// DWrite 팩토리 생성
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));

		// 텍스트 포맷 생성
		pDWriteFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 32.0f, L"en-us", &pTextFormat);
		//pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		
		// 렌더 타겟 생성
		IDXGISurface* pDXGISurface;
		SwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&pDXGISurface);

		D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
		pD2DFactory->CreateDxgiSurfaceRenderTarget(pDXGISurface, &props, &pRenderTarget);

		// 브러시 생성
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

	// 렌더러에 사용된 모든 리소스를 해제하는 함수
	void Release()
	{
		RasterizerState->Release();

		// 렌더 타겟을 초기화
		DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

		ReleaseFrameBuffer();
		ReleaseDeviceAndSwapChain();
		ReleaseTextFactory();
	}

	// 스왑 체인의 백 버퍼와 프론트 버퍼를 교체하여 화면에 출력
	void SwapBuffer()
	{
		SwapChain->Present(1, 0); // 1: VSync 활성화
	}

public:
	// 기존 URenderer Classd에 아래 코드를 추가 하세요.

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

	// 정점 데이터를 GPU에 전달하고, 실제로 화면에 그리는(Render)함수.
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
		//배경색을 안칠하면 투명하다!!
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
		MessageBoxW(nullptr, L"윈도우 클래스 등록 실패!", L"오류", MB_OK | MB_ICONERROR);
		return -1;
	}
	HWND hWnd = CreateWindowExW(0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1024, 1024, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) {
		MessageBoxW(nullptr, L"윈도우 생성 실패!", L"오류", MB_OK | MB_ICONERROR);
		return -1;
	}

	// Renderer 및 Direct3D 관련 초기화
	URenderer renderer;

	// D3D11 생성 함수 호출.
	renderer.Create(hWnd);
	renderer.CreateShader();
	renderer.CreateConstantBuffer();

	// 구 정점 버퍼 1회 생성
	ID3D11Buffer* vertexBufferSphere = renderer.CreateVertexBuffer(sphere_vertices, numVerticesSphere * sizeof(FVertexSimple));

	ID3D11Buffer* vertexBufferBox = renderer.CreateVertexBuffer(box_vertices, numVerticesBox * sizeof(FVertexSimple));


	// IMGUI 초기화
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(renderer.Device, renderer.DeviceContext);

#pragma region Seyoungs
	/* Ball 관련 변수들 */
// 링크드리스트 헤더 볼
//UBall* HeadBall = nullptr;    

//static int targetBallCount = 0;
//// 구 질량 계산 시 비례 상수
//static int k = 1;

//// 존재하는 볼의 개수를 순회하며 카운트.
//auto CountBalls = [&HeadBall]() -> int {
//	int count = 0;
//	for (UBall* cur = HeadBall; cur != nullptr; cur = cur->NextBall)
//		count++;
//	return count;
//	};

//// 볼 추가
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

//// 볼 제거.
//auto RemoveRandomBall = [&HeadBall, &CountBalls]() {
//	int count = CountBalls();
//	if (count == 0) return;
//	// 0 ~ count-1 사이의 임의 인덱스 선택
//	int index = static_cast<int>(GetRandomFloat(0, static_cast<float>(count -1)));
//	UBall* current = HeadBall;
//	UBall* prev = nullptr;
//	for (int i = 0; i < index; i++) {
//		prev = current;
//		current = current->NextBall;
//	}
//	if (prev == nullptr) {  // HeadBall를 삭제
//		HeadBall = current->NextBall;
//	}
//	else {
//		prev->NextBall = current->NextBall;
//	}
//	delete current;
//	};

// 화면 경계 관련 변수.
//const float leftBorder = -1.0f;
//const float rightBorder = 1.0f;
//const float topBorder = -1.0f;
//const float bottomBorder = 1.0f;
//bool bBoundBallToScreen = true;
//bool bPinballMovement = true;
//bool bApplyGravity = true;
//각속도 미완
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
	static int numBalls = 0;  // 공의 개수 초기값

	const int targetFPS = 60;
	const double targetFrameTime = 1000.0 / targetFPS; // 한 프레임의 목표 시간 (밀리초 단위)
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
			TranslateMessage(&msg); // 키보드 입력 메시지를 문자메시지로 변경
			DispatchMessage(&msg); // 메시지를 WndProc에 전달

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
			/* 플레이어 */
			gameMode->bGameOver ? player->Initialize() : player->Reposition();
			/* 지형 */
			// LevelLoader
			/* 적 (UBall) */
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

		// 준비 작업
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
		
		// 텍스트 렌더링
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
			

		// ImGui 렌더링
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// 이후 ImGui UI 컨트롤 추가는 ImGui::NewFrame()과 ImGui::Render() 사이인 여기에 위치합니다.
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

		//버퍼 교체
		renderer.SwapBuffer();
		do
		{
			Sleep(0);

			// 루프 종료 시간 기록
			QueryPerformanceCounter(&endTime);

			// 한 프레임이 소요된 시간 계산 (밀리초 단위로 변환)
			elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;

		} while (elapsedTime < targetFrameTime);
	}
#pragma region Seyoungs
	// 프레임 관련 변수.
//const int targetFPS = 60;
//const double targetFrameTime = 1000.0 / targetFPS;
//LARGE_INTEGER startTime, endTime, frequency;
//QueryPerformanceFrequency(&frequency);
//double elapsedTime = 0.0;
//bool bIsExit = false;

//// 렌더링 루프.
//while (!bIsExit) {
//	QueryPerformanceCounter(&startTime);

//	// 메시지 처리
//	MSG msg;
//	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//		if (msg.message == WM_QUIT) {
//			bIsExit = true;
//			break;
//		}
//	}

//	// 마우스 위치 업데이트
//	UpdateMousePosition(hWnd);

//	if (bPinballMovement) {
//		for (UBall* curBall = HeadBall; curBall != nullptr; curBall = curBall->NextBall) {
//			if (bMagnetic) {
//				// 척력 계산 및 적용
//				FVector3 repulsiveForce = ComputeRepulsiveForce(curBall, MousePosition, 0.01f);
//				curBall->Velocity = SumVector3(curBall->Velocity, repulsiveForce);
//			}
//			curBall->Location.x += curBall->Velocity.x;
//			curBall->Location.y += curBall->Velocity.y;
//			curBall->Location.z += curBall->Velocity.z;

//			if (bApplyGravity) {
//				curBall->Velocity.y += GravityAcceleration;
//			}

//			// 너무 느리게 움직여서 떨리는 문제 방지
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
//				//각속도 미완
//				//curBall->AngularVelocity.z += curBall->Velocity.y * 0.1f;
//			}
//			else if (curBall->Location.x > rightBorder - curBall->Radius) {
//				curBall->Location.x = rightBorder - curBall->Radius;
//				curBall->Velocity.x *= -e;

//				//각속도 미완
//				//curBall->AngularVelocity.z -= curBall->Velocity.y * 0.1f;
//			}

//			// 상하 벽 충돌 (Y축 방향)
//			if (curBall->Location.y < topBorder + curBall->Radius) {
//				curBall->Location.y = topBorder + curBall->Radius;
//				curBall->Velocity.y *= -e;

//				//각속도 미완
//				//curBall->AngularVelocity.x += curBall->Velocity.x * 0.1f;
//			}
//			else if (curBall->Location.y > bottomBorder - curBall->Radius) {
//				curBall->Location.y = bottomBorder - curBall->Radius;
//				curBall->Velocity.y *= -e;

//				//각속도 미완
//				//curBall->AngularVelocity.x -= curBall->Velocity.x * 0.1f;
//			}

//			//각속도 미완
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

//	// 렌더링 준비 (프레임 클리어, 뷰포트 설정 등)
//	renderer.Prepare();
//	renderer.PrepareShader();

//	// ImGui 초기화
//	
//	ImGui_ImplDX11_NewFrame();
//	ImGui_ImplWin32_NewFrame();
//	ImGui::NewFrame();
//	ImGui::Begin("Jungle Property Window");
//	ImGui::Text("Hello Jungle World!");
//	ImGui::SliderFloat("COR", &e, 0.0f, 1.0f);
//	ImGui::Checkbox("Gravity", &bApplyGravity);
//	//각속도 미완
//	//ImGui::Checkbox("Angular Velocity", &bApplyAngularVelocity);
//	ImGui::Checkbox("Magnetic Repulsion", &bMagnetic);

//	// 공 개수 카운팅
//	ImGui::InputInt("Number of Balls", &targetBallCount);
//	if (targetBallCount < 0)
//		targetBallCount = 0;
//	

//	// 공 개수에 따라 조절.
//	int currentCount = CountBalls();
//	while (currentCount < targetBallCount) {
//		AddBall();
//		currentCount++;
//	}
//	while (currentCount > targetBallCount) {
//		RemoveRandomBall();
//		currentCount--;
//	}

//	// 각 볼들 렌더링
//	for (UBall* curBall = HeadBall; curBall != nullptr; curBall = curBall->NextBall) {
//		renderer.UpdateConstant(curBall->Location, curBall->Radius);
//		renderer.RenderPrimitive(vertexBufferSphere, numVerticesSphere);
//	}

//	//Player Rendering
//	renderer.UpdateConstant(pPlayer->GetLoc(), pPlayer->GetScale());
//	renderer.RenderPrimitive(vertexBufferBox, numVerticesBox);

//	// Player Rendering 종료
//	ImGui::End();
//	ImGui::Render();
//	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

//	// 프레임 조절 관련
//	do {
//		Sleep(0);
//		QueryPerformanceCounter(&endTime);
//		elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;
//	} while (elapsedTime < targetFrameTime);
//	
//	renderer.SwapBuffer();
//}

#pragma endregion


	// 자원해제 및 종료.
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
			// 두 공 간 거리 계산
			FVector3 diff = SubVector3(b2->Location, b1->Location);
			float distanceSq = SqVector3(diff);
			float radiusSum = b1->Radius + b2->Radius;

			if (distanceSq < radiusSum * radiusSum) {
				float distance = sqrt(distanceSq);
				if (distance == 0.0f) distance = 0.001f;

				// 충돌 벡터 정규화
				FVector3 normal = DivideVector3(diff, distance);

				// 상대 속도 벡터
				FVector3 relativeVelocity = SubVector3(b2->Velocity, b1->Velocity);

				// 충돌 방향으로의 상대 속도 크기
				float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;
				if (velocityAlongNormal > 0) continue;
				
				// 운동량 보존 법칙으로 충돌 후 속도 계산
				float m1 = b1->Mass;
				float m2 = b2->Mass;

				float impulseMagnitude = (-(1+e) * velocityAlongNormal) / (1 / m1 + 1 / m2);

				FVector3 impulse = MultVector3(normal, impulseMagnitude);


				b1->Velocity = SubVector3(b1->Velocity, MultVector3(impulse, 1/m1));
				b2->Velocity = SumVector3(b2->Velocity, MultVector3(impulse, 1/m2));

				// 충돌 후 겹침 방지
				float penetrationDepth = radiusSum - distance;
				FVector3 correction = MultVector3(normal, penetrationDepth / 2.0f);
				b1->Location = SubVector3(b1->Location, correction);
				b2->Location = SumVector3(b2->Location, correction);

				//각속도 미완
				//FVector3 tangential = { -normal.y, normal.x, 0 }; // 접선 방향
				//float spinForce = DotProductVector3(relativeVelocity, tangential);
				
				//각속도 미완
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

// 마우스 위치 업데이트 함수
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

// 척력 계산 함수
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
