#include "render/gfx/vk_context.hpp"
#include "render/gfx/vk_render.hpp"
#include "render/qt/vk_surface.hpp"
#include "render/qt/vk_viewport.hpp"

#include <QTimer>
#include <QQuickWindow>
#include <QGuiApplication>

#include "utils/log.hpp"

namespace Funkin::Render::QT {
    vk_viewport::vk_viewport() {
      LOG_PRINT("constructed");
    }

    vk_viewport::~vk_viewport() {
        LOG_PRINT("destructing");
        shutdown();
    }

    void vk_viewport::shutdown() {
        if (m_shutdown_done_) return;
        m_shutdown_done_ = true;

        if (m_timer_) {
            m_timer_->stop();
        }
        if (m_vk_initialized_) {
            GFX::vk_context::instance().shutdown();
            m_vk_initialized_ = false;
        }
    }

    void vk_viewport::itemChange(ItemChange change, const ItemChangeData& value) {
        QQuickItem::itemChange(change, value);

        if (change == ItemSceneChange && value.window != nullptr) {
            auto* quick_window = value.window;
            if (!quick_window) return;

            m_surface_ = std::make_unique<vk_surface>();
            connect(m_surface_.get(), &vk_surface::surface_ready, this, &vk_viewport::on_surface_ready);
            connect(m_surface_.get(), &vk_surface::surface_resized, this, &vk_viewport::on_surface_resized);
            LOG_PRINT("creating native surface");
            m_surface_->create();
            m_surface_->show();

            connect(qGuiApp, &QGuiApplication::aboutToQuit, this, &vk_viewport::shutdown);

            if (width() > 0 && height() > 0) {
                reparent_now(quick_window);
            } else {
                LOG_PRINT("geometry not ready yet, deferring reparent");
                m_pending_reparent_window_ = quick_window;
            }
        }
    }

    void vk_viewport::reparent_now(QWindow* quick_window) {
        if (m_reparented_ || !m_surface_) return;

        const qreal dpr = quick_window->devicePixelRatio();
        const int x = static_cast<int>(this->x() * dpr);
        const int y = static_cast<int>(this->y() * dpr);
        const int w = static_cast<int>(width()  * dpr) > 0 ? static_cast<int>(width()  * dpr) : 1;
        const int h = static_cast<int>(height() * dpr) > 0 ? static_cast<int>(height() * dpr) : 1;

        if (m_surface_->reparent_as_native_child(quick_window, x, y, w, h)) {
            m_reparented_ = true;
        }
    }

    void vk_viewport::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) {
        QQuickItem::geometryChange(newGeometry, oldGeometry);

        if (!m_reparented_ && m_pending_reparent_window_ && width() > 0 && height() > 0) {
            reparent_now(m_pending_reparent_window_);
            m_pending_reparent_window_ = nullptr;
        }

        sync_surface_geometry();
    }

    void vk_viewport::sync_surface_geometry() {
        if (!m_surface_ || !window()) return;

        const qreal dpr = window()->devicePixelRatio();
        const int x = static_cast<int>(this->x() * dpr);
        const int y = static_cast<int>(this->y() * dpr);
        const int w = static_cast<int>(width()  * dpr);
        const int h = static_cast<int>(height() * dpr);
        if (w <= 0 || h <= 0) return;

        m_surface_->set_native_child_geometry(x, y, w, h);

        if (m_vk_initialized_)
            GFX::vk_context::instance().reset(static_cast<uint16_t>(w), static_cast<uint16_t>(h));
    }

    void vk_viewport::on_surface_ready() {
        if (m_vk_initialized_) return;

        const qreal dpr = window() ? window()->devicePixelRatio() : 1.0;
        const int w = static_cast<int>(width()  * dpr) > 0 ? static_cast<int>(width()  * dpr) : 1;
        const int h = static_cast<int>(height() * dpr) > 0 ? static_cast<int>(height() * dpr) : 1;

        LOG_PRINT("surface ready, initializing vk at {}x{}", w, h);
        m_vk_initialized_ = GFX::vk_context::instance().init(
            m_surface_->native_handle(),
            static_cast<uint16_t>(w),
            static_cast<uint16_t>(h));

        if (!m_vk_initialized_) {
            LOG_CRIT("failed to initalize");
            return;
        }

        m_timer_ = new QTimer(this);
        m_timer_->setInterval(16);
        connect(m_timer_, &QTimer::timeout, this, &vk_viewport::render_frame);
        m_timer_->start();
    }

    void vk_viewport::on_surface_resized(int w, int h) {
        if (!m_vk_initialized_ || w <= 0 || h <= 0) return;
        GFX::vk_context::instance().reset(static_cast<uint16_t>(w), static_cast<uint16_t>(h));
    }

    void vk_viewport::render_frame() {
        if (!m_vk_initialized_) return;

        GFX::vk_render view(0);
        view.set_color(0x2A2A2AFF); // debug [0x00FF00FF]
        view.set_rect(0, 0, static_cast<uint16_t>(width()), static_cast<uint16_t>(height()));
        view.touch();

        bgfx::dbgTextClear();
        bgfx::dbgTextPrintf(0, 0, 0x0f, "hi :3");

        GFX::vk_context::instance().frame();
    }
}