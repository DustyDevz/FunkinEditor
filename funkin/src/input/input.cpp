/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include "input/input.hpp"
#include "input/input_map.hpp"
#include <unordered_map>
#include <string_view>

#include "../../libs/diligent/DiligentTools/ThirdParty/libtiff/tiff.h"

namespace Funkin::Input {
    static const char* s_keyNames[static_cast<size_t>(KeyCode::COUNT)] = {
        "Unknown",
        "A","B","C","D","E","F","G","H","I","J","K","L","M",
        "N","O","P","Q","R","S","T","U","V","W","X","Y","Z",
        "0","1","2","3","4","5","6","7","8","9",
        "Numpad0","Numpad1","Numpad2","Numpad3","Numpad4",
        "Numpad5","Numpad6","Numpad7","Numpad8","Numpad9",
        "NumpadAdd","NumpadSub","NumpadMul","NumpadDiv","NumpadEnter",
        "F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12",
        "LShift","RShift","LCtrl","RCtrl","LAlt","RAlt","LSuper","RSuper",
        "Up","Down","Left","Right",
        "Home","End","PageUp","PageDown","Insert","Delete",
        "Enter","Escape","Space","Tab","Backspace","CapsLock",
        "PrintScreen","ScrollLock","Pause",
        "Comma","Period","Slash","Backslash",
        "Semicolon","Apostrophe","Grave","LBracket","RBracket","Minus","Equal"
    };

    const char* keyCodeName(KeyCode k) {
        size_t idx = static_cast<size_t>(k);
        if (idx >= static_cast<size_t>(KeyCode::COUNT)) return "Unknown";
        return s_keyNames[idx];
    }

    KeyCode keyCodeFromName(std::string_view name) {
        static const auto keyTable = []() {
            std::unordered_map<std::string_view, KeyCode> map;
            map.reserve(static_cast<size_t>(KeyCode::COUNT));
            for (size_t i = 0; i < static_cast<size_t>(KeyCode::COUNT); ++i) {
                map[s_keyNames[i]] = static_cast<KeyCode>(i);
            }
            return map;
        }();

        auto it = keyTable.find(name);
        return (it != keyTable.end()) ? it->second : KeyCode::Unknown;
    }

    Input& Input::instance() {
        static Input instance;
        return instance;
    }

    void Input::init() {
        for (auto& dz : m_deadZones) {
            dz.inner = 0.15f;
            dz.outer = 0.95f;
        }
        m_state.frameEvents.reserve(64);
        m_startTime = std::chrono::high_resolution_clock::now();
    }

    void Input::shutdown() {}

