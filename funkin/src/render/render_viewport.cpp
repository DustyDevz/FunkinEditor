/*
 * FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

#include "render/render_viewport.hpp"
#include <QQuickWindow>

#define ENGINE_DLL 1
#include <RenderDevice.h>
#include <DeviceContext.h>
#include <SwapChain.h>

namespace Funkin::Render {
    RenderViewport::RenderViewport()
        : m_device_(nullptr),
          m_context_(nullptr),
          m_swapChain_(nullptr),
          m_initialized_(false)
    {
        connect(this, &QQuickItem::windowChanged, this,
            [this](QWindow* window)
        {
            if (auto* quickWindow = qobject_cast<QQuickWindow*>(window)) {
                connect(quickWindow,
                        &QQuickWindow::beforeRendering,
                        this,
                        &RenderViewport::renderFrame,
                        Qt::DirectConnection);

                init(window);
            }
        });
    }

    RenderViewport::~RenderViewport() {
        if (m_swapChain_) m_swapChain_->Release();
        if (m_context_)   m_context_  ->Release();
        if (m_device_)    m_device_   ->Release();
    }

    void RenderViewport::init(QWindow *window) {
        if (m_initialized_) return;
        m_initialized_ = true;
    }

    void RenderViewport::renderFrame() {
        if (!m_initialized_ || !m_context_ || !m_swapChain_) return;

        auto* pRTV = m_swapChain_->GetCurrentBackBufferRTV();
        auto* pDSV = m_swapChain_->GetDepthBufferDSV();
        float clearColor[] = {.12f, .12f, .12f, 1.f};
        m_context_->SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_context_->ClearRenderTarget(pRTV, clearColor, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_swapChain_->Present();
    }
}
