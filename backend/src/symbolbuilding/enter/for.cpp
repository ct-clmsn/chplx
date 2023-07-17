#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(For)
    {
        // symbol.scopePtr = the scope where the function is defined (equivalent to a lutId)
        //
        symstack.emplace_back(
            Symbol{{std::make_shared<func_kind>(func_kind{
                        {symbolTable.symbolTableRef->id, {}, {}, {}}}),
                std::string{"for" + emitChapelLine(ast)}, {}, -1, false,
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

        symnode = const_cast<uast::AstNode*>(ast);
    }

}}}}    // namespace chpl::ast::visitors::hpx
