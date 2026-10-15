#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl.h>
#include <Windows.h>

#include <cstdint>
#include <iostream>
#include <stdexcept>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxgiuid.lib")

using namespace Microsoft::WRL;

void throwIfFailed(HRESULT hr);

struct Dx12Core
{
    Dx12Core() = default;

    static ComPtr<ID3D12Device14> &getDevice();
    static ComPtr<IDXGIFactory6> &getFactory();
    static ComPtr<IDXGIAdapter4> &getAdapter();

    static ComPtr<ID3D12CommandQueue> &getDirectQueue();
    static ComPtr<ID3D12CommandQueue> &getComputeQueue();
    static ComPtr<ID3D12CommandQueue> &getCopyQueue();

    static void pullDebugMessages();
};