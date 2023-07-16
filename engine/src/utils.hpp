#pragma once

#define NOT_COPYABLE(TypeName)                    \
    TypeName(const TypeName&) = delete;           \
    TypeName& operator=(TypeName&) = delete;

#define NOT_MOVEABLE(TypeName)                     \
    TypeName(TypeName&&) = delete;           \
    TypeName& operator=(TypeName&&) = delete;

#define NOT_COPYABLE_AND_MOVEABLE(TypeName) \
    NOT_COPYABLE(TypeName)                  \
    NOT_MOVEABLE(TypeName)

#define XSTR(x) #x
#define STR(x) XSTR(x)

namespace ts
{
template<typename DerivedClass>
class Singleton
{
    NOT_COPYABLE(Singleton);

public:
    static DerivedClass& getInstance()
    {
        static DerivedClass instance;
        return instance;
    }

protected:
    Singleton() = default;
};

namespace utils
{
inline std::vector<std::string> unpackExtensionString(const std::string& str)
{
    std::vector<std::string> result;
    std::istringstream stream(str);
    std::string extension;

    while (getline(stream, extension, ' '))
    {
        result.emplace_back(std::move(extension));
    }

    return result;
}
} // namespace utils
} // namespace ts
