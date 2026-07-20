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

namespace Funkin::Render::Graphics {
  class graphics_context {
  public:
      graphics_context() = default;
      ~graphics_context();

      graphics_context(const graphics_context&) = delete;
      graphics_context& operator=(const graphics_context&) = delete;

      bool init(void* native_window_handle, uint16_t w, uint16_t h);
      void shutdown();

      void resize(uint16_t w, uint16_t h);
      void begin_frame(float r, float g, float b, float a);
      void present();

      bool is_initialized() const { return m_is_initialized; };

  private:
      Diligent::RefCntAutoPtr<Diligent::ISwapChain> m_swap_chain;
      bool m_is_initialized = false;
  };
}