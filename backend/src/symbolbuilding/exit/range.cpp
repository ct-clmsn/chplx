#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_EXIT_H_F(Range)
    {
        if (sym && 0 < sym->get().kind.index() &&
            std::holds_alternative<std::shared_ptr<range_kind>>(
                sym->get().kind))
        {
            symbolTable.addEntry(sym->get().identifier, *sym);
            sym.reset();
            symnode = nullptr;
        }
    }

}}}}    // namespace chpl::ast::visitors::hpx
