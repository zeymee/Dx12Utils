#pragma once
#include "Dx12ClassUtils.hpp"
#include "Dx12TypeUtils.hpp"

ComPtr<ID3D12Resource> createResource(
    uint64_t sizeInBytes,
    D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT,
    D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON
);

ComPtr<ID3D12Resource> createResource2D(
    uint32_t width, uint32_t height,
    D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT,
    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
    D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON
);

ComPtr<ID3D12Resource> createContiguousResource2D(
    uint32_t width, uint32_t height,
    D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT,
    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
    D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON
);

ComPtr<ID3D12PipelineState> createPipelineState(
    const sg::dxTypes::Blend_Desc& blendDesc,
    const sg::dxTypes::Rasterizer_Desc& rasterizerDesc,
    const sg::dxTypes::Depth_Stencil_Desc& depthStencilDesc,
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayoutDescs,
    const ComPtr<ID3D12RootSignature>& rootSignature,
    const sg::dxTypes::GenericData& vShader,
    const sg::dxTypes::GenericData& pShader,
    DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM
);

void uploadCpuDataToResource(
    const void* pData,
    ComPtr<ID3D12Resource>& dstResource,
    size_t byteSize,
    size_t dstOffset = 0
);

void uploadCpuDataToResource2D(
    uint32_t width, uint32_t height,
    DXGI_FORMAT format,
    const uint8_t* pData,
    ComPtr<ID3D12Resource>& dstResource,
    size_t byteSize,
    size_t dstOffset = 0
);

void recordUploadToDefaultResource(
    ID3D12GraphicsCommandList* pCmdList,
    const ComPtr<ID3D12Resource>& srcResource,
    const ComPtr<ID3D12Resource>& dstResource,
    size_t byteSize,
    size_t srcOffset = 0,
    size_t dstOffset = 0
);

inline void recordUploadToDefaultResource(
    const ComPtr<ID3D12GraphicsCommandList>& pCmdList,
    const ComPtr<ID3D12Resource>& srcResource,
    const ComPtr<ID3D12Resource>& dstResource,
    size_t byteSize,
    size_t srcOffset = 0,
    size_t dstOffset = 0
)
{
    recordUploadToDefaultResource(
        pCmdList.Get(),
        srcResource,
        dstResource,
        byteSize,
        srcOffset,
        dstOffset
    );
}

// Assumes that the srcResource contains the data in a 1D layout suitable for copying to a 2D resource.
void recordUploadToDefaultResource2D(
    ID3D12GraphicsCommandList* pCmdList,
    const ComPtr<ID3D12Resource>& srcResource,
    const ComPtr<ID3D12Resource>& dstResource,
    size_t byteSize,
    size_t srcOffset = 0,
    size_t dstOffset = 0
);

// Assumes that the srcResource contains the data in a 1D layout suitable for copying to a 2D resource.
inline void recordUploadToDefaultResource2D(
    const ComPtr<ID3D12GraphicsCommandList>& pCmdList,
    const ComPtr<ID3D12Resource>& srcResource,
    const ComPtr<ID3D12Resource>& dstResource,
    size_t byteSize,
    size_t srcOffset = 0,
    size_t dstOffset = 0
)
{
    recordUploadToDefaultResource2D(
        pCmdList.Get(),
        srcResource,
        dstResource,
        byteSize,
        srcOffset,
        dstOffset
    );
}

// Assumes that the srcResource contains the data in a 1D layout suitable for copying to a 2D resource.
inline void recordUploadToDefaultResource2D(
    CmdListManager& cmdListManager,
    const ComPtr<ID3D12Resource>& srcResource,
    const ComPtr<ID3D12Resource>& dstResource,
    size_t byteSize = 0,
    size_t dstOffset = 0
)
{
    recordUploadToDefaultResource2D(
        cmdListManager.operator->(),
        srcResource,
        dstResource,
        byteSize,
        0,
        dstOffset
    );
}

void recordTransitionResourceState(
    ID3D12GraphicsCommandList* pCmdList,
    const ComPtr<ID3D12Resource>& resource,
    D3D12_RESOURCE_STATES stateBefore,
    D3D12_RESOURCE_STATES stateAfter
);

inline void recordTransitionResourceState(
    const ComPtr<ID3D12GraphicsCommandList>& pCmdList,
    const ComPtr<ID3D12Resource>& resource,
    D3D12_RESOURCE_STATES stateBefore,
    D3D12_RESOURCE_STATES stateAfter
);

inline void recordTransitionResourceState(
    CmdListManager& cmdListManager,
    const ComPtr<ID3D12Resource>& resource,
    D3D12_RESOURCE_STATES stateBefore,
    D3D12_RESOURCE_STATES stateAfter
);
