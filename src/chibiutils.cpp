
#include "chibiutils.h"

sym2::PreservedSexp::Payload::Payload(sexp ctx, sexp what)
    : ctx{ctx}
    , what{what}
{
    preservation_list.var = &this->what;
    preservation_list.next = sexp_context_saves(ctx);
    sexp_context_saves(ctx) = &preservation_list;
}

sym2::PreservedSexp::Payload::~Payload()
{
    sexp_context_saves(ctx) = preservation_list.next;
}

sym2::PreservedSexp::PreservedSexp(sexp ctx, sexp what)
    : handle{std::make_unique<Payload>(ctx, what)}
{}

const sexp& sym2::PreservedSexp::get() const
{
    return handle->what;
}
