/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#pragma once
#include <QWindow>

namespace Funkin::Render::QT {
    class vk_surface : public QWindow {
        Q_OBJECT

    public:
        explicit vk_surface(QWindow* parent = nullptr);

        bool reparent_as_native_child(QWindow* window, int x, int y, int w, int h);
        void set_native_child_geometry(int x, int y, int w, int h);
        void* native_handle() const;

        signals:
        void surface_ready();
        //void surface_resized(int w, int h);

    protected:
        void exposeEvent(QExposeEvent *) override;
        //void resizeEvent(QResizeEvent *) override;

    private:
        bool m_exposed_ = false;
        bool m_native_child_ = false;
    };
}