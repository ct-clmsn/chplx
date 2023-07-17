#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(Variable)
    {
        const Variable::Kind k = dynamic_cast<Variable const*>(ast)->kind();
        int kindqual = -1;

        if (k == Variable::Kind::VAR)
        {
            kindqual = 0;
        }
        else if (k == Variable::Kind::CONST)
        {
            kindqual = 1;
        }
        else if (k == Variable::Kind::CONST_REF)
        {
            kindqual = 2;
        }
        else if (k == Variable::Kind::REF)
        {
            kindqual = 3;
        }
        else if (k == Variable::Kind::PARAM)
        {
            kindqual = 4;
        }
        else if (k == Variable::Kind::TYPE)
        {
            kindqual = 5;
        }
        else if (k == Variable::Kind::INDEX)
        {
            kindqual = 10;
        }

        const bool cfg = dynamic_cast<Variable const*>(ast)->isConfig();

        symstack.emplace_back(Symbol{{{},
            std::string{dynamic_cast<NamedDecl const*>(ast)->name().c_str()},
            {}, kindqual, cfg, symbolTable.symbolTableRef->id}});

        sym.reset();
        sym = symstack.back();
        symnode = const_cast<uast::AstNode*>(ast);
    }

}}}}    // namespace chpl::ast::visitors::hpx
