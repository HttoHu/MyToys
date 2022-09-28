#pragma once
template <typename Func>
struct Guarder
{
public:
    Guarder(Func _f) : func(_f) {}
    ~Guarder()
    {
        func();
    }

private:
    Func func;
};