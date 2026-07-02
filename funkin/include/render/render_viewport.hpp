/*
 * FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

#pragma once
#include <QQuickItem>
#include <QSGTexture>
#include <QTimer>
#include <QImage>
#include <vector>

#ifdef _WIN32
#include <vulkan/vulkan.h>
#endif

namespace Diligent {
    class IRenderDevice;
    class IDeviceContext;
    class ITexture;
}

namespace Funkin::Render {
    class RenderViewport : public QQuickItem {
        Q_OBJECT

    public:
        RenderViewport();
        ~RenderViewport() override;

    protected:
        QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data) override;
        void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;

    private slots:
        void onFrameTick();

    private:
        void init();
        void resizeRenderTarget(int width, int height);
        void renderFrame();
        bool readbackFrame(QImage& outImage);

        bool createSharedColorImage(int width, int height);
        void destroySharedColorImage();

        Diligent::IRenderDevice*  m_device_;
        Diligent::IDeviceContext* m_context_;
        Diligent::ITexture*       m_colorTarget_;
        Diligent::ITexture*       m_depthTarget_;
        Diligent::ITexture*       m_stagingTexture_;

        #ifdef _WIN32
            VkImage        m_sharedImage_;
            VkDeviceMemory m_sharedMemory_;
            VkDevice       m_diligentVkDevice_;
        #endif

        int  m_targetWidth_;
        int  m_targetHeight_;
        bool m_initialized_;
        bool m_pendingResize_;

        QTimer m_renderTimer_;
        QImage m_latestFrame_;
        bool   m_frameDirty_;
    };
}