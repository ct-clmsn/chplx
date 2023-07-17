#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(StringLiteral)
    {
        if (!(sym && 0 < sym->get().kind.index()))
        {
            if (sym &&
                std::holds_alternative<std::shared_ptr<func_kind>>(
                    sym->get().kind))
            {
                std::shared_ptr<func_kind>& fk =
                    std::get<std::shared_ptr<func_kind>>(sym->get().kind);
                if (0 < fk->args.size() &&
                    std::holds_alternative<nil_kind>(fk->args.back().kind))
                {
                    return;
                }
            }

            std::optional<Symbol> fsym{};
            symbolTable.find(0, "string", fsym);

            if (fsym.has_value())
            {
                sym->get().kind = fsym->kind;
                sym->get().literal.push_back(ast);
            }
        }
    }
}}}}    // namespace chpl::ast::visitors::hpx