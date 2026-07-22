/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#pragma once

#include <RefCntAutoPtr.hpp>
#include <RenderDevice.h>
#include <DeviceContext.h>
#include <EngineFactoryVk.h>

namespace Funkin::Render::Graphics {
    class graphics_device {
    public:
        static graphics_device& instance();

        graphics_device(const graphics_device&) = delete;
        graphics_device& operator=(const graphics_device&) = delete;

        bool init(bool enable_validation);
        void shutdown();
        [[nodiscard]] bool is_initialized() const { return m_device_ != nullptr; }
        bool create_swap_chain(void* hwnd, uint32_t width, uint32_t height);

        [[nodiscard]] Diligent::IRenderDevice*    device()     const { return m_device_; }
        [[nodiscard]] Diligent::IDeviceContext*   context()    const { return m_context_; }
        [[nodiscard]] Diligent::IEngineFactoryVk* factory()    const { return m_factory_; }
        [[nodiscard]] Diligent::ISwapChain*       swap_chain() const { return m_swap_chain_; }

        struct native_vk_handles {
            void*    instance           = nullptr;
            void*    physical_device    = nullptr;
            void*    device             = nullptr;
            void*    queue              = nullptr;
            uint32_t queue_family_index = 0;
            uint32_t queue_index        = 0;
        };

        [[nodiscard]] native_vk_handles get_native_vk_handles() const;

    private:
        graphics_device() = default;
        ~graphics_device() = default;

        Diligent::IEngineFactoryVk* m_factory_ = nullptr;
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_device_;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_context_;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_swap_chain_;
    };
}