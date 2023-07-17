#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(Function)
    {
        // pattern to repeat in the symbol table builder
        //
        struct ProgramTreeFunctionVisitor
        {
            bool enter(uast::AstNode const* ast)
            {
                const auto tag = ast->tag();
                if (tag == asttags::Function && !complete)
                {
                    lookup += std::string{
                        dynamic_cast<Function const*>(ast)->name().c_str()};
                }
                else if (tag == asttags::Identifier && !complete)
                {
                    lookup += "|" +
                        std::string{dynamic_cast<Identifier const*>(ast)
                                        ->name()
                                        .c_str()};
                }
                else if (tag == asttags::Block && !complete)
                {
                    complete = true;
                }
                return true;
            }

            void exit(uast::AstNode const* ast) {}

            bool complete;
            std::string lookup;
        };

        // if(!(sym && std::holds_alternative<std::shared_ptr<module_kind>>(sym->get().kind))) { // 0 < sym->get().kind.index())) {
        {
            ProgramTreeFunctionVisitor v{false};
            ast->traverse(v);

            Function const* astfn = static_cast<Function const*>(ast);
            const bool is_iter = astfn->kind() == Function::Kind::ITER;
            const bool is_lambda = astfn->kind() == Function::Kind::LAMBDA;

            // symbol.scopePtr = the scope where the function is defined (equivalent to a lutId)
            //
            symstack.emplace_back(
                Symbol{{std::make_shared<func_kind>(func_kind{
                            {symbolTable.symbolTableRef->id, v.lookup, {}, {}},
                            is_iter, is_lambda}),
                    v.lookup, {}, -1, false, symbolTable.symbolTableRef->id}});

            std::shared_ptr<SymbolTable::SymbolTableNode> prevSymbolTableRef =
                symbolTable.symbolTableRef;
            const std::size_t parScope = symbolTable.symbolTableRef->id;
            symbolTable.pushScope();
            sym.reset();
            sym = symstack.back();

            std::shared_ptr<func_kind>& fk =
                std::get<std::shared_ptr<func_kind>>(sym->get().kind);

            // fk->symbolTableSignature = std::string{v.lookup};
            //  func_kind.lutId = the scope where the function's symboltable references
            //

            fk->lutId = symbolTable.symbolTableRef->id;

            symbolTable.parentSymbolTableId = parScope;
            symbolTable.symbolTableRef->parent = prevSymbolTableRef;

            symnode = const_cast<uast::AstNode*>(ast);
        }
    }

}}}}    // namespace chpl::ast::visitors::hpx
