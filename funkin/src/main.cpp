/*
FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include <QGuiApplication>
#include <QQuickWindow>
#include <QQuickGraphicsDevice>
#include <QVulkanInstance>

#include "render/graphics/graphics_context.hpp"
#include "render/graphics/graphics_device.hpp"
#include "render/graphics/graphics_viewport.hpp"
#include "utils/log.hpp"

int main(int argc, char* argv[]) {
    qputenv("QSG_RHI_BACKEND", "vulkan");
    qputenv("QSG_INFO", "1");

    QGuiApplication app(argc, argv);

    auto& graphics_device = Funkin::Render::Graphics::graphics_device::instance();
    if (!graphics_device.init(false)) {
        LOG_CRIT("device init FAILED");
        return -1;
    }

    auto handles = graphics_device.get_native_vk_handles();
    Funkin::Render::Graphics::graphics_context graphics_context;
    if (!graphics_context.init(1280, 720)) {
        LOG_CRIT("context init FAILED");
        graphics_device.shutdown();
        return -1;
    }

    int result = 0;

    {
        QVulkanInstance vulkan_instance;
        vulkan_instance.setVkInstance(static_cast<VkInstance>(handles.instance));
        if (!vulkan_instance.create()) {
            LOG_CRIT("q_vulkan_instance creation FAILED");
            graphics_device.shutdown();
            return -1;
        }

        QQuickWindow quick_window;
        quick_window.setVulkanInstance(&vulkan_instance);

        QQuickGraphicsDevice quick_graphics_device = QQuickGraphicsDevice::fromDeviceObjects(
            static_cast<VkPhysicalDevice>(handles.physical_device),
            static_cast<VkDevice>(handles.device),
            static_cast<int>(handles.queue_family_index),
            static_cast<int>(handles.queue_index)
        );
        quick_window.setGraphicsDevice(quick_graphics_device);

        auto* viewport = new Funkin::Render::Graphics::graphics_viewport();
        viewport->set_graphics_context(&graphics_context);
        viewport->setParentItem(quick_window.contentItem());
        viewport->setSize(QSizeF(1280, 720));

        QObject::connect(&quick_window, &QQuickWindow::beforeRenderPassRecording, &quick_window, [&graphics_context]() {
            graphics_context.begin_frame(0.2f, 0.4f, 0.8f, 1.0f);
            graphics_context.end_frame();
        }, Qt::DirectConnection);

        QObject::connect(&quick_window, &QQuickWindow::afterRendering, &quick_window, [&quick_window]() {
            quick_window.update();
        }, Qt::QueuedConnection);

        quick_window.resize(1280, 720);
        quick_window.show();
        result = app.exec();
    }

    graphics_context.shutdown();
    graphics_device.shutdown();
    return result;
}