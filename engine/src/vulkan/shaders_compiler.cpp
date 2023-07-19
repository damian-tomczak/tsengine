#include "shaders_compiler.h"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"
#include "tsengine/logger.h"
#include <fstream>

namespace
{
std::string readShader(const std::filesystem::path& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        LOGGER_ERR("shader couldn't be opened");
    }

    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

glslang_stage_t getShaderStage(const std::filesystem::path& file)
{
    auto extension{ file.extension() };

    if (extension == ".vert")
    {
        return GLSLANG_STAGE_VERTEX;
    }

    if (extension == ".frag")
    {
        return GLSLANG_STAGE_FRAGMENT;
    }

    return GLSLANG_STAGE_COUNT;
}

std::vector<uint32_t> processShader(const glslang_stage_t stage, const std::string& src)
{
    const glslang_input_t input
    {
        .language{ GLSLANG_SOURCE_GLSL },
        .stage{ stage },
        .client{ GLSLANG_CLIENT_VULKAN },
        .client_version{ GLSLANG_TARGET_VULKAN_1_1 },
        .target_language{ GLSLANG_TARGET_SPV },
        .target_language_version{ GLSLANG_TARGET_SPV_1_3 },
        .code{ src.c_str() },
        .default_version{ 100 },
        .default_profile{ GLSLANG_NO_PROFILE },
        .messages{ GLSLANG_MSG_DEFAULT_BIT },
        .resource{ glslang_default_resource() }
    };

    auto shader{ glslang_shader_create(&input) };

    auto loggerWrapper{ [&shader](std::string errorTitle) -> void {
        LOGGER_ERR((
            errorTitle + "\n" +
            glslang_shader_get_info_log(shader) + "\n" +
            glslang_shader_get_info_debug_log(shader)).c_str());
    }};

    if (!glslang_shader_preprocess(shader, &input))
    {
        loggerWrapper("Shader preprocessing failed");
    }

    if (!glslang_shader_parse(shader, &input))
    {
        loggerWrapper("Shader parsing failed");
    }

    auto program{ glslang_program_create() };
    glslang_program_add_shader(program, shader);

    if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
    {
        loggerWrapper("Shader linking failed");
    }

    glslang_program_SPIRV_generate(program, stage);

    std::vector<uint32_t> spirv(glslang_program_SPIRV_get_size(program));
    glslang_program_SPIRV_get(program, spirv.data());

    auto spirvMessages{ glslang_program_SPIRV_get_messages(program) };

    if (spirvMessages != nullptr)
    {
        LOGGER_ERR((std::string{ "glslang program spriv message: " } + spirvMessages).c_str());
    }

    glslang_program_delete(program);
    glslang_shader_delete(shader);

    return spirv;
}

std::vector<uint32_t> compileShaderFile(const std::filesystem::path& file)
{
    auto src{ readShader(file) };
    if (src.empty())
    {
        LOGGER_ERR((file.string() + " is empty").c_str());
    }

    auto shaderStage{ getShaderStage(file) };

    if (shaderStage == GLSLANG_STAGE_COUNT)
    {
        LOGGER_ERR((file.string() + "isn't supported").c_str());
    }
    return processShader(shaderStage, src.c_str());
}

void saveSPIRV(const std::filesystem::path& outputFileName, const std::vector<uint32_t>& spirv)
{
    std::ofstream file(outputFileName, std::ios::binary);
    if (!file.is_open())
    {
        LOGGER_ERR((outputFileName.string() + " can not be opened").c_str());
    }

    std::copy(spirv.begin(), spirv.end(), std::ostreambuf_iterator<char>(file));
}
}

namespace ts
{
void compileShaders(std::string_view shadersPath)
{
    if (!glslang_initialize_process())
    {
        LOGGER_ERR("glslang initialization failure");
    }

    if (!std::filesystem::is_directory(shadersPath))
    {
        LOGGER_ERR((std::string{ shadersPath } + " path couldn't be found").c_str());
    }

    auto compiledShadersNumber{ 0 };

    for (const auto& file : std::filesystem::recursive_directory_iterator(shadersPath))
    {
        if (file.is_directory() || file.path().extension() == ".spirv")
        {
            continue;
        }

        auto spriv{ compileShaderFile(file.path()) };

        auto outputFileName{ std::string{ shadersPath } + "/" + file.path().filename().string() + ".spirv" };
        saveSPIRV(outputFileName, spriv);

        compiledShadersNumber++;
    }

    if (compiledShadersNumber == 0)
    {
        LOGGER_WARN("number of compiled shaders is 0");
    }

    glslang_finalize_process();
}
}