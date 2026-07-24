/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include "input/input_map.hpp"
#include <unordered_map>

namespace Funkin::Input {
    KeyCode QtKeyMap::mapQtKey(int qtKey, std::uint32_t nativeScanCode) {
        if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z) {
            return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::A) + (qtKey - Qt::Key_A));
        }
        if (qtKey >= Qt::Key_0 && qtKey <= Qt::Key_9) {
            if (qtKey == Qt::Key_0) return KeyCode::Num0;
            return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::Num1) + (qtKey - Qt::Key_1));
        }
        if (qtKey >= Qt::Key_F1 && qtKey <= Qt::Key_F12) {
            return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::F1) + (qtKey - Qt::Key_F1));
        }

        static const std::unordered_map<int, KeyCode> qtLookup = {
            { Qt::Key_Up, KeyCode::Up }, { Qt::Key_Down, KeyCode::Down },
            { Qt::Key_Left, KeyCode::Left }, { Qt::Key_Right, KeyCode::Right },
            { Qt::Key_Return, KeyCode::Enter }, { Qt::Key_Enter, KeyCode::Enter },
            { Qt::Key_Escape, KeyCode::Escape }, { Qt::Key_Space, KeyCode::Space },
            { Qt::Key_Tab, KeyCode::Tab }, { Qt::Key_Backspace, KeyCode::Backspace },
            { Qt::Key_Shift, KeyCode::LShift }, { Qt::Key_Control, KeyCode::LCtrl },
            { Qt::Key_Alt, KeyCode::LAlt }, { Qt::Key_Meta, KeyCode::LSuper },
            { Qt::Key_CapsLock, KeyCode::CapsLock }, { Qt::Key_Delete, KeyCode::Delete },
            { Qt::Key_Insert, KeyCode::Insert }, { Qt::Key_Home, KeyCode::Home },
            { Qt::Key_End, KeyCode::End }, { Qt::Key_PageUp, KeyCode::PageUp },
            { Qt::Key_PageDown, KeyCode::PageDown }, { Qt::Key_Comma, KeyCode::Comma },
            { Qt::Key_Period, KeyCode::Period }, { Qt::Key_Slash, KeyCode::Slash },
            { Qt::Key_Backslash, KeyCode::Backslash }, { Qt::Key_Semicolon, KeyCode::Semicolon },
            { Qt::Key_Apostrophe, KeyCode::Apostrophe }, { Qt::Key_AsciiTilde, KeyCode::Grave },
            { Qt::Key_QuoteLeft, KeyCode::Grave }, { Qt::Key_BracketLeft, KeyCode::LBracket },
            { Qt::Key_BracketRight, KeyCode::RBracket }, { Qt::Key_Minus, KeyCode::Minus },
            { Qt::Key_Equal, KeyCode::Equal }
        };

        auto it = qtLookup.find(qtKey);
        return (it != qtLookup.end()) ? it->second : KeyCode::Unknown;
    }

    MouseButton QtKeyMap::mapQtMouseButton(Qt::MouseButton button) {
        switch (button) {
            case Qt::LeftButton:   return MouseButton::Left;
            case Qt::RightButton:  return MouseButton::Right;
            case Qt::MiddleButton: return MouseButton::Middle;
            case Qt::BackButton:   return MouseButton::X1;
            case Qt::ForwardButton:return MouseButton::X2;
            default:               return MouseButton::COUNT;
        }
    }
}