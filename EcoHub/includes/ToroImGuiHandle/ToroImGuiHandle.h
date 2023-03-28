#pragma once

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <functional>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif



struct FrameContext
{
	ID3D12CommandAllocator* CommandAllocator;
	UINT64                  FenceValue;
};

// Low Data
constexpr int NUM_BACK_BUFFERS = 3;
constexpr int NUM_FRAMES_IN_FLIGHT = 3;
static FrameContext g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
static UINT g_frameIndex = 0;
static ID3D12Device* g_pd3dDevice = NULL;
static ID3D12DescriptorHeap* g_pd3dRtvDescHeap = NULL;
static ID3D12DescriptorHeap* g_pd3dSrvDescHeap = NULL;
static ID3D12CommandQueue* g_pd3dCommandQueue = NULL;
static ID3D12GraphicsCommandList* g_pd3dCommandList = NULL;
static ID3D12Fence* g_fence = NULL;
static HANDLE g_fenceEvent = NULL;
static UINT64 g_fenceLastSignaledValue = 0;
static IDXGISwapChain3* g_pSwapChain = NULL;
static HANDLE g_hSwapChainWaitableObject = NULL;
static ID3D12Resource* g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

// Helper funcs
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForLastSubmittedFrame();
FrameContext* WaitForNextFrameResources();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef void (*WinBlock)();

class appLayer
{
public:
	virtual ~appLayer() = default;

	virtual void update() {}
	virtual void menuBar() {}
};


class ImGuiHandle
{
private:

	WNDCLASSEXW _wc;
	HWND _hwnd;

	WinBlock _bgFunc = nullptr;
	WinBlock _menuBar = nullptr;

	std::vector<std::shared_ptr<appLayer>> _winsStack;
	int _activeWin = -1;

public:

	ImGuiHandle() {}

	bool init(LPCWSTR bigParentName);
	bool init(LPCWSTR bigParentName, unsigned int bigParentW, unsigned int bigParentH);

	void menuBar(WinBlock menuBarContent) { _menuBar = menuBarContent; }
	void bgDeamon(WinBlock bgFunc) { _bgFunc = bgFunc; }

	void loop(bool *done);

	void end();

	template<typename T>
	void pushLayer()
	{
		static_assert(std::is_base_of<appLayer, T>::value, "Pushed type is not subclass of Layer!");
		_winsStack.emplace_back(std::make_shared<T>());
	}

	void setActiveWin(int index)
	{
		if (index >= _winsStack.size())
			_activeWin = 0;
		else
			_activeWin = index;
	}
};

extern ImGuiHandle IGH;

namespace ImToro
{
	bool vCombo(const char* label, int* current_item, ::std::vector<::std::string> items, int popup_max_height_in_items = -1);
}
