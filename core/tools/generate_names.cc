#include "common/common.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "absl/strings/escaping.h"

using namespace std;

struct NameDef {
    int id;
    string srcName;
    string val;

    NameDef(string_view srcName, string_view val) : srcName(string(srcName)), val(string(val)) {
        if (srcName == val) {
            sorbet::Exception::raise("Only pass one arg for '", val, "'");
        }
    }
    NameDef(string_view srcName) : srcName(string(srcName)), val(string(srcName)) {}
};

NameDef names[] = {
    {"initialize"},
    {"andAnd", "&&"},
    {"orOr", "||"},
    {"to_s"},
    {"to_a"},
    {"to_h"},
    {"to_hash"},
    {"to_proc"},
    {"concat"},
    {"key_p", "key?"},
    {"intern"},
    {"call"},
    {"bang", "!"},
    {"squareBrackets", "[]"},
    {"squareBracketsEq", "[]="},
    {"unaryPlus", "+@"},
    {"unaryMinus", "-@"},
    {"star", "*"},
    {"starStar", "**"},
    {"ampersand", "&"},
    {"tripleEq", "==="},
    {"orOp", "|"},
    {"backtick", "`"},
    {"slice"},
    {"defined_p", "defined?"},
    {"each"},

    // used in CFG for temporaries
    {"whileTemp", "<whileTemp>"},
    {"ifTemp", "<ifTemp>"},
    {"returnTemp", "<returnTemp>"},
    {"statTemp", "<statTemp>"},
    {"assignTemp", "<assignTemp>"},
    {"returnMethodTemp", "<returnMethodTemp>"},
    {"debugEnvironmentTemp", "<debugEnvironmentTemp>"},
    {"blockReturnTemp", "<blockReturnTemp>"},
    {"nextTemp", "<nextTemp>"},
    {"selfMethodTemp", "<selfMethodTemp>"},
    {"hashTemp", "<hashTemp>"},
    {"arrayTemp", "<arrayTemp>"},
    {"rescueTemp", "<rescueTemp>"},
    {"rescueStartTemp", "<rescueStartTemp>"},
    {"rescueEndTemp", "<rescueEndTemp>"},
    {"gotoDeadTemp", "<gotoDeadTemp>"},
    {"exceptionClassTemp", "<exceptionClassTemp>"},
    {"isaCheckTemp", "<isaCheckTemp>"},
    {"throwAwayTemp", "<throwAwayTemp>"},
    {"castTemp", "<castTemp>"},
    {"finalReturn", "<finalReturn>"},
    {"cfgAlias", "<cfgAlias>"},
    {"magic", "<magic>"},
    // end CFG temporaries

    {"include"},
    {"extend"},
    {"currentFile", "__FILE__"},
    {"merge"},

    // T keywords
    {"sig"},
    {"typeParameters", "type_parameters"},
    {"typeParameter", "type_parameter"},
    {"abstract"},
    {"implementation"},
    {"override_", "override"},
    {"overridable"},

    // Sig builders
    {"params"},
    {"final"},
    {"returns"},
    {"void_", "void"},
    {"checked"},
    {"soft"},
    {"generated"},

    {"all"},
    {"any"},
    {"enum_", "enum"},
    {"nilable"},
    {"proc"},
    {"untyped"},
    {"Array"},
    {"Hash"},
    {"noreturn"},
    {"singletonClass", "singleton_class"},
    {"class_", "class"},
    {"classOf", "class_of"},
    {"selfType", "self_type"},
    {"coerce"},

    {"assertType", "assert_type!"},
    {"cast"},
    {"let"},
    {"unsafe"},
    {"must"},
    {"declareInterface", "interface!"},
    {"declareAbstract", "abstract!"},
    {"revealType", "reveal_type"},
    // end T keywords

    // Ruby DSL methods which we understand
    {"attr"},
    {"attrAccessor", "attr_accessor"},
    {"attrWriter", "attr_writer"},
    {"attrReader", "attr_reader"},
    {"private_", "private"},
    {"protected_", "protected"},
    {"public_", "public"},
    {"privateClassMethod", "private_class_method"},
    {"moduleFunction", "module_function"},
    {"aliasMethod", "alias_method"},
    {"typeAlias", "type_alias"},
    {"typeMember", "type_member"},
    {"typeTemplate", "type_template"},
    {"T"},
    {"covariant", "out"},
    {"contravariant", "in"},
    {"invariant", "<invariant>"},
    {"fixed"},

    {"prop"},
    {"token_prop"},
    {"timestamped_token_prop"},
    {"created_prop"},
    {"merchant_prop"},
    {"encrypted_prop"},
    {"array"},
    {"type"},
    {"optional"},
    {"immutable"},
    {"migrate"},
    {"default_", "default"},
    {"const_", "const"},
    {"token"},
    {"created"},
    {"merchant"},
    {"foreign"},
    {"Chalk"},
    {"ODM"},
    {"Mutator"},
    {"Private"},
    {"HashMutator"},
    {"ArrayMutator"},
    {"DocumentMutator"},

    {"describe"},
    {"it"},
    {"before"},

    {"dslOptional", "dsl_optional"},
    {"dslRequired", "dsl_required"},
    {"implied"},
    {"skipGetter", "skip_getter"},

    {"wrapInstance", "wrap_instance"},

    {"registered"},
    {"instanceRegistered", "<instance_registered>"},
    {"helpers"},

    {"Opus"},
    {"DB"},
    {"Model"},
    {"Mixins"},
    {"Encryptable"},
    {"EncryptedValue"},
    {"Command"},
    // end DSL methods

    // Our own special methods which have special meaning
    {"hardAssert", "hard_assert"}, // Kernel.hard_assert
    // end special methods

    // The next two names are used as keys in SymbolInfo::members to store
    // pointers up and down the singleton-class hierarchy. If A's singleton
    // class is B, then A will have a `singletonClass` entry in its members
    // table which references B, and B will have an `attachedClass` entry
    // pointing at A.
    //
    // The "attached class" terminology is borrowed from MRI, which refers
    // to the unique instance attached to a singleton class as the "attached
    // object"
    {"singleton", "<singleton class>"},
    {"attached", "<attached class>"},

    // This name is used as a key in SymbolInfo::members to store the module
    // registered via the `mixes_in_class_method` name.
    {"classMethods", "<class methods>"},
    {"mixesInClassMethods", "mixes_in_class_methods"},

    {"blockTemp", "<block>"},
    {"blockRetrunType", "<block-return-type>"},
    {"blockPreCallTemp", "<block-pre-call-temp>"},
    {"blockPassTemp", "<block-pass>"},
    {"forTemp"},
    {"new_", "new"},
    {"blockCall", "<block-call>"},
    {"blkArg", "<blk>"},
    {"blockGiven_p", "block_given?"},

    // Used to generate temporary names for destructuring arguments ala proc do
    //  |(x,y)|; end
    {"destructureArg", "<destructure>"},

    {"lambda"},
    {"nil_p", "nil?"},
    {"present_p", "present?"},
    {"nil"},
    {"NilClass"},
    {"super"},
    {"empty", ""},

    {"buildHash", "<build-hash>"},
    {"buildArray", "<build-array>"},
    {"splat", "<splat>"},
    {"expandSplat", "<expand-splat>"},
    {"arg0"},
    {"arg1"},
    {"arg2"},
    {"opts"},
    {"Struct"},
    {"keepForIde", "keep_for_ide"},
    {"keepForTypechecking", "keep_for_typechecking"},

    {"is_a_p", "is_a?"},
    {"kind_of", "kind_of?"},
    {"lessThan", "<"},
    {"eqeq", "=="},
    {"neq", "!="},

    // methods that are known by tuple and\or shape types
    {"freeze"},
    {"last"},
    {"first"},
    {"min"},
    {"max"},

    // Enumerable#flat_map has special-case logic in Infer
    {"flatMap", "flat_map"},

    // Array#flatten and #compact are also custom-implemented
    {"flatten"},
    {"compact"},

    {"staticInit", "<static-init>"},

    {"require"},
    {"callWithSplat", "<call-with-splat>"},
    {"Enumerable_to_h"},
};

