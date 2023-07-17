#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(BracketLoop)
    {
        if (sym.has_value())
        {
            sym->get().kind = std::make_shared<array_kind>(
                array_kind{{symbolTable.symbolTableRef->id,
                    std::string{"array_" + emitChapelLine(ast)}, {},
                    std::make_shared<kind_node_type>(kind_node_type{
                        {std::make_shared<array_kind>(array_kind{{}})}})}});
        }
    }

}}}}    // namespace chpl::ast::visitors::hpx
