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
        LOGGER_ERR(("shader file can not be opened" + path.string()).c_str());
    }

    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

glslang_stage_t getShaderStage(const std::filesystem::path& file)
{
    auto extension{file.extension()};

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
        .resource = glslang_default_resource()
    };

    auto shader = glslang_shader_create(&input);

    // TODO: implement more complex logging system, taking into account also line highlighting
    auto loggerWrapper{[&](const std::string& errorTitle) -> void {
        std::ostringstream message;
        message
            << errorTitle
            << ": "
            << filePath.string()
            << "\n"
            << "compilation info: "
            << glslang_shader_get_info_log(shader)
            << "\n"
            << glslang_shader_get_info_debug_log(shader);

        LOGGER_ERR(message.str().c_str());
    }};

    if (!glslang_shader_preprocess(shader, &input))
    {
        loggerWrapper("shader preprocessing failed");
    }

    if (!glslang_shader_parse(shader, &input))
    {
        loggerWrapper("shader parsing failed");
    }

    auto program{glslang_program_create()};
    glslang_program_add_shader(program, shader);

    if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
    {
        loggerWrapper("shader linking failed");
    }

    glslang_program_SPIRV_generate(program, stage);

    std::vector<uint32_t> spirv(glslang_program_SPIRV_get_size(program));
    glslang_program_SPIRV_get(program, spirv.data());

    auto spirvMessages{glslang_program_SPIRV_get_messages(program)};

    if (spirvMessages != nullptr)
    {
        LOGGER_ERR((std::string("glslang program spriv message: ") + spirvMessages).c_str());
    }

    glslang_program_delete(program);
    glslang_shader_delete(shader);

    return spirv;
}

std::vector<uint32_t> compileShaderFile(const std::filesystem::path& filePath)
{
    auto src = readShader(filePath);
    if (src.empty())
    {
        LOGGER_ERR(("shader file is empty: " + filePath.string()).c_str());
    }

    const auto shaderStage{getShaderStage(filePath)};

    if (shaderStage == GLSLANG_STAGE_COUNT)
    {
        LOGGER_ERR(("shader file extension isn't supported: " + filePath.string()).c_str());
    }

    return processShader(shaderStage, filePath, src.c_str());
}

void saveSPIRV(const std::filesystem::path& outputFilePath, const std::vector<uint32_t>& spirv)
{
    std::ofstream file{outputFilePath, std::ios::binary};
    if (!file.is_open())
    {
        LOGGER_ERR(("shader file can not be opened: " + outputFilePath.string()).c_str());
    }

    file.write(reinterpret_cast<const char*>(spirv.data()), spirv.size() * sizeof(uint32_t));

    if (file.bad())
    {
        LOGGER_ERR(("failed to write to the shader file: " + outputFilePath.string()).c_str());
    }

    file.close();
    if (file.fail())
    {
        LOGGER_ERR(("failed to close the shader file: " + outputFilePath.string()).c_str());
    }
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
        LOGGER_ERR((std::string{shadersPath} + " path couldn't be found").c_str());
    }

    size_t shadersFoundNumber{};
    for (const auto& file : std::filesystem::recursive_directory_iterator(shadersPath))
    {
        if (file.is_directory() || (file.path().extension() == ".spirv"))
        {
            continue;
        }

        shadersFoundNumber++;

        if (std::filesystem::exists((file.path().string() + ".spirv")))
        {
            continue;
        }

        auto spriv{compileShaderFile(file.path())};

        auto outputFileName{file.path().string() + ".spirv"};
        saveSPIRV(outputFileName, spriv);
    }

    if (shadersFoundNumber == 0)
    {
        LOGGER_WARN("no shaders found");
    }

    glslang_finalize_process();
}
}