#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_F(BoolLiteral)
    {
        if (!(sym && !std::holds_alternative<std::monostate>(sym->get().kind) &&
                0 < sym->get().kind.index()))
        {
            if (std::holds_alternative<std::shared_ptr<func_kind>>(
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

            auto fsym = symbolTable.find("bool");
            if (fsym.has_value())
            {
                sym->get().kind = (fsym->kind);
                if (sym->get().literal.size())
                {
                    sym->get().literal.push_back(ast);
                }
                else
                {
                    sym->get().literal = {
                        ast,
                    };
                }
            }
        }
    }
}}}}    // namespace chpl::ast::visitors::hpx