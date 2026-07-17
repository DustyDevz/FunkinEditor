/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include "render/qt/vk_surface.hpp"

namespace Funkin::Render::QT {
    vk_surface::vk_surface(QWindow *parent) : QWindow(parent) {}

    void *vk_surface::native_handle() const {
        return reinterpret_cast<void*>(winId());
    }

    bool vk_surface::reparent_as_native_child(QWindow *window, int x, int y, int w, int h) {
        if (!window || !handle()) {
            LOG_ERR("missing window or handle");
            return false;
        }

        auto child_hwnd = reinterpret_cast<HWND>(winId());
        auto hwnd       = reinterpret_cast<HWND>(window->winId());
        if (!child_hwnd || !hwnd) {
            LOG_ERR("null hwnd (child={} owner={})", (void*)child_hwnd, (void*)hwnd);
            return false;
        }

        LONG_PTR style = GetWindowLongPtrW(child_hwnd, GWL_STYLE);
        style &= ~(WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        style |= WS_CHILD | WS_VISIBLE;
        SetWindowLongPtrW(child_hwnd, GWL_STYLE, style);

        LONG_PTR exStyle = GetWindowLongPtrW(child_hwnd, GWL_EXSTYLE);
        exStyle &= ~(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME);
        SetWindowLongPtrW(child_hwnd, GWL_EXSTYLE, exStyle);

        if (SetParent(child_hwnd, hwnd) == nullptr) {
            LOG_ERR("set parent failed, getlasterror={}", GetLastError());
            return false;
        }

        SetWindowPos(child_hwnd, nullptr, x, y, w, h,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW
        );

        m_native_child_ = true;
        LOG_PRINT("success, pos=({},{}) size=({}x{})", x, y, w, h);
        return true;
    }

    void vk_surface::set_native_child_geometry(int x, int y, int w, int h) {
        if (!m_native_child_ || !handle()) return;

        auto child_hwnd = reinterpret_cast<HWND>(winId());
        if (!child_hwnd) return;
        SetWindowPos(child_hwnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void vk_surface::exposeEvent(QExposeEvent *) {
        if (isExposed() && !m_exposed_) {
            m_exposed_ = true;
            emit surface_ready();
        }
    }

    void vk_surface::resizeEvent(QResizeEvent *event) {
        QWindow::resizeEvent(event);
        emit surface_resized(event->size().width(), event->size().height());
    }
}
