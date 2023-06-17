#include "shaders.h"

#include "pch.h"

#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"

#include <fstream>

namespace
{
std::string readShader(const std::filesystem::path& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        // TODO: logger
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

    throw std::exception{}; // TODO:: to supress warning
    // TODO: logger
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

    auto loggerWrapper{ [](std::string&& errorTitle) -> void {
        // TODO:: logger
        // glslang_shader_get_info_log(shader)
        // glslang_shader_get_info_debug_log(shader)
        throw std::exception{};
    } };

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

    if (spirvMessages)
    {
        // TODO: logger
    }

    glslang_program_delete(program);
    glslang_shader_delete(shader);

    return spirv;
}

std::vector<uint32_t> compileShaderFile(const std::filesystem::path& path)
{
    auto src{ readShader(path) };
    if (!src.empty())
    {
        // TODO: logger
    }

    return processShader(getShaderStage(path), src.c_str());
}

void saveSPIRV(std::filesystem::path outputFileName, std::vector<uint32_t> spirv)
{
    std::ofstream file(outputFileName, std::ios::binary);
    if (!file)
    {
        throw std::filesystem::filesystem_error("Can't open output file", std::error_code());
    }

    std::copy(spirv.begin(), spirv.end(), std::ostreambuf_iterator<char>(file));
}
} // namespace

namespace ts
{
void compileShaders()
{
    if (!glslang_initialize_process())
    {
        // TODO: logger
    }

    constexpr auto shadersPath{ "assets/shaders" };

    if (!std::filesystem::is_directory(shadersPath))
    {
        // TODO: logger
    }

    for (const auto& file : std::filesystem::directory_iterator(shadersPath))
    {
        auto spriv{ compileShaderFile(file.path()) };

        saveSPIRV(shadersPath + std::string("/") + file.path().filename().string() + ".spirv", spriv);
    }

    glslang_finalize_process();
}
} // namespace ts