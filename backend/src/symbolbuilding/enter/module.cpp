#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(Module)
    {
        std::string lookup = static_cast<Module const*>(ast)->name().str();
        symbolTable.parentModuleId = symbolTable.modules.size();
        // symbol.scopePtr = the scope where the function is defined (equivalent to a lutId)
        //
        symstack.emplace_back(
            Symbol{{std::make_shared<module_kind>(module_kind{
                        {symbolTable.symbolTableRef->id, lookup, {}, {}}}),
                lookup, {}, -1, false, symbolTable.symbolTableRef->id}});

        std::shared_ptr<SymbolTable::SymbolTableNode> prevSymbolTableRef =
            symbolTable.symbolTableRef;
        const std::size_t parScope = symbolTable.symbolTableRef->id;
        symbolTable.pushScope();
        sym.reset();
        sym = symstack.back();

        std::shared_ptr<module_kind>& mk =
            std::get<std::shared_ptr<module_kind>>(sym->get().kind);

        // fk->symbolTableSignature = std::string{v.lookup};
        //  func_kind.lutId = the scope where the function's symboltable references
        //
        mk->lutId = symbolTable.symbolTableRef->id;

        symbolTable.parentSymbolTableId = parScope;
        symbolTable.symbolTableRef->parent = prevSymbolTableRef;

        symnode = const_cast<uast::AstNode*>(ast);
    }

}}}}    // namespace chpl::ast::visitors::hpx
