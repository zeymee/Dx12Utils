#include "Dx12ClassUtils.hpp"
#include <sstream>


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
    descriptorRanges_.push_back(RootParameterDesc(
        0,
        baseShaderRegister,
        registerSpace,
        shaderVisibility
    ));
}

void RootSignatureBuilder::addCBVDescriptorRange(
    uint32_t numDescriptors,
    uint32_t baseShaderRegister,
    uint32_t registerSpace,
    D3D12_SHADER_VISIBILITY shaderVisibility)
{
    descriptorRanges_.push_back(RootParameterDesc(
        0,
        baseShaderRegister,
        registerSpace,
        shaderVisibility
    ));
}

void RootSignatureBuilder::addUAVDescriptorRange(
    uint32_t numDescriptors,
    uint32_t baseShaderRegister,
    uint32_t registerSpace,
    D3D12_SHADER_VISIBILITY shaderVisibility)
{
    descriptorRanges_.push_back(RootParameterDesc(
        0,
        baseShaderRegister,
        registerSpace,
        shaderVisibility
    ));
}

void RootSignatureBuilder::add32BitConstants(uint32_t num32BitValues, uint32_t shaderRegister, uint32_t registerSpace, D3D12_SHADER_VISIBILITY shaderVisibility)
{
    descriptorRanges_.push_back(RootParameterDesc(
        num32BitValues,
        shaderRegister,
        registerSpace,
        shaderVisibility
    ));
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
    std::vector<RootSignatureBuilder::RootParameter> rootParams;
    for(auto const& desc : descriptorRanges_)
        rootParams.push_back(RootSignatureBuilder::RootParameter(desc));

    std::vector<D3D12_ROOT_PARAMETER1> rootParameters;
    for(auto const& rootParam : rootParams)
        rootParameters.push_back(rootParam.rootParameter);

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc{};
    versionedRootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_2;
    versionedRootSignatureDesc.Desc_1_2.NumParameters = static_cast<uint32_t>(rootParameters.size());
    versionedRootSignatureDesc.Desc_1_2.pParameters = rootParameters.data();
    versionedRootSignatureDesc.Desc_1_2.NumStaticSamplers = static_cast<uint32_t>(staticSamplers_.size());
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

Dx12ShaderCompile::Dx12ShaderCompile()
{
    throwIfFailed(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils)));
    throwIfFailed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)));
    throwIfFailed(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library)));
    throwIfFailed(library->CreateIncludeHandler(&includeHandler));
}

ComPtr<IDxcBlob> Dx12ShaderCompile::loadFromFile(
    const std::wstring &filePath,
    const std::wstring &entryPoint,
    const ShaderTarget target,
    const ShaderFlags flag)
{
    std::wstring profile = L"vs_6_6";
    switch(target)
    {
        case ShaderTarget::eVertex:
            profile = L"vs_6_6";
            break;
        case ShaderTarget::ePixel:
            profile = L"ps_6_6";
            break;
        case ShaderTarget::eCompute:
            profile = L"cs_6_6";
            break;
        case ShaderTarget::eDomain:
            profile = L"ds_6_6";
            break;
    }

    std::vector<const wchar_t*> args;
    args.push_back(L"-E");
    args.push_back(entryPoint.c_str());
    args.push_back(L"-T");
    args.push_back(profile.c_str());

    if(flag & ShaderFlags::eDebug)
    {
        args.push_back(L"-Zi");
        args.push_back(L"-Qembed_debug");
    }
    if(flag & ShaderFlags::eSkipOptimization)
        args.push_back(L"-Od");
    if(flag & ShaderFlags::eOptimizationLevel0)
        args.push_back(L"-O0");
    if(flag & ShaderFlags::eOptimizationLevel1)
        args.push_back(L"-O1");
    if(flag & ShaderFlags::eOptimizationLevel2)
        args.push_back(L"-O2");
    if(flag & ShaderFlags::eOptimizationLevel3)
        args.push_back(L"-O3");

    ComPtr<IDxcBlobEncoding> source;
    throwIfFailed(utils->LoadFile(filePath.c_str(), nullptr, &source));

    DxcBuffer buffer{};
    buffer.Ptr = source->GetBufferPointer();
    buffer.Size = source->GetBufferSize();
    buffer.Encoding = DXC_CP_ACP;

    ComPtr<IDxcResult> result;
    throwIfFailed(compiler->Compile(
        &buffer,
        args.data(),
        static_cast<uint32_t>(args.size()),
        includeHandler.Get(),
        IID_PPV_ARGS(&result)
    ));

    ComPtr<IDxcBlobUtf8> errors;
    result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
    if(errors && errors->GetStringLength() > 0)
        std::cout << errors->GetStringPointer() << std::endl;

    HRESULT status = S_OK;
    throwIfFailed(result->GetStatus(&status));
    if(FAILED(status))
        throw std::runtime_error("Failed to compile shader.");

    ComPtr<IDxcBlob> shader;
    throwIfFailed(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader), nullptr));
    return shader;
}
