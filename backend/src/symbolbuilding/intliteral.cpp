#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_F(IntLiteral)
    {
        if (sym && !std::holds_alternative<std::monostate>(sym->get().kind))
        {
            if (std::holds_alternative<std::shared_ptr<func_kind>>(
                    sym->get().kind))
            {
                std::shared_ptr<func_kind>& fk =
                    std::get<std::shared_ptr<func_kind>>(sym->get().kind);
                if (0 < fk->args.size() &&
                    std::holds_alternative<std::shared_ptr<range_kind>>(
                        fk->args.back().kind))
                {
                    auto& symref = std::get<std::shared_ptr<range_kind>>(
                        fk->args.back().kind);

                    symref->args.emplace_back(Symbol{{int_kind{}, std::string{},
                        {ast}, -1, false, symbolTable.symbolTableRef->id}});
                }
                else if (0 < fk->args.size() &&
                    std::holds_alternative<nil_kind>(fk->args.back().kind))
                {
                    return;
                }
            }
            else if (std::holds_alternative<std::shared_ptr<tuple_kind>>(
                         sym->get().kind))
            {
                std::shared_ptr<tuple_kind>& symref =
                    std::get<std::shared_ptr<tuple_kind>>(sym->get().kind);

                std::string ident{std::string{"intlit_" + emitChapelLine(ast)}};
                std::get<std::shared_ptr<kind_node_type>>(symref->retKind)
                    ->children.emplace_back(int_kind{});

                symref->args.emplace_back(Symbol{{int_kind{}, ident, {ast}, -1,
                    false, symbolTable.symbolTableRef->id}});
            }
            else if (std::holds_alternative<std::shared_ptr<array_kind>>(
                         sym->get().kind))
            {
                std::shared_ptr<array_kind>& symref =
                    std::get<std::shared_ptr<array_kind>>(sym->get().kind);

                // user has an array that's undefined type and
                // provided a literal declaration of the array
                //
                if (std::holds_alternative<std::shared_ptr<kind_node_type>>(
                        symref->retKind))
                {
                    std::string ident{
                        std::string{"intlit_" + emitChapelLine(ast)}};

                    if (std::holds_alternative<std::shared_ptr<domain_kind>>(
                            symref->args.back().kind))
                    {
                        std::get<std::shared_ptr<kind_node_type>>(
                            symref->retKind)
                            ->children.emplace_back(int_kind{});
                        auto& domk = std::get<std::shared_ptr<domain_kind>>(
                            symref->args.back().kind);
                        if (domk->args.size() &&
                            std::holds_alternative<std::shared_ptr<range_kind>>(
                                domk->args.back().kind))
                        {
                            auto& rngk = std::get<std::shared_ptr<range_kind>>(
                                domk->args.back().kind);
                            rngk->args.emplace_back(
                                Symbol{{int_kind{}, ident, {ast}, -1, false,
                                    symbolTable.symbolTableRef->id}});
                        }
                        else
                        {
                            // literal indicator
                            //
                            domk->args.emplace_back(
                                Symbol{{int_kind{}, ident, {ast}, -1, false,
                                    symbolTable.symbolTableRef->id}});
                        }
                    }
                }
            }
            else if (std::holds_alternative<std::shared_ptr<range_kind>>(
                         sym->get().kind))
            {
                std::string ident{std::string{"intlit_" + emitChapelLine(ast)}};
                auto& symref =
                    std::get<std::shared_ptr<range_kind>>(sym->get().kind);
                symref->args.push_back(Symbol{{int_kind{}, ident, {ast}, -1,
                    false, symbolTable.symbolTableRef->id}});
            }
        }
        else if (sym)
        {
            auto fsym = symbolTable.find(0, "int");

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