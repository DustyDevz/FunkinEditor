/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include "input/input_binding.hpp"
#include <fstream>

namespace Funkin::Input {
   bool BindingManager::parseRawInput(const std::string& query, KeyCode& outKey, MouseButton& outMouse, ControllerButton& outCtrl) const {
        outKey   = KeyCode::Unknown;
        outMouse = MouseButton::COUNT;
        outCtrl  = ControllerButton::COUNT;

        std::string prefix;
        std::string name = query;

        auto colonPos = query.find(':');
        if (colonPos != std::string::npos) {
            prefix = query.substr(0, colonPos);
            name   = query.substr(colonPos + 1);
        }

        if (prefix == "mouse" || prefix == "m") {
            if (name == "Left" || name == "LMB")        outMouse = MouseButton::Left;
            else if (name == "Right" || name == "RMB")  outMouse = MouseButton::Right;
            else if (name == "Middle" || name == "MMB") outMouse = MouseButton::Middle;
            else if (name == "X1")                      outMouse = MouseButton::X1;
            else if (name == "X2")                      outMouse = MouseButton::X2;
            return outMouse != MouseButton::COUNT;
        }

        if (prefix == "ctrl" || prefix == "c" || prefix == "pad") {
            if (name == "A")                outCtrl = ControllerButton::A;
            else if (name == "B")           outCtrl = ControllerButton::B;
            else if (name == "X")           outCtrl = ControllerButton::X;
            else if (name == "Y")           outCtrl = ControllerButton::Y;
            else if (name == "Start")       outCtrl = ControllerButton::Start;
            else if (name == "Back")        outCtrl = ControllerButton::Back;
            else if (name == "LB")          outCtrl = ControllerButton::LeftShoulder;
            else if (name == "RB")          outCtrl = ControllerButton::RightShoulder;
            else if (name == "DpadUp")      outCtrl = ControllerButton::DpadUp;
            else if (name == "DpadDown")    outCtrl = ControllerButton::DpadDown;
            else if (name == "DpadLeft")    outCtrl = ControllerButton::DpadLeft;
            else if (name == "DpadRight")   outCtrl = ControllerButton::DpadRight;
            return outCtrl != ControllerButton::COUNT;
        }

        KeyCode k = keyCodeFromName(name.c_str());
        if (k != KeyCode::Unknown) {
            outKey = k;
            return true;
        }

        return false;
    }

    void BindingManager::bind(const std::string& action, KeyCode key) {
        auto& b  = m_bindings[action];
        b.action = action;
        b.key    = key;
        b.useKey = true;
    }

    void BindingManager::bind(const std::string& action, ControllerButton btn) {
        auto& b   = m_bindings[action];
        b.action  = action;
        b.ctrlBtn = btn;
        b.useCtrl = true;
    }

    void BindingManager::bind(const std::string& action, MouseButton btn) {
        auto& b    = m_bindings[action];
        b.action   = action;
        b.mouseBtn = btn;
        b.useMouse = true;
    }

    void BindingManager::unbind(const std::string& action) {
        m_bindings.erase(action);
    }

    bool BindingManager::isDown(const std::string& action, const InputState& state) const {
        auto it = m_bindings.find(action);
        if (it != m_bindings.end()) {
            const auto& b = it->second;
            if (b.useKey && state.keys[static_cast<size_t>(b.key)]) return true;
            if (b.useCtrl && state.controllers[0].buttons[static_cast<size_t>(b.ctrlBtn)]) return true;
            if (b.useMouse && state.mouseButtons[static_cast<size_t>(b.mouseBtn)]) return true;
            return false;
        }

        KeyCode key;
        MouseButton mouseBtn;
        ControllerButton ctrlBtn;
        if (parseRawInput(action, key, mouseBtn, ctrlBtn)) {
            if (key != KeyCode::Unknown)           return state.keys[static_cast<size_t>(key)];
            if (mouseBtn != MouseButton::COUNT)    return state.mouseButtons[static_cast<size_t>(mouseBtn)];
            if (ctrlBtn != ControllerButton::COUNT) return state.controllers[0].buttons[static_cast<size_t>(ctrlBtn)];
        }

        return false;
    }

