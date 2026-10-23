#include "Dx12Core.hpp"

static const bool isDebug = 
#if defined(DEBUG) || defined(_DEBUG)
true;
#else
false;
#endif


struct Dx12CoreMembers
{
    void pullDebugMessages();

    Dx12CoreMembers();
    ~Dx12CoreMembers();
    ComPtr<ID3D12Device14> device{nullptr};
    ComPtr<IDXGIFactory6> factory{nullptr};
    ComPtr<IDXGIAdapter4> adapter{nullptr};

    ComPtr<ID3D12Debug1> debugController{nullptr};
    ComPtr<ID3D12InfoQueue> infoQueue{nullptr};
    D3D12_MESSAGE *messages{nullptr};

    ComPtr<ID3D12CommandQueue> directQueue{nullptr};
    ComPtr<ID3D12CommandQueue> computeQueue{nullptr};
    ComPtr<ID3D12CommandQueue> copyQueue{nullptr};
};

static Dx12CoreMembers dx12CoreMembers = Dx12CoreMembers();

void Dx12CoreMembers::pullDebugMessages()
{
    size_t msgCount = infoQueue->GetNumStoredMessages();
    for(size_t i = 0; i < msgCount; ++i)
    {
        size_t messageLength = 0;
        infoQueue->GetMessage(i, nullptr, &messageLength);
        if(messages && messageLength > 0)
        {
            infoQueue->GetMessage(i, messages, &messageLength);
            std::wcout << L"Dx12 Debug Message: " << messages->pDescription << "\n";
        }
    }
    infoQueue->ClearStoredMessages();
}

Dx12CoreMembers::Dx12CoreMembers()
{
if(isDebug)
{
    D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
    debugController->EnableDebugLayer();
}

    throwIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));
    throwIfFailed(factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)));
    throwIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&device)));

    DXGI_ADAPTER_DESC3 adapterDesc;
    adapter->GetDesc3(&adapterDesc);
    std::wcout << L"Using adapter: " << adapterDesc.Description <<
    L" memory: " << adapterDesc.DedicatedVideoMemory / (1024 * 1024) << L" MB" << std::endl;

if(isDebug)
{
    throwIfFailed(device.As(&infoQueue));
    infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
    infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
    infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, TRUE);
    infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_MESSAGE, TRUE);
    messages = (D3D12_MESSAGE *)malloc(8'192);
}

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 0;

    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    throwIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&directQueue)));

    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    throwIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&computeQueue)));

    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    throwIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&copyQueue)));
}

Dx12CoreMembers::~Dx12CoreMembers()
{
    if(isDebug)
        if (messages)
        {
            free(messages);
            messages = nullptr;
        }
}

void throwIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        std::cout << "HRESULT failed: " << std::hex << hr << std::endl;
        if(isDebug)
            dx12CoreMembers.pullDebugMessages();
        throw std::runtime_error("HRESULT failed");
    }
}

ComPtr<ID3D12Device14> &Dx12Core::getDevice()
{ return dx12CoreMembers.device; }

ComPtr<IDXGIFactory6> &Dx12Core::getFactory()
{ return dx12CoreMembers.factory; }

ComPtr<IDXGIAdapter4> &Dx12Core::getAdapter()
{ return dx12CoreMembers.adapter; }

ComPtr<ID3D12CommandQueue> &Dx12Core::getDirectQueue()
{ return dx12CoreMembers.directQueue; }

ComPtr<ID3D12CommandQueue> &Dx12Core::getComputeQueue()
{ return dx12CoreMembers.computeQueue; }

ComPtr<ID3D12CommandQueue> &Dx12Core::getCopyQueue()
{ return dx12CoreMembers.copyQueue; }
