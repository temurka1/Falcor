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
#include "Particles.h"
#include "API/D3D/FalcorD3D.h"

void Particles::onGuiRender()
{
    mpGui->addText("Hello from Particles");
    if (mpGui->addButton("Click Here"))
    {
        msgBox("Now why would you do that?");
    }
}

void Particles::onLoad()
{
    //Wtf is this computecontext for?
    //mpComputeContext = ComputeContext::create();
    mParticleSystem.init(mpRenderContext.get(), 4096);
    mpRenderContext->initCommandSignatures();
    mpCamera = Camera::create();
    mpCamController.attachCamera(mpCamera);
}

void Particles::onFrameRender()
{
	const glm::vec4 clearColor(0.38f, 0.52f, 0.10f, 1);
 	mpRenderContext->clearFbo(mpDefaultFBO.get(), clearColor, 1.0f, 0, FboAttachmentType::All);

    mpCamController.update();

    mParticleSystem.emit(mpRenderContext.get(), 1);
    //This should actually be dt from framerate object
    mParticleSystem.update(mpRenderContext.get(), 0.016f);
    mParticleSystem.render(mpRenderContext.get(), mpCamera->getViewMatrix(), mpCamera->getProjMatrix());
}

void Particles::onShutdown()
{
    mpRenderContext->releaseCommandSignatures();
}

bool Particles::onKeyEvent(const KeyboardEvent& keyEvent)
{
    mpCamController.onKeyEvent(keyEvent);
    return false;
}

bool Particles::onMouseEvent(const MouseEvent& mouseEvent)
{
    mpCamController.onMouseEvent(mouseEvent);
    return false;
}

void Particles::onDataReload()
{

}

void Particles::onResizeSwapChain()
{
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    Particles sample;
    SampleConfig config;
    config.windowDesc.title = "Falcor Project Template";
    config.windowDesc.resizableWindow = true;
    sample.run(config);
}