    uint64_t Input::getNow() const {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_startTime).count();
    }

    void Input::setDeadZone(ControllerAxis axis, DeadZone dz) {
        m_deadZones[static_cast<size_t>(axis)] = dz;
    }

    DeadZone Input::deadZone(ControllerAxis axis) const {
        return m_deadZones[static_cast<size_t>(axis)];
    }

    float Input::get_latency(uint64_t event_time_stamp) const {
        if (event_time_stamp == 0) return .0f;

        uint64_t now = getNow();
        uint64_t diff = (now >= event_time_stamp) ? (now - event_time_stamp) : 0;
        return static_cast<float>(diff) / 1'000'000.f;
    }

    float Input::get_key_latency(KeyCode key_code) const {
        size_t k = static_cast<size_t>(key_code);
        return (k < static_cast<size_t>(KeyCode::COUNT)) ? get_latency(m_state.keyJustDownTime[k]) : .0f;
    }

    float Input::get_mouse_latency(MouseButton mouse_button) const {
        size_t b = static_cast<size_t>(mouse_button);
        return (b < static_cast<size_t>(MouseButton::COUNT)) ? get_latency(m_state.mouseJustDownTime[b]) : .0f;
    }

    float Input::get_controller_latency(ControllerButton controller_button, uint8_t controller) const {
        if (controller >= 4) return 0.0f;

        size_t b = static_cast<size_t>(controller_button);
        return (b < static_cast<size_t>(ControllerButton::COUNT))
            ? get_latency(m_state.controllers[controller].justDownTime[b])
            : .0f;
    }

    float Input::get_controller_axis_latency(ControllerAxis axis, uint8_t controller) const {
        if (controller >= 4) return .0f;

        size_t a = static_cast<size_t>(axis);
        return (a < static_cast<size_t>(ControllerAxis::COUNT))
        ? get_latency(m_state.controllers[controller].axes[a].lastUpdateTime) : .0f;
    }

    float Input::axis(uint8_t controller, ControllerAxis a) const {
        if (controller >= 4) return 0.0f;
        return m_state.controllers[controller].axes[static_cast<size_t>(a)].processed;
    }

    void Input::handleQtKeyEvent(QKeyEvent* event, bool isDown) {
        InputEvent e{};
        e.time = getNow();
        e.type = isDown ? InputEventType::KeyDown : InputEventType::KeyUp;
        e.key.key = QtKeyMap::mapQtKey(event->key(), event->nativeScanCode());
        e.key.repeat = event->isAutoRepeat();

        if (e.key.key != KeyCode::Unknown) {
            m_ring.push(e);
        }
    }

    void Input::handleQtMouseEvent(QMouseEvent* event, bool isDown) {
        InputEvent e{};
        e.time = getNow();
        e.type = isDown ? InputEventType::MouseDown : InputEventType::MouseUp;
        e.mouseButton.btn = QtKeyMap::mapQtMouseButton(event->button());

        if (e.mouseButton.btn != MouseButton::COUNT) {
            m_ring.push(e);
        }
    }

    void Input::handleQtMouseMoveEvent(QMouseEvent* event) {
        float x = static_cast<float>(event->position().x());
        float y = static_cast<float>(event->position().y());

        if (!m_hasLastMousePos) {
            m_lastMouseX = x;
            m_lastMouseY = y;
            m_hasLastMousePos = true;
        }

        InputEvent e{};
        e.time = getNow();
        e.type = InputEventType::MouseMove;
        e.mouseMove.x = x - m_lastMouseX;
        e.mouseMove.y = y - m_lastMouseY;

        m_lastMouseX = x;
        m_lastMouseY = y;

        m_ring.push(e);
    }

    void Input::handleQtWheelEvent(QWheelEvent* event) {
        InputEvent e{};
        e.time = getNow();
        e.type = InputEventType::MouseScroll;
        e.mouseScroll.dx = static_cast<float>(event->angleDelta().x()) / 120.0f;
        e.mouseScroll.dy = static_cast<float>(event->angleDelta().y()) / 120.0f;

        m_ring.push(e);
    }

   void Input::update() {
        m_state.keysJustDown.fill(false);
        m_state.keysJustUp.fill(false);
        m_state.mouseJustDown.fill(false);
        m_state.mouseJustUp.fill(false);
        m_state.mouseDX = 0.0f;
        m_state.mouseDY = 0.0f;
        m_state.scrollX = 0.0f;
        m_state.scrollY = 0.0f;
        m_state.frameEvents.clear();

        for (auto& ctrl : m_state.controllers) {
            ctrl.justDown.fill(false);
            ctrl.justUp.fill(false);
        }

        InputEvent e{};
        while (m_ring.pop(e)) {
            m_state.frameEvents.push_back(e);

            switch (e.type) {
                case InputEventType::KeyDown: {
                    size_t k = static_cast<size_t>(e.key.key);
                    if (!m_state.keys[k]) {
                        m_state.keysJustDown[k] = true;
                        m_state.keyJustDownTime[k] = e.time;
                    }
                    m_state.keys[k] = true;
                    break;
                }
                case InputEventType::KeyUp: {
                    size_t k = static_cast<size_t>(e.key.key);
                    m_state.keys[k] = false;
                    m_state.keysJustUp[k] = true;
                    break;
                }
                case InputEventType::MouseDown: {
                    size_t b = static_cast<size_t>(e.mouseButton.btn);
                    if (!m_state.mouseButtons[b]) {
                        m_state.mouseJustDown[b] = true;
                        m_state.mouseJustDownTime[b] = e.time;
                    }
                    m_state.mouseButtons[b] = true;
                    break;
                }
                case InputEventType::MouseUp: {
                    size_t b = static_cast<size_t>(e.mouseButton.btn);
                    m_state.mouseButtons[b] = false;
                    m_state.mouseJustUp[b]  = true;
                    break;
                }
                case InputEventType::ControllerButtonDown: {
                    auto& ctrl = m_state.controllers[e.ctrlBtn.id];
                    size_t b = static_cast<size_t>(e.ctrlBtn.btn);
                    if (!ctrl.buttons[b]) {
                        ctrl.justDown[b] = true;
                        ctrl.justDownTime[b] = e.time;
                    }
                    ctrl.buttons[b] = true;
                    break;
                }
                case InputEventType::ControllerButtonUp: {
                    auto& ctrl = m_state.controllers[e.ctrlBtn.id];
                    size_t b = static_cast<size_t>(e.ctrlBtn.btn);
                    ctrl.buttons[b] = false;
                    ctrl.justUp[b]  = true;
                    break;
                }
                case InputEventType::ControllerAxis: {
                    auto& ctrl = m_state.controllers[e.ctrlAxis.id];
                    size_t a   = static_cast<size_t>(e.ctrlAxis.axis);
                    ctrl.axes[a].raw            = e.ctrlAxis.value;
                    ctrl.axes[a].processed      = m_deadZones[a].apply(e.ctrlAxis.value);
                    ctrl.axes[a].lastUpdateTime = e.time;
                    break;
                }
                default: break;
            }
        }
    }
}
