/*
FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#pragma once
#include <RefCntAutoPtr.hpp>
#include <SwapChain.h>
#include <cstdint>
#include "TextureVk.h"

namespace Funkin::Render::Graphics {
  class graphics_context {
  public:
      graphics_context() = default;
      ~graphics_context();

      graphics_context(const graphics_context&) = delete;
      graphics_context& operator=(const graphics_context&) = delete;

      bool init(uint32_t width, uint32_t height);
      void shutdown();
      void resize(uint32_t width, uint32_t height);

      void begin_frame(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 1.0f);
      void end_frame();

      VkImage get_vk_image() const;
      uint32_t width() const { return m_width; }
      uint32_t height() const { return m_height; }
  private:
      bool create_targets(uint32_t width, uint32_t height);

      Diligent::RefCntAutoPtr<Diligent::ITexture> m_render_target;
      Diligent::RefCntAutoPtr<Diligent::ITexture> m_depth_buffer;

      uint32_t m_width  = 0;
      uint32_t m_height = 0;
      bool m_is_initialized = false;
  };
}