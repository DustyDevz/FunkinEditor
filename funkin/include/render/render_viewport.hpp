/*
 * FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

#pragma once
#include <QQuickItem>

namespace Diligent {
    class IRenderDevice;
    class IDeviceContext;
    class ISwapChain;
}

namespace Funkin::Render {
    class RenderViewport : public QQuickItem {
        Q_OBJECT

    public:
        RenderViewport();
        ~RenderViewport() override;

    public slots:
        void renderFrame();

    private:
        void init(QWindow* window);

        Diligent::IRenderDevice* m_device_;
        Diligent::IDeviceContext* m_context_;
        Diligent::ISwapChain* m_swapChain_;
        bool m_initialized_;
    };
}