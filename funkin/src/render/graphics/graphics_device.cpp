/*
FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include "render/graphics/graphics_device.hpp"
#include <EngineFactoryVk.h>

#include "utils/log.hpp"

namespace Funkin::Render::GFX {
    graphics_device &graphics_device::instance() {
        static graphics_device instance;
        return instance;
    }

    bool graphics_device::init(bool enable_validation) {
        if (is_initialized()) return true;

        auto* get_factory_vk = Diligent::LoadGraphicsEngineVk();
        if (!get_factory_vk) {
            LOG_CRIT("get_factory_vk failed");
            return false;
        }

        m_factory_ = get_factory_vk();
        Diligent::EngineVkCreateInfo engine_vk_create_info{};
        engine_vk_create_info.EnableValidation = enable_validation;

        Diligent::IRenderDevice*  raw_device = nullptr;
        Diligent::IDeviceContext* raw_context = nullptr;

        m_factory_->CreateDeviceAndContextsVk(
            engine_vk_create_info,
            &raw_device,
            &raw_context
        );

        if (!raw_device || !raw_context) {
            LOG_CRIT("create_device_and_contexts_vk failed");
            return false;
        }

        m_device_.Attach(raw_device);
        m_context_.Attach(raw_context);

        LOG_PRINT("init OK");
        return true;
    }

    void graphics_device::shutdown() {
        if (m_context_) {
            m_context_->Flush();
            m_context_.Release();
        }

        if (m_device_) {
            m_device_.Release();
        }

        m_factory_ = nullptr;
        LOG_PRINT("shutdown OK");
    }
}
