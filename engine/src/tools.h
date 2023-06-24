#pragma once

#define NOT_COPYABLE(TypeName)                    \
    TypeName(const TypeName&) = delete;           \
    TypeName& operator=(const TypeName&) = delete;

#define NOT_MOVEABLE(TypeName)                     \
    TypeName(const TypeName&&) = delete;           \
    TypeName& operator=(const TypeName&&) = delete;

#define NOT_COPYABLE_AND_MOVEABLE(TypeName) \
    NOT_COPYABLE(TypeName)                  \
    NOT_MOVEABLE(TypeName)

#define XSTR(x) #x
#define STR(x) XSTR(x)

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

#define SINGLETON_BODY(DerivedClass)      \
private:                                  \
    friend class Singleton<DerivedClass>; \
    DerivedClass() = default;             \