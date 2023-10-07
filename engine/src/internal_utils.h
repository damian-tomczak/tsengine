#pragma once

#include "tsengine/utils.h"

#define TS_CATCH_FALLBACK                                                           \
    catch (const TSException&)                                                      \
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
    catch (const TSException&)                                                      \
    {                                                                               \
        cleanerFunction();                                                          \
                                                                                    \
    return TS_FAILURE;                                                              \
    }                                                                               \
    catch (const std::exception& e)                                                 \
    {                                                                               \
        ts::logger::error(e.what(), __FILE__, FUNCTION_SIGNATURE, __LINE__, false); \
                                                                                    \
        cleanerFunction();                                                          \
                                                                                    \
        return STL_FAILURE;                                                         \
    }                                                                               \
    catch (...)                                                                     \
    {                                                                               \
        cleanerFunction();                                                          \
                                                                                    \
        return UNKNOWN_FAILURE;                                                     \
    }
