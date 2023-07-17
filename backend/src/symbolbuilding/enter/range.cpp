#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(Range)
    {
        if (sym && !std::holds_alternative<std::monostate>(sym->get().kind))
        {
            if (std::holds_alternative<std::shared_ptr<array_kind>>(
                    sym->get().kind))
            {
                std::string ident{std::string{"range_" + emitChapelLine(ast)}};

                auto& arrk =
                    std::get<std::shared_ptr<array_kind>>(sym->get().kind);
                auto& domk = std::get<std::shared_ptr<domain_kind>>(
                    arrk->args.back().kind);
                domk->args.emplace_back(
                    Symbol{{std::make_shared<range_kind>(range_kind{{}}), ident,
                        {}, -1, false, symbolTable.symbolTableRef->id}});
            }
            else if (std::holds_alternative<std::shared_ptr<func_kind>>(
                         sym->get().kind) &&
                (sym->get().identifier.find("for") != std::string::npos))
            {
                std::shared_ptr<func_kind>& fk =
                    std::get<std::shared_ptr<func_kind>>(sym->get().kind);

                std::string ident{std::string{"range_" + emitChapelLine(ast)}};
                fk->args.emplace_back(
                    Symbol{{std::make_shared<range_kind>(range_kind{{}}), ident,
                        {}, -1, false, symbolTable.symbolTableRef->id}});

                symbolTable.addEntry(fk->lutId, ident, fk->args.back());

                sym.reset();
                sym = symstack.back();
                symnode = const_cast<uast::AstNode*>(ast);
            }
        }
        else
        {
            symstack.emplace_back(
                Symbol{{std::make_shared<range_kind>(range_kind{{}}),
                    std::string{"range_" + emitChapelLine(ast)}, {}, -1, false,
                    symbolTable.symbolTableRef->id}});

            sym.reset();
            sym = symstack.back();
            symnode = const_cast<uast::AstNode*>(ast);
        }
    }
}}}}    // namespace chpl::ast::visitors::hpx