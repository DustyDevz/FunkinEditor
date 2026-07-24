/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/


#pragma once
#include <array>
#include <vector>
#include <cstdint>
#include "input_types.hpp"

namespace Funkin::Input {
    struct ControllerState {
        bool connected = false;
        std::array<bool, static_cast<size_t>(ControllerButton::COUNT)> buttons{};
        std::array<bool, static_cast<size_t>(ControllerButton::COUNT)> justDown{};
        std::array<bool, static_cast<size_t>(ControllerButton::COUNT)> justUp{};
        std::array<uint64_t, static_cast<size_t>(ControllerButton::COUNT)> justDownTime{};

        struct AxisState {
            float raw = 0.0f;
            float processed = 0.0f;
            uint64_t lastUpdateTime = 0;
        };
        std::array<AxisState, static_cast<size_t>(ControllerAxis::COUNT)> axes{};
    };

    struct InputState {
        std::array<bool, static_cast<size_t>(KeyCode::COUNT)> keys{};
        std::array<bool, static_cast<size_t>(KeyCode::COUNT)> keysJustDown{};
        std::array<bool, static_cast<size_t>(KeyCode::COUNT)> keysJustUp{};
        std::array<uint64_t, static_cast<size_t>(KeyCode::COUNT)> keyJustDownTime{};

        std::array<bool, static_cast<size_t>(MouseButton::COUNT)> mouseButtons{};
        std::array<bool, static_cast<size_t>(MouseButton::COUNT)> mouseJustDown{};
        std::array<bool, static_cast<size_t>(MouseButton::COUNT)> mouseJustUp{};
        std::array<uint64_t, static_cast<size_t>(MouseButton::COUNT)> mouseJustDownTime{};

        float mouseX = 0.0f, mouseY = 0.0f;
        float mouseDX = 0.0f, mouseDY = 0.0f;
        float scrollX = 0.0f, scrollY = 0.0f;

        std::array<ControllerState, 4> controllers{};
        std::vector<InputEvent> frameEvents{};
    };
}