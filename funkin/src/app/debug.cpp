/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include "app/debug.hpp"
#include "utils/log.hpp"

namespace Funkin::App {
    Debug::Debug(QObject* parent)
        : QObject(parent), m_fps(0), m_memoryUsage(0.0), m_showHitboxes(false)
    {
        LOG_PRINT("Debug init");
    }

    Debug::~Debug() {
        LOG_PRINT("Debug died!!!");
    }

    int Debug::fps() const {
        return m_fps;
    }

    void Debug::setFps(int value) {
        if (m_fps != value) {
            m_fps = value;
            emit fpsChanged();
        }
    }

    double Debug::memoryUsage() const {
        return m_memoryUsage;
    }

    void Debug::setMemoryUsage(double value) {
        if (m_memoryUsage != value) {
            m_memoryUsage = value;
            emit memoryUsageChanged();
        }
    }
}