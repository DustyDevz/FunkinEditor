/*
FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include "app/app.hpp"
#include "utils/log.hpp"

// ! DO NOT PUT JUNK INTO MAIN ! \\
// ! DO NOT PUT JUNK INTO MAIN ! \\
// ! DO NOT PUT JUNK INTO MAIN ! \\

int main(int argc, char* argv[]) {
    Funkin::App app(argc, argv);

    if (!app.init()) {
        LOG_CRIT("Engine initialization failed.");
        return -1;
    }

    return app.run();
}