    bool BindingManager::justDown(const std::string& action, const InputState& state) const {
        auto it = m_bindings.find(action);
        if (it != m_bindings.end()) {
            const auto& b = it->second;
            if (b.useKey && state.keysJustDown[static_cast<size_t>(b.key)]) return true;
            if (b.useCtrl && state.controllers[0].justDown[static_cast<size_t>(b.ctrlBtn)]) return true;
            if (b.useMouse && state.mouseJustDown[static_cast<size_t>(b.mouseBtn)]) return true;
            return false;
        }

        KeyCode key;
        MouseButton mouseBtn;
        ControllerButton ctrlBtn;
        if (parseRawInput(action, key, mouseBtn, ctrlBtn)) {
            if (key != KeyCode::Unknown)           return state.keysJustDown[static_cast<size_t>(key)];
            if (mouseBtn != MouseButton::COUNT)    return state.mouseJustDown[static_cast<size_t>(mouseBtn)];
            if (ctrlBtn != ControllerButton::COUNT) return state.controllers[0].justDown[static_cast<size_t>(ctrlBtn)];
        }

        return false;
    }

    bool BindingManager::justUp(const std::string& action, const InputState& state) const {
        auto it = m_bindings.find(action);
        if (it != m_bindings.end()) {
            const auto& b = it->second;
            if (b.useKey && state.keysJustUp[static_cast<size_t>(b.key)]) return true;
            if (b.useCtrl && state.controllers[0].justUp[static_cast<size_t>(b.ctrlBtn)]) return true;
            if (b.useMouse && state.mouseJustUp[static_cast<size_t>(b.mouseBtn)]) return true;
            return false;
        }

        KeyCode key;
        MouseButton mouseBtn;
        ControllerButton ctrlBtn;
        if (parseRawInput(action, key, mouseBtn, ctrlBtn)) {
            if (key != KeyCode::Unknown)           return state.keysJustUp[static_cast<size_t>(key)];
            if (mouseBtn != MouseButton::COUNT)    return state.mouseJustUp[static_cast<size_t>(mouseBtn)];
            if (ctrlBtn != ControllerButton::COUNT) return state.controllers[0].justUp[static_cast<size_t>(ctrlBtn)];
        }

        return false;
    }

    uint64_t BindingManager::getLastTimestamp(const std::string& action, const InputState& state) const {
        KeyCode targetKey = KeyCode::Unknown;

        auto it = m_bindings.find(action);
        if (it != m_bindings.end()) {
            targetKey = it->second.key;
        } else {
            MouseButton m; ControllerButton c;
            parseRawInput(action, targetKey, m, c);
        }

        if (targetKey == KeyCode::Unknown) return 0;

        uint64_t latest = 0;
        for (const auto& e : state.frameEvents) {
            if (e.type == InputEventType::KeyDown && e.key.key == targetKey) {
                latest = e.time;
            }
        }
        return latest;
    }

    const Binding* BindingManager::getBinding(const std::string& action) const {
        auto it = m_bindings.find(action);
        return it != m_bindings.end() ? &it->second : nullptr;
    }

    void BindingManager::saveBindings(const std::string& path) const {
        std::ofstream f(path);
        if (!f.is_open()) return;

        for (const auto& [name, b] : m_bindings) {
            if (b.useKey)
                f << name << "=key:" << static_cast<uint32_t>(b.key) << "\n";
            if (b.useCtrl)
                f << name << "=ctrl:" << static_cast<uint32_t>(b.ctrlBtn) << "\n";
            if (b.useMouse)
                f << name << "=mouse:" << static_cast<uint32_t>(b.mouseBtn) << "\n";
        }
    }

    void BindingManager::loadBindings(const std::string& path) {
        std::ifstream f(path);
        if (!f.is_open()) return;

        std::string line;
        while (std::getline(f, line)) {
            auto eq = line.find('=');
            if (eq == std::string::npos) continue;

            std::string action = line.substr(0, eq);
            std::string value  = line.substr(eq + 1);

            if (value.starts_with("key:"))
                bind(action, static_cast<KeyCode>(std::stoul(value.substr(4))));
            else if (value.starts_with("ctrl:"))
                bind(action, static_cast<ControllerButton>(std::stoul(value.substr(5))));
            else if (value.starts_with("mouse:"))
                bind(action, static_cast<MouseButton>(std::stoul(value.substr(6))));
        }
    }
}