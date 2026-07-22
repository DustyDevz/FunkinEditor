/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include "render/graphics/graphics_viewport.hpp"

namespace Funkin::Render::Graphics {
    graphics_viewport::graphics_viewport() {
        setFlag(ItemHasContents, true);
    }

    void graphics_viewport::set_graphics_context(graphics_context *graphics_ctx) {
        m_context_ = graphics_ctx;
    }

    void graphics_viewport::invalidate_texture() {
        m_texture_dirty_ = true;
    }

    void graphics_viewport::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) {
        QQuickItem::geometryChange(newGeometry, oldGeometry);

        if (newGeometry.size() != oldGeometry.size()) {
            const auto dpr = window() ? window()->devicePixelRatio() : 1.;
            const uint32_t new_w = static_cast<uint32_t>(newGeometry.width() * dpr);
            const uint32_t new_h = static_cast<uint32_t>(newGeometry.height() * dpr);

            if (new_w > 0 && new_h > 0) {
                m_pending_width_ = new_w;
                m_pending_height_ = new_h;
                m_size_dirty_ = true;

                update();
            }
        }
    }

    QSGNode *graphics_viewport::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData *) {
        auto* node = static_cast<QSGSimpleTextureNode*>(oldNode);
        if (!node) {
            node = new QSGSimpleTextureNode();
            node->setFiltering(QSGTexture::Linear);
            node->setSourceRect(QRectF(0.0f, 0.0f, 1.0f, 1.0f));
        }

        if (!m_context_ || !window()) return node;

        bool geometry_changed = false;

        if (m_size_dirty_) {
            m_context_->resize(m_pending_width_, m_pending_height_);
            m_size_dirty_ = false;
            m_texture_dirty_ = true;
        }

        VkImage vk_image = m_context_->get_vk_image();
        if (vk_image == VK_NULL_HANDLE) return node;

        if (!m_sg_texture_ || m_texture_dirty_ || m_last_vk_image_ != vk_image) {
            delete m_sg_texture_;

            QSize size(m_context_->width(), m_context_->height());
            QQuickWindow::CreateTextureOptions texture_options = QQuickWindow::TextureHasAlphaChannel;
            m_sg_texture_ = QNativeInterface::QSGVulkanTexture::fromNative(
                vk_image,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                window(),
                size,
                texture_options
            );

            m_last_vk_image_ = vk_image;
            m_texture_dirty_ = false;

            node->setTexture(m_sg_texture_);
            node->setOwnsTexture(false);
            node->markDirty(QSGNode::DirtyMaterial);
            geometry_changed = true;
        }

        if (geometry_changed || node->rect() != boundingRect()) {
            node->setRect(boundingRect());
            node->markDirty(QSGNode::DirtyGeometry);
        }

        return node;
    }
}