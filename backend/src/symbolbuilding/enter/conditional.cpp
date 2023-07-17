#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(Conditional)
    {
        if (!(sym &&
                !std::holds_alternative<std::shared_ptr<module_kind>>(
                    sym->get().kind)))
        {
            // symbol.scopePtr = the scope where the function is defined (equivalent to a lutId)
            //
            symstack.emplace_back(
                Symbol{{std::make_shared<func_kind>(func_kind{
                            {symbolTable.symbolTableRef->id, {}, {}, {}}}),
                    std::string{"if" + emitChapelLine(ast)}, {}, -1, false,
                    symbolTable.symbolTableRef->id}});

            std::shared_ptr<SymbolTable::SymbolTableNode> prevSymbolTableRef =
                symbolTable.symbolTableRef;
            const std::size_t parScope = symbolTable.symbolTableRef->id;
            symbolTable.pushScope();
            sym.reset();
            sym = symstack.back();

            std::shared_ptr<func_kind>& fk =
                std::get<std::shared_ptr<func_kind>>(sym->get().kind);

            fk->symbolTableSignature = (sym->get().identifier);
            // func_kind.lutId = the scope where the function's symboltable references
            //
            fk->lutId = symbolTable.symbolTableRef->id;

            symbolTable.parentSymbolTableId = parScope;
            symbolTable.symbolTableRef->parent = prevSymbolTableRef;

            symnode = const_cast<uast::AstNode*>(ast);
        }
        // inside a for loop or a function
        //
        else
        {
            // nested conditionals have an additional block node before the conditional node in the ast
            // this logic detects the additional block and removes it from the symbol table
            //
            if (sym->get().identifier == symstack.back().identifier &&
                sym->get().identifier.find("else") != std::string::npos)
            {
                symstack.pop_back();
                symbolTable.lut.pop_back();
                symbolTable.popScope();
            }

            std::shared_ptr<SymbolTable::SymbolTableNode> prevSymbolTableRef =
                symbolTable.symbolTableRef;
            const std::size_t parScope = symbolTable.symbolTableRef->id;

            symstack.emplace_back(
                Symbol{{std::make_shared<func_kind>(func_kind{
                            {symbolTable.symbolTableRef->id, {}, {}, {}}}),
                    std::string{"if" + emitChapelLine(ast)}, {}, -1, false,
                    symbolTable.symbolTableRef->id}});

            symbolTable.pushScope();
            sym.reset();
            sym = symstack.back();

            std::shared_ptr<func_kind>& fk =
                std::get<std::shared_ptr<func_kind>>(sym->get().kind);

            fk->symbolTableSignature = (sym->get().identifier);
            // func_kind.lutId = the scope where the function's symboltable references
            //
            fk->lutId = symbolTable.symbolTableRef->id;

            symbolTable.parentSymbolTableId = parScope;
            symbolTable.symbolTableRef->parent = prevSymbolTableRef;

            symnode = const_cast<uast::AstNode*>(ast);
        }
    }

}}}}    // namespace chpl::ast::visitors::hpx
