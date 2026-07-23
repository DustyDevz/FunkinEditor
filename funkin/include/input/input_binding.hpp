/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#pragma once
#include <string>
#include <unordered_map>
#include "input_types.hpp"
#include "input_state.hpp"

namespace Funkin::Input {
    struct Binding {
        std::string      action;
        KeyCode          key      = KeyCode::Unknown;
        ControllerButton ctrlBtn  = ControllerButton::COUNT;
        MouseButton      mouseBtn = MouseButton::COUNT;
        bool             useKey   = false;
        bool             useCtrl  = false;
        bool             useMouse = false;
    };

    class BindingManager {
    public:
        void bind(const std::string& action, KeyCode key);
        void bind(const std::string& action, ControllerButton btn);
        void bind(const std::string& action, MouseButton btn);
        void unbind(const std::string& action);

        bool isDown(const std::string& action, const InputState& state) const;
        bool justDown(const std::string& action, const InputState& state) const;
        bool justUp(const std::string& action, const InputState& state) const;

        uint64_t getLastTimestamp(const std::string& action, const InputState& state) const;

        void loadBindings(const std::string& path);
        void saveBindings(const std::string& path) const;

        const Binding* getBinding(const std::string& action) const;

    private:
        bool parseRawInput(const std::string& query, KeyCode& outKey, MouseButton& outMouse, ControllerButton& outCtrl) const;

        std::unordered_map<std::string, Binding> m_bindings;
    };
}