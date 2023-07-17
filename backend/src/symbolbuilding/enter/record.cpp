#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(Record)
    {
        std::string lookup = static_cast<Record const*>(ast)->name().str();

        // symbol.scopePtr = the scope where the function is defined (equivalent to a lutId)
        //
        symstack.emplace_back(
            Symbol{{std::make_shared<record_kind>(record_kind{
                        symbolTable.symbolTableRef->id, lookup, {}}),
                lookup, {}, -1, false, symbolTable.symbolTableRef->id}});

        std::shared_ptr<SymbolTable::SymbolTableNode> prevSymbolTableRef =
            symbolTable.symbolTableRef;
        const std::size_t parScope = symbolTable.symbolTableRef->id;
        symbolTable.pushScope();
        sym.reset();
        sym = symstack.back();

        std::shared_ptr<record_kind>& rk =
            std::get<std::shared_ptr<record_kind>>(sym->get().kind);

        // fk->symbolTableSignature = std::string{lookup};
        //  func_kind.lutId = the scope where the function's symboltable references
        //

        rk->lutId = symbolTable.symbolTableRef->id;

        symbolTable.parentSymbolTableId = parScope;
        symbolTable.symbolTableRef->parent = prevSymbolTableRef;

        symnode = const_cast<uast::AstNode*>(ast);
    }

}}}}    // namespace chpl::ast::visitors::hpx
