#pragma once

#define NOT_COPYABLE(TypeName)          \
    TypeName(const TypeName&) = delete; \
    TypeName& operator=(TypeName&) = delete;

#define NOT_MOVEABLE(TypeName)           \
    TypeName(const TypeName&&) = delete; \
    TypeName& operator=(TypeName&&) = delete;

#define NOT_COPYABLE_AND_MOVEABLE(TypeName) \
    NOT_COPYABLE(TypeName)                  \
    NOT_MOVEABLE(TypeName)

#define XSTR(x) #x
#define STR(x) XSTR(x)