void emit_name_header(ostream &out, NameDef &name) {
    out << "#ifndef NAME_" << name.srcName << '\n';
    out << "#define NAME_" << name.srcName << '\n';
    out << "    // \"" << name.val << "\"" << '\n';
    out << "    static inline constexpr NameRef " << name.srcName << "() {" << '\n';
    out << "        return NameRef(NameRef::WellKnown{}, " << name.id << ");" << '\n';
    out << "    }" << '\n';
    out << "#endif" << '\n';
    out << '\n';
}

void emit_name_string(ostream &out, NameDef &name) {
    out << "const char *" << name.srcName << " = \"";
    out << absl::CEscape(name.val) << "\";" << '\n';

    out << "std::string_view " << name.srcName << "_DESC{(char*)";
    out << name.srcName << "," << name.val.size() << "};" << '\n';
    out << '\n';
}

void emit_register(ostream &out) {
    out << "void Names::registerNames(GlobalState &gs) {" << '\n';
    for (auto &name : names) {
        out << "    NameRef " << name.srcName << "_id = gs.enterNameUTF8(" << name.srcName << "_DESC);" << '\n';
    }
    out << '\n';
    for (auto &name : names) {
        out << "    ENFORCE(" << name.srcName << "_id._id == " << name.id << "); /* Names::" << name.srcName << "() */"
            << '\n';
    }
    out << '\n';
    out << "}" << '\n';
}

