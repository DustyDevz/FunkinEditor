/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#pragma once

#include <QQuickItem>
#include <QSGSimpleTextureNode>
#include <QQuickWindow>
#include <QSGTexture>

#include "graphics_context.hpp"

namespace Funkin::Render::Graphics {
    class graphics_viewport : public QQuickItem {
        Q_OBJECT
    public:
        graphics_viewport();

        void set_graphics_context(graphics_context* graphics_ctx);
        void invalidate_texture();

    protected:
        QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;
        void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    private:
        graphics_context* m_context_ = nullptr;
        QSGTexture* m_sg_texture_ = nullptr;
        VkImage m_last_vk_image_ = VK_NULL_HANDLE;

        bool m_texture_dirty_ = true;
        bool m_size_dirty_ = false;

        uint32_t m_pending_width_ = 0;
        uint32_t m_pending_height_ = 0;
    };
}