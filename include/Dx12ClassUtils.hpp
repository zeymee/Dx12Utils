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
    struct RootParameterDesc
    {
        RootParameterDesc(
            uint32_t num32BitValues = 0,
            uint32_t shaderRegister = 0, uint32_t registerSpace = 0,
            D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL) :
                num32BitValues(num32BitValues),
                shaderRegister(shaderRegister), registerSpace(registerSpace),
                shaderVisibility(shaderVisibility) {}

        uint32_t num32BitValues{ 0 };
        uint32_t shaderRegister{ 0 };
        uint32_t registerSpace{ 0 };
        D3D12_SHADER_VISIBILITY shaderVisibility{ D3D12_SHADER_VISIBILITY_ALL };

        D3D12_DESCRIPTOR_RANGE1 getDescriptorRange() const
        {
            D3D12_DESCRIPTOR_RANGE1 descriptorRange{};
            descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            descriptorRange.NumDescriptors = 1;
            descriptorRange.BaseShaderRegister = shaderRegister;
            descriptorRange.RegisterSpace = registerSpace;
            descriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
            return descriptorRange;
        }
    };

    struct RootParameter
    {
        D3D12_DESCRIPTOR_RANGE1 descriptorRange{};
        D3D12_ROOT_PARAMETER1 rootParameter{};
        RootParameter(RootParameterDesc const& desc)
        {
            if(desc.num32BitValues > 0)
            {
                rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
                rootParameter.Constants.Num32BitValues = desc.num32BitValues;
                rootParameter.Constants.ShaderRegister = desc.shaderRegister;
                rootParameter.Constants.RegisterSpace = desc.registerSpace;
                rootParameter.ShaderVisibility = desc.shaderVisibility;
            }
            else
            {
                descriptorRange = desc.getDescriptorRange();
                rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParameter.DescriptorTable.NumDescriptorRanges = 1;
                rootParameter.DescriptorTable.pDescriptorRanges = &descriptorRange;
                rootParameter.ShaderVisibility = desc.shaderVisibility;
            }
            descriptorRange = desc.getDescriptorRange();
        }
    };
    std::vector<D3D12_STATIC_SAMPLER_DESC1> staticSamplers_;
    std::vector<RootParameterDesc> descriptorRanges_;

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