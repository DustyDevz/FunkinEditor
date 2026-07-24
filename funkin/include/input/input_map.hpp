/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#pragma once
#include "input_types.hpp"

namespace Funkin::Input {
    class QtKeyMap {
    public:
        static KeyCode mapQtKey(int qtKey, std::uint32_t nativeScanCode);
        static MouseButton mapQtMouseButton(Qt::MouseButton button);
    };
}