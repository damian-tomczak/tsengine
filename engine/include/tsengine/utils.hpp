#pragma once

#define NOT_COPYABLE(TypeName)                     \
    TypeName(const TypeName&) = delete;            \
    TypeName& operator=(const TypeName&) = delete;

#define NOT_MOVEABLE(TypeName)                \
    TypeName(TypeName&&) = delete;            \
    TypeName& operator=(TypeName&&) = delete;

#define NOT_COPYABLE_AND_MOVEABLE(TypeName) \
    NOT_COPYABLE(TypeName);                 \
    NOT_MOVEABLE(TypeName);

#define SINGLETON_BODY(TypeName)   \
    NOT_COPYABLE(TypeName);        \
    private: TypeName() = default; \
    friend Singleton<TypeName>;

#define STR(x) XSTR(x)
#define XSTR(x) #x

template<>
struct std::hash<std::string_view> final
{
    constexpr size_t operator()(const std::string_view s) const noexcept
    {
        return fnv1a_32(s, s.length());
    }

private:
    // Inspired by https://gist.github.com/Lee-R/3839813?permalink_comment_id=4018536#gistcomment-4018536
    constexpr size_t fnv1a_32(const std::string_view s, const size_t count) const noexcept
    {
        return count ? (fnv1a_32(s, count - 1) ^ s[count - 1]) * 16777619u : 2166136261u;
    }
};

namespace ts
{
enum
{
    TS_SUCCESS,
    TS_FAILURE,
    STL_FAILURE,
    UNKNOWN_FAILURE
};

class Exception : public std::exception
{
public:
    Exception() : mMessage{"Exception message not provided"} {}
    Exception(std::string_view message) : mMessage{message} {}
    
    const char* what() const noexcept override { return mMessage.data(); }

private:
    const std::string_view mMessage;
};

template<typename DerivedClass>
class Singleton
{
    NOT_COPYABLE(Singleton);

public:
    static auto& getInstance()
    {
        std::lock_guard _(mutex);
        static DerivedClass instance;
        return instance;
    }

protected:
    Singleton() = default;

private:
    inline static std::mutex mutex;
};
} // namespace ts