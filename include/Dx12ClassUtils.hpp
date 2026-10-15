#pragma once
#include "Dx12Core.hpp"

#include <vector>
#include <string>

class CmdListManager
{
    ComPtr<ID3D12PipelineState> pso_{nullptr};
    ComPtr<ID3D12GraphicsCommandList10> cmdList_{nullptr};
    ComPtr<ID3D12CommandAllocator> cmdAllocator_{nullptr};
    ComPtr<ID3D12CommandQueue> localQueue_{nullptr};

    ComPtr<ID3D12Fence> fence_{nullptr};
    HANDLE fenceEvent_{nullptr};

    D3D12_COMMAND_LIST_TYPE queueType_{D3D12_COMMAND_LIST_TYPE_DIRECT};

    void init();

public:
    ~CmdListManager();
    CmdListManager(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
    CmdListManager(ComPtr<ID3D12PipelineState> pso);

    ID3D12GraphicsCommandList10* operator->() { return cmdList_.Get(); }

    void execute();
    void close();
    void wait();
    void reset();
};

class RootSignatureBuilder
{
    struct RootParameter
    {
        D3D12_DESCRIPTOR_RANGE1 descriptorRange;
        D3D12_SHADER_VISIBILITY shaderVisibility;
    };

    std::vector<RootParameter> rootParameters_;
    std::vector<D3D12_STATIC_SAMPLER_DESC1> staticSamplers_;

    std::vector<RootParameter> descriptorRanges_;

public:
    void addSRVDescriptorRange(
        uint32_t numDescriptors,
        uint32_t baseShaderRegister, uint32_t registerSpace = 0,
        D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL);
    void addCBVDescriptorRange(
        uint32_t numDescriptors,
        uint32_t baseShaderRegister, uint32_t registerSpace = 0,
        D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL);
    void addUAVDescriptorRange(
        uint32_t numDescriptors,
        uint32_t baseShaderRegister, uint32_t registerSpace = 0,
        D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL);
    void add32BitConstants(
        uint32_t num32BitValues,
        uint32_t shaderRegister, uint32_t registerSpace = 0,
        D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL);
    void addSamplerDescriptor(uint32_t shaderRegister, uint32_t registerSpace = 0);

    ComPtr<ID3D12RootSignature> buildRootSignature(D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
};