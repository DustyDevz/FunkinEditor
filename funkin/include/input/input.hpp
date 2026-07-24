/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#pragma once
#include <chrono>
#include <QWindow>

#include "input_types.hpp"
#include "input_ring_buffer.hpp"
#include "input_state.hpp"
#include "input_deadzone.hpp"
#include "input_binding.hpp"

namespace Funkin::Input {
    class Input {
    public:
        static Input& instance();

        void init();
        void shutdown();
        void update();

        void setWindow(QWindow* window) { m_windowHandle = window; }

        void handleQtKeyEvent(QKeyEvent* event, bool isDown);
        void handleQtMouseEvent(QMouseEvent* event, bool isDown);
        void handleQtMouseMoveEvent(QMouseEvent* event);
        void handleQtWheelEvent(QWheelEvent* event);

        const InputState& state() const { return m_state; }

        bool isDown(KeyCode k)   const { return m_state.keys[static_cast<size_t>(k)]; }
        bool justDown(KeyCode k) const { return m_state.keysJustDown[static_cast<size_t>(k)]; }
        bool justUp(KeyCode k)   const { return m_state.keysJustUp[static_cast<size_t>(k)]; }

        bool isDown(const std::string& action)   const { return m_bindingManager.isDown(action, m_state); }
        bool justDown(const std::string& action) const { return m_bindingManager.justDown(action, m_state); }
        bool justUp(const std::string& action)   const { return m_bindingManager.justUp(action, m_state); }

        float axis(uint8_t controller, ControllerAxis a) const;
        const std::vector<InputEvent>& frameEvents() const { return m_state.frameEvents; }

        float get_latency(uint64_t event_time_stamp) const;
        float get_key_latency(KeyCode key_code) const;
        float get_mouse_latency(MouseButton mouse_button) const;
        float get_controller_latency(ControllerButton controller_button, uint8_t controller = 0) const;
        float get_controller_axis_latency(ControllerAxis axis, uint8_t controller = 0) const;

        BindingManager& bindings() { return m_bindingManager; }

        void setDeadZone(ControllerAxis axis, DeadZone dz);
        DeadZone deadZone(ControllerAxis axis) const;

        uint64_t getNow() const;
        uint64_t getLastTimestamp(const std::string& action) const {
            return m_bindingManager.getLastTimestamp(action, m_state);
        }

    private:
        Input() = default;

        static constexpr size_t RING_SIZE = 4096;
        InputRingBuffer<RING_SIZE> m_ring;

        InputState m_state;
        BindingManager m_bindingManager;
        std::array<DeadZone, static_cast<size_t>(ControllerAxis::COUNT)> m_deadZones{};

        std::chrono::high_resolution_clock::time_point m_startTime;
        QWindow* m_windowHandle = nullptr;

        float m_lastMouseX = 0.0f;
        float m_lastMouseY = 0.0f;
        bool  m_hasLastMousePos = false;
    };
}