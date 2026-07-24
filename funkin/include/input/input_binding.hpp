/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#pragma once
#include <string>
#include <string_view>
#include <unordered_map>

#include "input_types.hpp"
#include "input_state.hpp"

namespace Funkin::Input {
    class Input;

    struct StringHash {
        using is_transparent = void;
        size_t operator()(std::string_view sv) const {
            return std::hash<std::string_view>{}(sv);
        }
    };

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

        [[nodiscard]] bool isDown(std::string_view action, const InputState& state) const;
        [[nodiscard]] bool justDown(std::string_view action, const InputState& state) const;
        [[nodiscard]] bool justUp(std::string_view action, const InputState& state) const;

        [[nodiscard]] uint64_t getLastTimestamp(std::string_view action, const InputState& state) const;

        void loadBindings(const std::string& path);
        void saveBindings(const std::string& path) const;

        [[nodiscard]] const Binding* getBinding(std::string_view action) const;

        float get_latency(std::string_view action, const InputState& state, const Input& input) const;

    private:
        bool parseRawInput(std::string_view query, KeyCode& outKey, MouseButton& outMouse, ControllerButton& outCtrl) const;

        std::unordered_map<std::string, Binding, StringHash, std::equal_to<>> m_bindings;
    };
}