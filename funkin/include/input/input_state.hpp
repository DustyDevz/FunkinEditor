/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/


#pragma once
#include "input_types.hpp"
#include <array>
#include <vector>

namespace Funkin::Input {
    struct AxisState {
        float raw       = .0f;
        float processed = .0f;
    };

    struct ControllerState {
        bool connected = false;
        std::array<bool, static_cast<size_t>(ControllerButton::COUNT)> buttons{};
        std::array<bool, static_cast<size_t>(ControllerButton::COUNT)> justDown{};
        std::array<bool, static_cast<size_t>(ControllerButton::COUNT)> justUp{};
        std::array<AxisState, static_cast<size_t>(ControllerAxis::COUNT)> axes{};
    };

    struct InputState {
        std::array<bool, static_cast<size_t>(KeyCode::COUNT)> keys{};
        std::array<bool, static_cast<size_t>(KeyCode::COUNT)> keysJustDown{};
        std::array<bool, static_cast<size_t>(KeyCode::COUNT)> keysJustUp{};

        float mouseX  = .0f, mouseY  = .0f;
        float mouseDX = .0f, mouseDY = .0f;
        float scrollX = .0f, scrollY = .0f;
        std::array<bool, static_cast<size_t>(MouseButton::COUNT)> mouseButtons{};
        std::array<bool, static_cast<size_t>(MouseButton::COUNT)> mouseJustDown{};
        std::array<bool, static_cast<size_t>(MouseButton::COUNT)> mouseJustUp{};

        std::array<ControllerState, 4> controllers{};
        std::vector<InputEvent> frameEvents;
    };
}