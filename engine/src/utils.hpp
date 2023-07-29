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
