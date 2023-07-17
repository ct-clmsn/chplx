#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(RealLiteral)
    {
        if (!(sym && 0 < sym->get().kind.index()))
        {
            if (std::holds_alternative<std::shared_ptr<func_kind>>(
                    sym->get().kind))
            {
                std::shared_ptr<func_kind>& fk =
                    std::get<std::shared_ptr<func_kind>>(sym->get().kind);
                if (std::holds_alternative<nil_kind>(fk->args.back().kind))
                {
                    return;
                }
            }
            else if (std::holds_alternative<std::shared_ptr<tuple_kind>>(
                         sym->get().kind))
            {
                std::shared_ptr<tuple_kind>& symref =
                    std::get<std::shared_ptr<tuple_kind>>(sym->get().kind);

                std::string ident{
                    std::string{"reallit_" + emitChapelLine(ast)}};
                std::get<std::shared_ptr<kind_node_type>>(symref->retKind)
                    ->children.emplace_back(real_kind{});

                symref->args.emplace_back(Symbol{{real_kind{}, ident, {ast}, -1,
                    false, symbolTable.symbolTableRef->id}});
            }
            else if (std::holds_alternative<std::shared_ptr<func_kind>>(
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

            auto fsym = symbolTable.find("real");

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