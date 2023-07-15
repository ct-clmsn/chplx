#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_F(Identifier)
    {
        std::string identifier_str{
            dynamic_cast<Identifier const*>(ast)->name().c_str()};

        if (sym && !std::holds_alternative<std::monostate>(sym->get().kind))
        {
            const std::size_t lutId = sym->get().scopeId;
            //std::holds_alternative<std::shared_ptr<func_kind>>(sym->get().kind) ?
            //   std::get<std::shared_ptr<func_kind>>(sym->get().kind)->lutId :
            //   sym->get().scopeId; // :
            //   0;
            if (std::holds_alternative<std::shared_ptr<array_kind>>(
                    sym->get().kind))
            {
                auto fsym = symbolTable.find(lutId, identifier_str);

                if (fsym)
                {
                    std::shared_ptr<array_kind>& symref =
                        std::get<std::shared_ptr<array_kind>>(sym->get().kind);

                    auto& domk = std::get<std::shared_ptr<domain_kind>>(
                        symref->args.back().kind);
                    auto& rngk = std::get<std::shared_ptr<range_kind>>(
                        domk->args.back().kind);
                    if (fsym->scopeId != 0 && rngk->args.size() < 2)
                    {
                        rngk->args.emplace_back(*fsym);
                    }
                    else
                    {
                        std::get<std::shared_ptr<array_kind>>(sym->get().kind)
                            ->retKind = fsym->kind;
                        sym->get().scopeId = symbolTable.symbolTableRef->id;
                    }
                }
            }
            else if (std::holds_alternative<std::shared_ptr<tuple_kind>>(
                         sym->get().kind))
            {
                auto fsym = symbolTable.find(lutId, identifier_str);
                if (fsym)
                {
                    std::shared_ptr<tuple_kind>& symref =
                        std::get<std::shared_ptr<tuple_kind>>(sym->get().kind);

                    std::get<std::shared_ptr<kind_node_type>>(symref->retKind)
                        ->children.emplace_back(fsym->kind);

                    //symref->args.emplace_back(
                    //   Symbol{{
                    //      fsym->kind,
                    //      fsym->identifier,
                    //      {ast}, -1, false, symbolTable.symbolTableRef->id
                    //   }});
                }
            }
            else if (std::holds_alternative<std::shared_ptr<func_kind>>(
                         sym->get().kind) &&
                !std::holds_alternative<std::shared_ptr<cxxfunc_kind>>(
                    sym->get().kind))
            {
                std::shared_ptr<func_kind>& fk =
                    std::get<std::shared_ptr<func_kind>>(sym->get().kind);

                std::optional<Symbol> fsym;
                symbolTable.find(sym->get().scopeId, identifier_str, fsym);
                //auto fsym = symbolTable.find(sym->get().scopeId, identifier_str);

                if (fsym.has_value())
                {
                    if (std::holds_alternative<std::shared_ptr<cxxfunc_kind>>(
                            fsym->kind))
                    {
                        return;
                    }
                    else if (0 < fk->args.size())
                    {
                        if (std::holds_alternative<nil_kind>(
                                fk->args.back().kind))
                        {
                            return;
                        }
                        else if (fk->args.back().kind.index() < 1)
                        {
                            fk->args.back().kind = fsym->kind;
                        }
                        else if (!std::holds_alternative<
                                     std::shared_ptr<func_kind>>(fsym->kind) &&
                            !std::holds_alternative<
                                std::shared_ptr<cxxfunc_kind>>(fsym->kind))
                        {
                            fk->retKind = fsym->kind;
                        }
                    }
                    else if (0 == fk->args.size())
                    {
                        fk->retKind = fsym->kind;
                    }
                }
            }
            else
            {
                auto fsym = symbolTable.find(lutId, identifier_str);
                if (std::holds_alternative<std::monostate>(sym->get().kind) &&
                    fsym.has_value())
                {
                    sym->get().kind =
                        std::holds_alternative<std::shared_ptr<func_kind>>(
                            fsym->kind) ?
                        std::get<std::shared_ptr<func_kind>>(fsym->kind)
                            ->retKind :
                        fsym->kind;
                    sym->get().scopeId = symbolTable.symbolTableRef->id;
                }
            }
        }
        else if (sym)
        {
            std::optional<Symbol> fsym{};
            symbolTable.find(identifier_str, fsym);
            if (fsym.has_value())
            {
                sym->get().kind = fsym->kind;
                sym->get().scopeId = symbolTable.symbolTableRef->id;
            }
        }
    }
}}}}    // namespace chpl::ast::visitors::hpx