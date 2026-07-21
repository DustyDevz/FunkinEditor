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

    QSGNode *graphics_viewport::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData *) {
        auto* node = static_cast<QSGSimpleTextureNode*>(oldNode);
        if (!node) {
            node = new QSGSimpleTextureNode();
        }

        if (!m_context_ || !window()) return node;

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
        }

        node->setRect(boundingRect());
        node->markDirty(QSGNode::DirtyMaterial);
        return node;
    }
}