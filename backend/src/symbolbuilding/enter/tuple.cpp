#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(Tuple)
    {
        if (sym)
        {
            Tuple const* astfn = static_cast<Tuple const*>(ast);

            if (sym->get().kind.index() == 0)
            {
                sym->get().kind = std::make_shared<tuple_kind>(
                    tuple_kind{{symbolTable.symbolTableRef->id,
                        std::string{"tuple_" + emitChapelLine(ast)}, {},
                        std::make_shared<kind_node_type>(kind_node_type{
                            {std::make_shared<tuple_kind>(tuple_kind{{}})}})}});
            }
        }
    }

}}}}    // namespace chpl::ast::visitors::hpx
