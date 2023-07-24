#include "tsengine/core.h"
#include "context.h"
#include "window.h"
#include "vulkan/vulkan_functions.h"
#include "tsengine/logger.h"
#include "mirror_view.h"
#include "headset.h"
#include "controllers.h"
#include "game_object.hpp"

unsigned tickCount{};
bool isAlreadyInitiated{};

std::mutex engineInit;

namespace ts
{
unsigned getTickCount()
{
    return tickCount;
}

int run(Engine* const pEngine) try
{
    std::lock_guard<std::mutex> _{engineInit};

    if (!pEngine)
    {
        LOGGER_ERR("game is unallocated");
    }

    if (isAlreadyInitiated)
    {
        LOGGER_ERR("game is already initiated");
    }

    unsigned width{1280}, height{720};
    pEngine->init(width, height);

    if (!std::filesystem::is_directory("assets"))
    {
        LOGGER_ERR("assets can not be found");
    }

    compileShaders("assets/shaders");

    Context ctx;
    ctx.createOpenXrContext();
    ctx.createVulkanContext();

    auto pWindow{Window::createWindowInstance(width, height)};
    MirrorView mirrorView{ctx, pWindow};
    ctx.createVkDevice(mirrorView.getSurface());
    Headset headset(ctx);
    headset.createRenderPass();
    headset.createXrSession();
    headset.createSwapchain();
    Controllers controllers(ctx.getXrInstance(), headset.getXrSession());
    controllers.setupControllers();

    Model
        gridModel,
        ruinsModel,
        carModelLeft,
        carModelRight,
        beetleModel,
        bikeModel,
        handModelLeft,
        handModelRight,
        logoModel;

    std::vector<Model*> models{
        &gridModel,
        &ruinsModel,
        &carModelLeft,
        &carModelRight,
        &beetleModel,
        &bikeModel,
        &handModelLeft,
        &handModelRight,
        &logoModel
    };

    math::Matrix4x4<> var{{{
        {1,2,3,4},
        {1,2,3,4},
        {1,2,3,4},
        {1,2,3,4}
        }}};

    std::cout << var[0][1];
    gridModel.worldMatrix = math::Matrix4x4<>::makeScalarMat(1.f);
    //carModelLeft.worldMatrix =
    //    math::rotate(math::translate(math::Matrix4x4<>::makeScalarMat(1.f), {-3.5f, 0.0f, -7.0f}), math::radians(75.0f), {0.0f, 1.0f, 0.0f});
    //carModelRight.worldMatrix =
    //    math::rotate(math::translate(math::Matrix4x4<>::makeScalarMat(1.f), {8.0f, 0.0f, -15.0f}), math::radians(-15.0f), {0.0f, 1.0f, 0.0f});
    //beetleModel.worldMatrix =
    //    math::rotate(math::translate(math::Matrix4x4<>::makeScalarMat(1.f), {-3.5f, 0.0f, -0.5f}), math::radians(-125.0f), {0.0f, 1.0f, 0.0f});
    logoModel.worldMatrix = math::translate(math::Matrix4x4<>::makeScalarMat(1.0f), {0.0f, 3.0f, -10.0f});

    isAlreadyInitiated = true;
    LOGGER_LOG("tsengine initialization completed successfully");

    pWindow->show();
    while (true /*!pEngine->tick()*/)
    {
        auto message{pWindow->peekMessage()};
        (void)message;
        if (false)
        {
            pEngine->onMouseMove(-1, -1, -1, -1);
        }

        if (false)
        {
            pEngine->onMouseButtonClick({}, false);
        }

        if (false)
        {
            pEngine->onKeyPressed({});
        }

        if (false)
        {
            pEngine->onKeyReleased({});
        }
    }

    pEngine->close();
    ctx.sync();
    isAlreadyInitiated = false;

    return EXIT_SUCCESS;
}
TS_CATCH_FALLBACK
} // namespace ts
