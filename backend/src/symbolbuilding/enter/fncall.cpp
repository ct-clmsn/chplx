#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(FnCall)
    {
        if (sym.has_value() &&
            std::holds_alternative<std::monostate>(sym->get().kind))
        {
            const FnCall* fc = dynamic_cast<const FnCall*>(ast);
            std::string identifier{
                dynamic_cast<const Identifier*>(fc->calledExpression())
                    ->name()
                    .c_str()};

            auto rsym = symbolTable.findPrefix(
                symbolTable.symbolTableRef->id, identifier);

            auto itr = rsym->first;
            for (; itr != rsym->second; ++itr)
            {
                auto pipeloc = itr->first.find('|');
                std::string itrstr{(pipeloc == itr->first.npos) ?
                        itr->first :
                        itr->first.substr(0, pipeloc)};

                // assign variable the return value of the function...
                //
                if (itrstr.size() >= identifier.size() &&
                    itrstr == identifier && 0 < itr->second.kind.index() &&
                    std::holds_alternative<std::shared_ptr<func_kind>>(
                        itr->second.kind))
                {
                    sym->get().kind =
                        std::get<std::shared_ptr<func_kind>>(itr->second.kind)
                            ->retKind;
                    break;
                }
                else if (std::holds_alternative<std::monostate>(
                             sym->get().kind) &&
                    std::holds_alternative<std::shared_ptr<tuple_kind>>(
                        itr->second.kind))
                {
                    assert(0 < fc->numActuals());
                    const AstNode* idx = fc->actual(0);
                    assert(idx->tag() == asttags::IntLiteral);

                    std::shared_ptr<tuple_kind>& symref =
                        std::get<std::shared_ptr<tuple_kind>>(itr->second.kind);

                    std::shared_ptr<kind_node_type>& knt =
                        std::get<std::shared_ptr<kind_node_type>>(
                            symref->retKind);

                    const auto tup_idx = int_kind::value(idx);

                    assert(tup_idx <= knt->children.size());
                    sym->get().kind = knt->children[tup_idx];

                    // assert( tup_idx <= symref->args.size() );
                    // sym->get().kind = symref->args[tup_idx].kind;

                    break;
                }
            }

            if (std::holds_alternative<nil_kind>(sym->get().kind))
            {
                sym->get().kind = expr_kind{};
            }
        }
    }

}}}}    // namespace chpl::ast::visitors::hpx
