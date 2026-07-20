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
#include <RenderDeviceVk.h>
#include <CommandQueueVk.h>
#include <vulkan/vulkan.h>

namespace Funkin::Render::Graphics {
   class graphics_device {
   public:
       static graphics_device& instance();

       bool init(bool enable_validation);
       void shutdown();
       bool is_initialized() const { return m_device_ != nullptr; }

       Diligent::IRenderDevice*    device()  const { return m_device_; }
       Diligent::IDeviceContext*   context() const { return m_context_; }
       Diligent::IEngineFactoryVk* factory() const { return m_factory_; }

       struct native_vk_handles {
           void*    instance           = nullptr; // VkInstance
           void*    physical_device    = nullptr; // VkPhysicalDevice
           void*    device             = nullptr; // VkDevice
           void*    queue              = nullptr; // VkQueue
           uint32_t queue_family_index = 0;
           uint32_t queue_index        = 0;
       };
       native_vk_handles get_native_vk_handles() const;

   private:
       graphics_device() = default;
       ~graphics_device() = default;
       graphics_device(const graphics_device&) = delete;
       graphics_device& operator=(const graphics_device&) = delete;

       Diligent::IEngineFactoryVk* m_factory_ = nullptr;
       Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_device_;
       Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_context_;
   };
}