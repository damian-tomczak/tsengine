#pragma once

#include "tsengine/utils.hpp"

#define TS_CATCH_FALLBACK                                                           \
    catch (const Exception&)                                                        \
    {                                                                               \
        return TS_FAILURE;                                                          \
    }                                                                               \
    catch (const std::exception& e)                                                 \
    {                                                                               \
        ts::logger::error(e.what(), __FILE__, FUNCTION_SIGNATURE, __LINE__, false); \
                                                                                    \
        return STL_FAILURE;                                                         \
    }                                                                               \
    catch (...)                                                                     \
    {                                                                               \
        return UNKNOWN_FAILURE;                                                     \
    }

#define TS_CATCH_FALLBACK_WITH_CLEANER(cleanerFunction)                             \
    catch (const Exception&)                                                        \
    {                                                                               \
        cleanerFunction();                                                          \
                                                                                    \
        return TS_FAILURE;                                                          \
    }                                                                               \
    catch (const std::exception& e)                                                 \
    {                                                                               \
        ts::logger::error(e.what(), __FILE__, FUNCTION_SIGNATURE, __LINE__, false); \
                                                                                    \
        cleanerFunction();                                                          \
                                                                                    \
        return TS_STL_FAILURE;                                                      \
    }                                                                               \
    catch (...)                                                                     \
    {                                                                               \
        cleanerFunction();                                                          \
                                                                                    \
        return TS_UNKNOWN_FAILURE;                                                  \
    }