int main(int argc, char **argv) {
    int i = 1;
    for (auto &name : names) {
        name.id = i++;
    }
    int lastId = i - 1;

    // emit header file
    {
        ofstream header(argv[1], ios::trunc);
        if (!header.good()) {
            cerr << "unable to open " << argv[1] << '\n';
            return 1;
        }
        header << "#include \"core/NameRef.h\"" << '\n' << '\n';
        header << "namespace sorbet {" << '\n';
        header << "namespace core {" << '\n';
        header << "class GlobalState;" << '\n';
        header << "namespace Names {" << '\n';

        for (auto &name : names) {
            emit_name_header(header, name);
        }

        header << "#ifndef NAME_LAST_WELL_KNOWN_NAME" << '\n';
        header << "#define NAME_LAST_WELL_KNOWN_NAME" << '\n';
        header << "constexpr int LAST_WELL_KNOWN_NAME = " << lastId << ";" << '\n';
        header << "#endif" << '\n';

        header << "    void registerNames(GlobalState &gs);" << '\n';
        header << "}" << '\n';
        header << "}" << '\n';
        header << "}" << '\n';
    }

    // emit initialization .cc file
    {
        ofstream classfile(argv[2], ios::trunc);
        if (!classfile.good()) {
            cerr << "unable to open " << argv[2] << '\n';
            return 1;
        }
        classfile << "#include \"core/GlobalState.h\"" << '\n' << '\n';
        classfile << "#include \"core/Names.h\"" << '\n' << '\n';
        classfile << "#include \"core/Names_gen.h\"" << '\n' << '\n';
        classfile << "namespace sorbet {" << '\n';
        classfile << "namespace core {" << '\n';
        classfile << "namespace {" << '\n';
        for (auto &name : names) {
            emit_name_string(classfile, name);
        }
        classfile << "}" << '\n';
        classfile << '\n';

        emit_register(classfile);

        classfile << "}" << '\n';
        classfile << "}" << '\n';
    }

    return 0;
}