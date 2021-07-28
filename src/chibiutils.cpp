
#include "chibiutils.h"

sym2::PreservedSexp::PreservedSexp(sexp ctx, sexp what)
    : ctx{ctx}
    , what{what}
{
    preservation_list.var = &this->what;
    preservation_list.next = sexp_context_saves(ctx);
    sexp_context_saves(ctx) = &preservation_list;
}

sym2::PreservedSexp::PreservedSexp(PreservedSexp&& other) noexcept
    : ctx{other.ctx}
    , what{other.what}
    , preservation_list{other.preservation_list}
{
    other.what = nullptr;
    other.ctx = nullptr;
}

sym2::PreservedSexp& sym2::PreservedSexp::operator=(PreservedSexp&& other) noexcept
{
    if (this != &other) {
        ctx = other.ctx;
        what = other.what;
        preservation_list = other.preservation_list;

        other.what = nullptr;
        other.ctx = nullptr;
    }

    return *this;
}

sym2::PreservedSexp::~PreservedSexp()
{
    if (what != nullptr && ctx != nullptr)
        sexp_context_saves(ctx) = preservation_list.next;
}

const sexp& sym2::PreservedSexp::get() const
{
    return what;
}
