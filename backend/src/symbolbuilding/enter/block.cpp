#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(Block)
    {
        if (sym &&
            !std::holds_alternative<std::shared_ptr<module_kind>>(
                sym->get().kind))
        {    // 0 < sym->get().kind.index()) {
            if (std::holds_alternative<std::shared_ptr<func_kind>>(
                    sym->get().kind))
            {
                std::shared_ptr<func_kind>& fk =
                    std::get<std::shared_ptr<func_kind>>(sym->get().kind);
                if (0 < fk->args.size() &&
                    (fk->args.back().identifier == "nil"))
                {
                    // this 'function' is really a stand-in scope for the following
                    // expressions: for, forall, conditional (if/else) clause
                    //
                    return;
                }
                else
                {
                    // the block node of the chapel ast indicates the end of a function
                    // declaration's argument list; the following empty argument is
                    // state information about the ast traversal
                    //
                    fk->args.emplace_back(
                        Symbol{{nil_kind{}, std::string{"nil"}, {}, -1, false,
                            symbolTable.symbolTableRef->id}});
                }
            }
        }
        else
        {
            symstack.emplace_back(
                Symbol{{std::make_shared<func_kind>(func_kind{
                            {symbolTable.symbolTableRef->id, {}, {}, {}}}),
                    std::string{"else" + emitChapelLine(ast)}, {}, -1, false,
                    symbolTable.symbolTableRef->id}});

            std::shared_ptr<SymbolTable::SymbolTableNode> prevSymbolTableRef =
                symbolTable.symbolTableRef;
            const std::size_t parScope = symbolTable.symbolTableRef->id;
            symbolTable.pushScope();
            sym.reset();
            sym = symstack.back();

            std::shared_ptr<func_kind>& fk =
                std::get<std::shared_ptr<func_kind>>(sym->get().kind);

            fk->symbolTableSignature = sym->get().identifier;
            // func_kind.lutId = the scope where the function's symboltable references
            //
            fk->lutId = symbolTable.symbolTableRef->id;

            symbolTable.parentSymbolTableId = parScope;
            symbolTable.symbolTableRef->parent = prevSymbolTableRef;
        }
    }

}}}}    // namespace chpl::ast::visitors::hpx
