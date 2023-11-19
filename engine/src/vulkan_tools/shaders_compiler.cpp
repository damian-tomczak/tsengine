#include "shaders_compiler.h"

#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"
#include "tsengine/logger.h"

namespace ts
{
inline namespace TS_VER
{
namespace
{
std::string readShader(const std::filesystem::path& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        TS_ERR(("Shader file can not be opened: " + path.string()).c_str());
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

glslang_stage_t getShaderStage(const std::filesystem::path& file)
{
    const auto extension = file.extension();

    if (extension == ".vert")
    {
        return GLSLANG_STAGE_VERTEX;
    }

    if (extension == ".frag")
    {
        return GLSLANG_STAGE_FRAGMENT;
    }

    return GLSLANG_STAGE_COUNT;
} // namespace

std::vector<uint32_t> processShader(const glslang_stage_t stage, const std::filesystem::path& filePath, const std::string& src)
{
    const glslang_input_t input{
        .language = GLSLANG_SOURCE_GLSL,
        .stage = stage,
        .client = GLSLANG_CLIENT_VULKAN,
        .client_version = GLSLANG_TARGET_VULKAN_1_1,
        .target_language = GLSLANG_TARGET_SPV,
        .target_language_version = GLSLANG_TARGET_SPV_1_3,
        .code = src.c_str(),
        .default_version = 100,
        .default_profile = GLSLANG_NO_PROFILE,
        .messages = GLSLANG_MSG_DEFAULT_BIT,
        .resource = glslang_default_resource(),
    };

#ifndef NDEBUG
    glslang_spv_options_s options{
        .generate_debug_info = true,
    };
#else
    glslang_spv_options_s options{};
#endif // !NDEBUG

    auto shader = glslang_shader_create(&input);

    // TODO: implement more complex logging system, taking into account also line highlighting
    auto loggerWrapper{[&](std::string_view errorTitle) -> void {
        std::ostringstream message;
        message
            << errorTitle
            << ": "
            << filePath.string()
            << "\n"
            << glslang_shader_get_info_log(shader)
            << "\n"
            << glslang_shader_get_info_debug_log(shader);

        TS_ERR(message.str().c_str());
    }};

    if (!glslang_shader_preprocess(shader, &input))
    {
        loggerWrapper("Shader preprocessing failed");
    }

    if (!glslang_shader_parse(shader, &input))
    {
        loggerWrapper("Shader parsing failed");
    }

    auto program = glslang_program_create();
    glslang_program_add_shader(program, shader);

    if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
    {
        loggerWrapper("Shader linking failed");
    }

    glslang_program_SPIRV_generate_with_options(program, stage, &options);

    std::vector<uint32_t> spirv(glslang_program_SPIRV_get_size(program));
    glslang_program_SPIRV_get(program, spirv.data());

    const auto spirvMessages = glslang_program_SPIRV_get_messages(program);

    if (spirvMessages != nullptr)
    {
        TS_ERR(("Glslang program spriv message: "s + spirvMessages).c_str());
    }

    glslang_program_delete(program);
    glslang_shader_delete(shader);

    return spirv;
}

std::vector<uint32_t> compileShaderFile(const std::filesystem::path& filePath)
{
    const auto src = readShader(filePath);
    if (src.empty())
    {
        TS_ERR(("Shader file is empty: " + filePath.string()).c_str());
    }

    const auto shaderStage = getShaderStage(filePath);

    if (shaderStage == GLSLANG_STAGE_COUNT)
    {
        TS_ERR(("Shader file extension isn't supported: " + filePath.string()).c_str());
    }

    return processShader(shaderStage, filePath, src.c_str());
}

void saveSPIRV(const std::filesystem::path& outputFilePath, const std::vector<uint32_t>& spirv)
{
    std::ofstream file{outputFilePath, std::ios::binary};
    if (!file.is_open())
    {
        TS_ERR(("Shader file can not be opened: " + outputFilePath.string()).c_str());
    }

    file.write(reinterpret_cast<const char*>(spirv.data()), spirv.size() * sizeof(uint32_t));

    if (file.bad())
    {
        TS_ERR(("Failed to write to the shader file: " + outputFilePath.string()).c_str());
    }

    file.close();
    if (file.fail())
    {
        TS_ERR(("Failed to close the shader file: " + outputFilePath.string()).c_str());
    }
}
} // namespace

void compileShaders(const std::filesystem::path shadersPath)
{
    if (!glslang_initialize_process())
    {
        TS_ERR("Glslang initialization failure");
    }

    if (!std::filesystem::is_directory(shadersPath))
    {
        TS_ERR(("Path couldn't be found: " + shadersPath.string()).c_str());
    }

    // TODO: compile only modified shaders
    size_t shadersFoundCount{};
    for (const auto& file : std::filesystem::recursive_directory_iterator(shadersPath))
    {
        if (file.is_directory() || (file.path().extension() == ".spirv") || (file.path().extension() == ".h"))
        {
            continue;
        }

        shadersFoundCount++;

#ifdef NDEBUG
        // Shader is already compiled
        if (std::filesystem::exists((file.path().string() + ".spirv")))
        {
            continue;
        }
#endif // NDEBUG

        const auto spriv = compileShaderFile(file.path());

        const auto outputFileName = file.path().string() + ".spirv";
        saveSPIRV(outputFileName, spriv);
    }

    if (shadersFoundCount == 0)
    {
        TS_WARN("No shaders found");
    }

    glslang_finalize_process();
}
} // namespace ver
} // namespace ts