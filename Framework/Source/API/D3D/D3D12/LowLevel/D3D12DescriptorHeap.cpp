/***************************************************************************
# Copyright (c) 2015, NVIDIA CORPORATION. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of NVIDIA CORPORATION nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#include "Framework.h"
#include "D3D12DescriptorHeap.h"

namespace Falcor
{
    D3D12DescriptorHeap::D3D12DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t descriptorsCount) : mCount(descriptorsCount), mType (type)
    {
		ID3D12DevicePtr pDevice = gpDevice->getApiHandle();
        mDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(type);
    }

    D3D12DescriptorHeap::~D3D12DescriptorHeap() = default;

    D3D12DescriptorHeap::SharedPtr D3D12DescriptorHeap::create(Type type, uint32_t descriptorsCount, bool shaderVisible)
    {
		ID3D12DevicePtr pDevice = gpDevice->getApiHandle();

        D3D12DescriptorHeap::SharedPtr pHeap = SharedPtr(new D3D12DescriptorHeap(type, descriptorsCount));
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};

        desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.Type = getHeapType(type);
        desc.NumDescriptors = descriptorsCount;
        if(FAILED(pDevice->CreateD3D12DescriptorHeap(&desc, IID_PPV_ARGS(&pHeap->mApiHandle))))
        {
            logError("Can't create descriptor heap");
            return nullptr;
        }

        pHeap->mCpuHeapStart = pHeap->mApiHandle->GetCPUDescriptorHandleForHeapStart();
        pHeap->mGpuHeapStart = pHeap->mApiHandle->GetGPUDescriptorHandleForHeapStart();
        return pHeap;
    }

    template<typename HandleType>
    HandleType getHandleCommon(HandleType base, uint32_t index, uint32_t descSize)
    {
        base.ptr += descSize * index;
        return base;
    }

    D3D12DescriptorHeap::CpuHandle D3D12DescriptorHeap::getCpuHandle(uint32_t index) const
    {
        assert(index < mCurDesc);
        return getHandleCommon(mCpuHeapStart, index, mDescriptorSize);
    }

    D3D12DescriptorHeap::GpuHandle D3D12DescriptorHeap::getGpuHandle(uint32_t index) const
    {
        assert(index < mCurDesc);
        return getHandleCommon(mGpuHeapStart, index, mDescriptorSize);
    }

    D3D12DescriptorHeapEntry::SharedPtr D3D12DescriptorHeap::allocateEntry()
    {
        uint32_t entry;
        if (mFreeEntries.empty() == false)
        {
            entry = mFreeEntries.front();
            mFreeEntries.pop();
        }
        else
        {
            if (mCurDesc >= mCount)
            {
                logError("Can't find free CPU handle in descriptor heap");
                return nullptr;
            }
            entry = mCurDesc;
            mCurDesc++;
        }

        return D3D12DescriptorHeapEntry::create(shared_from_this(), entry);
    }
}
