/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#pragma once

namespace Funkin::Input {
    struct DeadZone {
        float inner = .15f;
        float outer = .95f;

        float apply(float value) const {
            float abs_val = value < .0f ? -value : value;
            float sign = value < .0f ? -1.f : 1.f;

            if (abs_val < inner) return .0f;
            if (abs_val > outer) return sign;

            float normalized = (abs_val - inner) / (outer - inner);
            return sign * normalized;
        }
    };
}