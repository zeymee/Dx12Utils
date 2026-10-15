#include "Dx12ClassUtils.hpp"

void CmdListManager::init()
{
    ComPtr<ID3D12Device14> device = Dx12Core::getDevice();
    
    throwIfFailed(device->CreateCommandAllocator(queueType_, IID_PPV_ARGS(&cmdAllocator_)));
    throwIfFailed(device->CreateCommandList(0, queueType_, cmdAllocator_.Get(), pso_.Get(), IID_PPV_ARGS(&cmdList_)));

    throwIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)));
    
    fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if(fenceEvent_ == nullptr)
        throw std::runtime_error("Failed to create fence event!");
}

CmdListManager::~CmdListManager()
{ CloseHandle(fenceEvent_); }

CmdListManager::CmdListManager(D3D12_COMMAND_LIST_TYPE type) : queueType_(type)
{ init(); }

CmdListManager::CmdListManager(ComPtr<ID3D12PipelineState> pso) : pso_(pso)
{ init(); }

void CmdListManager::execute()
{
    wait();
    localQueue_->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const *>(cmdList_.GetAddressOf()));
    localQueue_->Signal(fence_.Get(), 1);
}

void CmdListManager::close()
{ throwIfFailed(cmdList_->Close()); }

void CmdListManager::wait()
{
    if(fence_->GetCompletedValue() == 1)
    {
        fence_->SetEventOnCompletion(1, fenceEvent_);
        WaitForSingleObject(fenceEvent_, INFINITE);
    }
    fence_->Signal(0);
}

void CmdListManager::reset()
{
    throwIfFailed(cmdAllocator_->Reset());
    throwIfFailed(cmdList_->Reset(cmdAllocator_.Get(), pso_.Get()));
}

void RootSignatureBuilder::addSRVDescriptorRange(uint32_t numDescriptors, uint32_t baseShaderRegister, uint32_t registerSpace, D3D12_SHADER_VISIBILITY shaderVisibility)
{
    D3D12_DESCRIPTOR_RANGE1 srvRange{};
    srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    srvRange.NumDescriptors = numDescriptors;
    srvRange.BaseShaderRegister = baseShaderRegister;
    srvRange.RegisterSpace = registerSpace;
    srvRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
    descriptorRanges_.push_back({ srvRange, shaderVisibility });
}

void RootSignatureBuilder::addCBVDescriptorRange(
    uint32_t numDescriptors,
    uint32_t baseShaderRegister,
    uint32_t registerSpace,
    D3D12_SHADER_VISIBILITY shaderVisibility)
{
    D3D12_DESCRIPTOR_RANGE1 cbvRange{};
    cbvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    cbvRange.NumDescriptors = numDescriptors;
    cbvRange.BaseShaderRegister = baseShaderRegister;
    cbvRange.RegisterSpace = registerSpace;
    cbvRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
    descriptorRanges_.push_back({ cbvRange, shaderVisibility });
}

void RootSignatureBuilder::addUAVDescriptorRange(
    uint32_t numDescriptors,
    uint32_t baseShaderRegister,
    uint32_t registerSpace,
    D3D12_SHADER_VISIBILITY shaderVisibility)
{
    D3D12_DESCRIPTOR_RANGE1 uavRange{};
    uavRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    uavRange.NumDescriptors = numDescriptors;
    uavRange.BaseShaderRegister = baseShaderRegister;
    uavRange.RegisterSpace = registerSpace;
    uavRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;
    descriptorRanges_.push_back({ uavRange, shaderVisibility });
}

void RootSignatureBuilder::addSamplerDescriptor(uint32_t shaderRegister, uint32_t registerSpace)
{
	D3D12_STATIC_SAMPLER_DESC1 samplerDesc{};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.MipLODBias = -1.f;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = shaderRegister;
	samplerDesc.RegisterSpace = registerSpace;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	staticSamplers_.push_back(samplerDesc);
}

ComPtr<ID3D12RootSignature> RootSignatureBuilder::buildRootSignature(D3D12_ROOT_SIGNATURE_FLAGS flags)
{
    std::vector<D3D12_ROOT_PARAMETER1> rootParams(descriptorRanges_.size());
    for(uint32_t i = 0; i < descriptorRanges_.size(); i++)
    {
        D3D12_ROOT_PARAMETER1 rootParam{};
        rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParam.DescriptorTable.NumDescriptorRanges = 1;
        rootParam.DescriptorTable.pDescriptorRanges = &descriptorRanges_[i].descriptorRange;
        rootParam.ShaderVisibility = descriptorRanges_[i].shaderVisibility;
        rootParams[i] = rootParam;
    }

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc{};
    versionedRootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_2;
    versionedRootSignatureDesc.Desc_1_2.NumParameters = static_cast<UINT>(rootParams.size());
    versionedRootSignatureDesc.Desc_1_2.pParameters = rootParams.data();
    versionedRootSignatureDesc.Desc_1_2.NumStaticSamplers = static_cast<UINT>(staticSamplers_.size());
    versionedRootSignatureDesc.Desc_1_2.pStaticSamplers = staticSamplers_.data();
    versionedRootSignatureDesc.Desc_1_2.Flags = flags;

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    throwIfFailed(D3D12SerializeVersionedRootSignature(
        &versionedRootSignatureDesc,
        &signature,
        &error
    ));

    if(error)
    {
        std::cout << static_cast<const char*>(error->GetBufferPointer()) << std::endl;
        throw std::runtime_error("Failed to serialize root signature.");
    }
    ComPtr<ID3D12Device14> device = Dx12Core::getDevice();
    ComPtr<ID3D12RootSignature> rootSignature;
    throwIfFailed(device->CreateRootSignature(
        0,
        signature->GetBufferPointer(),
        signature->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)
    ));
    return rootSignature;
}
