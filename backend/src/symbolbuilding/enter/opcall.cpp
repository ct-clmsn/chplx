#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(OpCall)
    {
        if (sym.has_value() &&
            std::holds_alternative<std::monostate>(sym->get().kind))
        {
            sym->get().kind = expr_kind{};
        }
    }

}}}}    // namespace chpl::ast::visitors::hpx
