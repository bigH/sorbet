#include <memory>
#include <vector>

#include "../core/Symbols.h"
#include "ast/ast.h"

namespace sorbet::resolver {

struct ParsedSig {
    struct ArgSpec {
        core::Loc loc;
        core::NameRef name;
        core::TypePtr type;
    };
    std::vector<ArgSpec> argTypes;
    core::TypePtr returns;

    struct TypeArgSpec {
        core::Loc loc;
        core::NameRef name;
        core::TypePtr type;
    };
    std::vector<TypeArgSpec> typeArgs;

    struct {
        bool sig = false;
        bool proc = false;
        bool params = false;
        bool abstract = false;
        bool override_ = false;
        bool overridable = false;
        bool implementation = false;
        bool generated = false;
        bool returns = false;
        bool void_ = false;
        bool checked = false;
        bool final = false;
    } seen;

    TypeArgSpec &enterTypeArgByName(core::NameRef name);
    const TypeArgSpec &findTypeArgByName(core::NameRef name) const;
};

class TypeSyntax {
public:
    static bool isSig(core::MutableContext ctx, ast::Send *send);
    static ParsedSig parseSig(core::MutableContext ctx, ast::Send *send, const ParsedSig *parent, bool allowSelfType,
                              core::SymbolRef untypedBlame);
    static core::TypePtr getResultType(core::MutableContext ctx, std::unique_ptr<ast::Expression> &expr,
                                       const ParsedSig &, bool allowSelfType, core::SymbolRef untypedBlame);

    TypeSyntax() = delete;
};

} // namespace sorbet::resolver