/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#pragma once
#include <QQuickItem>
#include <memory>

namespace Funkin::Render::QT {
    class vk_surface;
    class vk_viewport : public QQuickItem {
        Q_OBJECT

    public:
        vk_viewport();
        ~vk_viewport() override;

    protected:
        void itemChange(ItemChange, const ItemChangeData &) override;
        void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    private:
        void sync_surface_geometry();
        void on_surface_ready();
        void on_surface_resized(int w, int h);
        void render_frame();
        void shutdown();
        void reparent_now(QWindow* quick_window);

        std::unique_ptr<vk_surface> m_surface_;
        QTimer* m_timer_ = nullptr;
        QWindow* m_pending_reparent_window_ = nullptr;

        bool m_vk_initialized_ = false;
        bool m_shutdown_done_  = false;
        bool m_reparented_     = false;
    };
}