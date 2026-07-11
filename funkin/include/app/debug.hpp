/*
* FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

#pragma once
#include <QObject>

namespace Funkin::App {
    class Debug : public QObject {
        Q_OBJECT
        Q_PROPERTY(int fps READ fps WRITE setFps NOTIFY fpsChanged)
        Q_PROPERTY(double memoryUsage READ memoryUsage WRITE setMemoryUsage NOTIFY memoryUsageChanged)

    public:
        explicit Debug(QObject* parent = nullptr);
        ~Debug() override;

        int fps() const;
        void setFps(int value);

        double memoryUsage() const;
        void setMemoryUsage(double value);

        signals:
        void fpsChanged();
        void memoryUsageChanged();
        void showHitboxesChanged();

    private:
        int m_fps;
        double m_memoryUsage;
        bool m_showHitboxes;
    };
}