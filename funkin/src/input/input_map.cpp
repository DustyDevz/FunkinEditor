/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/
#include "input/input_map.hpp"

namespace Funkin::Input {
    KeyCode QtKeyMap::mapQtKey(int qtKey, std::uint32_t nativeScanCode) {
        if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z) {
            return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::A) + (qtKey - Qt::Key_A));
        }
        if (qtKey >= Qt::Key_0 && qtKey <= Qt::Key_9) {
            if (qtKey == Qt::Key_0) return KeyCode::Num0;
            return static_cast<KeyCode>(static_cast<uint32_t>(KeyCode::Num1) + (qtKey - Qt::Key_1));
        }

        switch (qtKey) {
            case Qt::Key_Up:         return KeyCode::Up;
            case Qt::Key_Down:       return KeyCode::Down;
            case Qt::Key_Left:       return KeyCode::Left;
            case Qt::Key_Right:      return KeyCode::Right;
            case Qt::Key_Return:
            case Qt::Key_Enter:      return KeyCode::Enter;
            case Qt::Key_Escape:     return KeyCode::Escape;
            case Qt::Key_Space:      return KeyCode::Space;
            case Qt::Key_Tab:        return KeyCode::Tab;
            case Qt::Key_Backspace:  return KeyCode::Backspace;
            case Qt::Key_Shift:      return KeyCode::LShift;
            case Qt::Key_Control:    return KeyCode::LCtrl;
            case Qt::Key_Alt:        return KeyCode::LAlt;
            case Qt::Key_Meta:       return KeyCode::LSuper;
            case Qt::Key_CapsLock:   return KeyCode::CapsLock;
            case Qt::Key_Delete:     return KeyCode::Delete;
            case Qt::Key_Insert:     return KeyCode::Insert;
            case Qt::Key_Home:       return KeyCode::Home;
            case Qt::Key_End:        return KeyCode::End;
            case Qt::Key_PageUp:     return KeyCode::PageUp;
            case Qt::Key_PageDown:   return KeyCode::PageDown;
            case Qt::Key_F1:         return KeyCode::F1;
            case Qt::Key_F2:         return KeyCode::F2;
            case Qt::Key_F3:         return KeyCode::F3;
            case Qt::Key_F4:         return KeyCode::F4;
            case Qt::Key_F5:         return KeyCode::F5;
            case Qt::Key_F6:         return KeyCode::F6;
            case Qt::Key_F7:         return KeyCode::F7;
            case Qt::Key_F8:         return KeyCode::F8;
            case Qt::Key_F9:         return KeyCode::F9;
            case Qt::Key_F10:        return KeyCode::F10;
            case Qt::Key_F11:        return KeyCode::F11;
            case Qt::Key_F12:        return KeyCode::F12;
            case Qt::Key_Comma:      return KeyCode::Comma;
            case Qt::Key_Period:     return KeyCode::Period;
            case Qt::Key_Slash:      return KeyCode::Slash;
            case Qt::Key_Backslash:  return KeyCode::Backslash;
            case Qt::Key_Semicolon:  return KeyCode::Semicolon;
            case Qt::Key_Apostrophe: return KeyCode::Apostrophe;
            case Qt::Key_AsciiTilde:
            case Qt::Key_QuoteLeft:  return KeyCode::Grave;
            case Qt::Key_BracketLeft:  return KeyCode::LBracket;
            case Qt::Key_BracketRight: return KeyCode::RBracket;
            case Qt::Key_Minus:      return KeyCode::Minus;
            case Qt::Key_Equal:      return KeyCode::Equal;
            default:                 return KeyCode::Unknown;
        }
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