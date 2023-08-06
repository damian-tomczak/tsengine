#pragma once

#define NOT_COPYABLE(TypeName)                     \
    TypeName(const TypeName&) = delete;            \
    TypeName& operator=(const TypeName&) = delete;

#define NOT_MOVEABLE(TypeName)                \
    TypeName(TypeName&&) = delete;            \
    TypeName& operator=(TypeName&&) = delete;

#define NOT_COPYABLE_AND_MOVEABLE(TypeName) \
    NOT_COPYABLE(TypeName)                  \
    NOT_MOVEABLE(TypeName)

#define TS_CATCH_FALLBACK                                                           \
    catch (const TSException&)                                                      \
    {                                                                               \
        return TS_FAILURE;                                                          \
    }                                                                               \
    catch (const std::exception& e)                                                 \
    {                                                                               \
        ts::logger::error(e.what(), __FILE__, FUNCTION_SIGNATURE, __LINE__, false); \
        return STL_FAILURE;                                                         \
    }                                                                               \
    catch (...)                                                                     \
    {                                                                               \
        return UNKNOWN_FAILURE;                                                     \
    }

#define XSTR(x) #x
#define STR(x) XSTR(x)

class TSException : public std::exception
{
public:
    TSException() {}
};

template<>
struct std::hash<std::string_view> final
{
    constexpr size_t operator()(std::string_view s) const noexcept
    {
        return fnv1a_32(s, s.length());
    }

private:
    // Inspired by https://gist.github.com/Lee-R/3839813?permalink_comment_id=4018536#gistcomment-4018536
    constexpr size_t fnv1a_32(std::string_view s, size_t count) const noexcept
    {
        return count ? (fnv1a_32(s, count - 1) ^ s[count - 1]) * 16777619u : 2166136261u;
    }
};
