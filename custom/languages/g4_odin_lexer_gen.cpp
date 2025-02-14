/*

SDG                                                                                  JJ

Model Definition for an Odin lexer

*/

// TODO(caleb): get 4coder to allow my formatted comment file headers

#define LANG_NAME_LOWER odin
#define LANG_NAME_CAMEL Odin

#include "lexer_generator/4coder_lex_gen_main.cpp"

#define BASE_KIND(base) sm_select_base_kind(base);
#define STATE(N) State *N = sm_add_state(#N)

internal void build_language_model(void) {
    u8 utf8[129];           // no clue why I need this
    smh_utf8_fill(utf8);    // but evidently for single-byte utf-8 chars
    
    smh_set_base_character_names();  // common semantic operators
    smh_typical_tokens();            // EOF, Whitespace etc.
    
    // renaming some chars for convenience
    sm_char_name('!', "Not");
    sm_char_name('&', "And");
    sm_char_name('|', "Or");
    sm_char_name('%', "Mod");
    sm_char_name('^', "Ptr");
    sm_char_name('?', "Opt");
    sm_char_name('/', "Div");
    
    // direct token kinds
    sm_select_base_kind(TokenBaseKind_Comment); // immediate mode?
    sm_direct_token_kind("BlockComment");
    sm_direct_token_kind("LineComment");
    
    sm_select_base_kind(TokenBaseKind_LiteralInteger);
    sm_direct_token_kind("LiteralInt");
    sm_direct_token_kind("LiteralHex");
    sm_direct_token_kind("LiteralOct");
    
    sm_select_base_kind(TokenBaseKind_LiteralFloat);
    sm_direct_token_kind("LiteralFloat");
    
    sm_select_base_kind(TokenBaseKind_LiteralString);
    sm_direct_token_kind("LiteralString");
    sm_direct_token_kind("LiteralStringRaw");
    sm_direct_token_kind("LiteralChar");
    sm_direct_token_kind("OdinImport");
    
    sm_select_base_kind(TokenBaseKind_Keyword);
    sm_direct_token_kind("KeywordGeneric");
    
    // Operators
    Operator_Set *main_ops = sm_begin_op_set();
    
    BASE_KIND(TokenBaseKind_ScopeOpen) sm_op("{");
    BASE_KIND(TokenBaseKind_ScopeClose) sm_op("}");
    
    BASE_KIND(TokenBaseKind_ParentheticalOpen){
        sm_op("(");
        sm_op("[");
    }
    BASE_KIND(TokenBaseKind_ParentheticalClose){
        sm_op(")");
        sm_op("]");
    }
    
    BASE_KIND(TokenBaseKind_Operator){
        sm_op("..");
        sm_op("^");
        sm_op("::");
        sm_op(":=");
        sm_op("->");
        sm_op("&");
        
        // arithmetic operators
        
        //unary
        sm_op("+");
        sm_op("-");
        sm_op("~");
        
        //binary
        sm_op("*");
        sm_op("/");
        sm_op("%");
        sm_op("%%");
        sm_op("|");
        sm_op("&~");
        sm_op(">>");
        sm_op("<<");
        
        // comparison
        sm_op("==");
        sm_op("!=");
        sm_op("<");
        sm_op("<=");
        sm_op(">");
        sm_op(">=");
        sm_op("&&");
        sm_op("||");
        
        //logical
        sm_op("!");
        
        // assignment
        sm_op("=");
        sm_op("+=");
        sm_op("-=");
        sm_op("*=");
        sm_op("/=");
        sm_op("%=");
        sm_op("%%=");
        
        // bitwise assignment
        sm_op("|=");
        sm_op("~=");
        sm_op("&=");
        sm_op("&~");
        sm_op("<<=");
        sm_op(">>=");
        
        // conditional assignment
        sm_op("&&=");
        sm_op("||=");
        
        sm_op("..=");
        sm_op("..<");
        
        sm_op(":");
        sm_op(".");
    }
    
    BASE_KIND(TokenBaseKind_StatementClose) sm_op(",");
    
    // TODO(caleb): add stuff like #+build and @ operators here if necessary
    
    Keyword_Set *main_keys = sm_begin_key_set("main_keys"); // TODO(caleb): scopify?
    
    // TODO(caleb): organize these at some point
    BASE_KIND(TokenBaseKind_Keyword){
        sm_key("If");
        sm_key("When");
        sm_key("Else");
        sm_key("OrElse", "or_else");
        sm_key("OrReturn", "or_return");
        sm_key("OrContinue", "or_continue");
        sm_key("OrBreak", "or_break");
        sm_key("In");
        sm_key("NotIn", "not_in");
        sm_key("Break");
        sm_key("Continue");
        sm_key("Fallthrough");
        sm_key("Do");
        sm_key("Switch");
        sm_key("Case");
        sm_key("For");
        sm_key("Struct"); // QUESTION(caleb): should types be here?
        sm_key("Using");
        sm_key("Import");
        sm_key("Defer");
        sm_key("Union");
        sm_key("Proc");
        sm_key("Cast");
        sm_key("Transmute");
        sm_key("AutoCast", "auto_cast");
        sm_key("Dynamic");
        sm_key("Context");
        sm_key("Enum");
        sm_key("BitSet", "bit_set");
        sm_key("Map");
        sm_key("BitField", "bit_field");
        sm_key("Any");
        sm_key("Rawptr");
        sm_key("Typeid");
        sm_key("Static");
        sm_key("Foreign");
        sm_key("Where");
        sm_key("Return");
        
        // TODO(caleb): maybe add numerical types here
        // see https://odin-lang.org/docs/overview/#basic-types
    }
    
    BASE_KIND(TokenBaseKind_LiteralInteger) {
        sm_key("LiteralTrue", "true");
        sm_key("LiteralFalse", "false");
        sm_key("LiteralNil", "nil");
        sm_key("LiteralUndefined", "---");
    }
    
    BASE_KIND(TokenBaseKind_Identifier) sm_key_fallback("Identifier");
    
    Keyword_Set *pp_directive_set = sm_begin_key_set("pp_directives");
    BASE_KIND(TokenBaseKind_Preprocessor){
        sm_key("PP_Partial", "partial");
        sm_key("PP_Align", "align");
        sm_key("PP_Packed", "packed");
        sm_key("PP_RawUnion", "raw_union");
        sm_key("PP_SOA", "soa");
        sm_key("PP_Assert", "assert");
        sm_key("PP_SharedNil", "shared_nil");
        sm_key("PP_Config", "config");
        sm_key("PP_ForceInline", "force_inline");
        sm_key("PP_NoNil", "no_nil");
        sm_key("PP_NoAlias", "no_alias");
        sm_key("PP_AnyInt", "any_int");
        sm_key("PP_CallerLocation", "caller_location");
        sm_key("PP_CallerExpression", "caller_expression");
        sm_key("PP_ByPtr", "by_ptr");
        sm_key("PP_OptionalOK", "optional_ok");
        // TODO(caleb): the rest of these can be found at: https://odin-lang.org/docs/overview/#directives
        // I'll fill them out if I need them
    }
    
    BASE_KIND(TokenBaseKind_LexError) sm_key_fallback("PPUnknown");
    
    // Begin state machine
    State *root = sm_begin_state_machine();
    
    Flag *is_hex = sm_add_flag(FlagResetRule_AutoZero);
    Flag *is_oct = sm_add_flag(FlagResetRule_AutoZero);
    Flag *is_utf8  = sm_add_flag(FlagResetRule_AutoZero);
    Flag *is_utf16 = sm_add_flag(FlagResetRule_AutoZero);
    Flag *is_utf32 = sm_add_flag(FlagResetRule_AutoZero);
    Flag *is_char  = sm_add_flag(FlagResetRule_AutoZero);
    Flag *is_wide = sm_add_flag(FlagResetRule_AutoZero);
    
    STATE(identifier);
    STATE(whitespace);
    
    STATE(operator_or_fnumber_dot);
    STATE(operator_or_comment_slash);
    
    STATE(number);
    STATE(znumber);
    
    STATE(fnumber_decimal);
    
    STATE(number_hex_first);
    STATE(number_hex);
    STATE(number_oct);
    //STATE(number_i);
    
    STATE(pp_directive);
    STATE(pp_directive_emit);
    
    STATE(character);
    STATE(string);
    STATE(string_esc);
    STATE(string_esc_oct2);
    STATE(string_esc_oct1);
    STATE(string_esc_hex);
    STATE(string_esc_universal_8);
    STATE(string_esc_universal_7);
    STATE(string_esc_universal_6);
    STATE(string_esc_universal_5);
    STATE(string_esc_universal_4);
    STATE(string_esc_universal_3);
    STATE(string_esc_universal_2);
    STATE(string_esc_universal_1);
    
    STATE(raw_string);
    
    STATE(comment_block);
    STATE(comment_block_try_close);
    STATE(comment_block_newline);
    STATE(comment_line);
    STATE(comment_line_backslashing);
    
    
    Operator_Set *main_ops_without_dot_or_slash = smo_copy_op_set(main_ops);
    smo_remove_ops_with_prefix(main_ops_without_dot_or_slash, ".");
    smo_remove_ops_with_prefix(main_ops_without_dot_or_slash, "/");
    
    Operator_Set *main_ops_with_dot = smo_copy_op_set(main_ops);
    smo_remove_ops_without_prefix(main_ops_with_dot, ".");
    smo_ops_string_skip(main_ops_with_dot, 1);
    
    // root state
    sm_select_state(root);
    
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("EOF");
        sm_case_eof(emit);
    }
    
    sm_case("abcdefghijklmnopqrstvwxyz"
            "ABCDEFGHIJKMNOPQSTVWXYZ"
            "_$",
            identifier);
    sm_case(utf8, identifier);
    
    sm_case(".", operator_or_fnumber_dot);
    sm_case("/", operator_or_comment_slash);
    
    sm_case("123456789", number);
    sm_case("0", znumber);
    
    sm_case("\"", string); // TODO(caleb): add a flag and check for import statement
    sm_case("\'", character);
    sm_case("`", raw_string);
    
    sm_case("#", pp_directive);
    
    {
        sm_case("abcdefghijklmnopqrstvwxyz"
                "ABCDEFGHIJKMNOPQSTVWXYZ"
                "_$",
                identifier);
        sm_case(utf8, identifier);
    }
    
    // identifier state
    sm_select_state(identifier);
    sm_case("abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "_$"
            "0123456789",
            identifier);
    sm_case(utf8, identifier);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_keys(main_keys);
        sm_fallback_peek(emit);
    }
    
    // whitespace state
    sm_select_state(whitespace);
    sm_case(" \t\r\f\v\n", whitespace);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("Whitespace");
        sm_fallback_peek(emit);
    }
    
    // forwardslash state
    sm_select_state(operator_or_comment_slash);
    sm_case("*", comment_block);
    sm_case("/", comment_line);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("DivEq");
        sm_case("=", emit);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("Div");
        sm_fallback_peek(emit);
    }
    
    // dot state
    sm_select_state(operator_or_fnumber_dot);
    sm_case("0123456789", fnumber_decimal);
    {
        Character_Set *char_set = smo_new_char_set();
        smo_char_set_union_ops_firsts(char_set, main_ops_with_dot);
        char *char_set_array = smo_char_set_get_array(char_set);
        State *operator_state = smo_op_set_lexer_root(main_ops_with_dot, root, "LexError");
        sm_case_peek(char_set_array, operator_state);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("Dot");
        sm_fallback_peek(emit);
    }
    
    // number state
    sm_select_state(number);
    sm_case("0123456789", number);
    sm_case(".", fnumber_decimal);
    //sm_case("i", number_i);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralInt");
        sm_fallback_peek(emit);
    }
    
    // znumber (0) state
    sm_select_state(znumber);
    sm_case(".", fnumber_decimal);
    //sm_case("i", number_i);
    sm_case("Xx", number_hex_first);
    sm_case("01234567", number_oct);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralInt");
        sm_case("i", emit); // handle imaginaries the same way
        sm_fallback_peek(emit);
    }
    
    // float state
    sm_select_state(fnumber_decimal);
    sm_case("0123456789", fnumber_decimal);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralFloat");
        sm_case("i", emit); // handle imaginaries the same way
        sm_fallback_peek(emit);
    }
    
    // hex_first state
    sm_select_state(number_hex_first);
    sm_set_flag(is_hex, true);
    sm_case("0123456789abcdefABCDEF", number_hex);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_fallback_peek(emit);
    }
    
    // hex state
    sm_select_state(number_hex);
    sm_case("0123456789abcdefABCDEF", number_hex);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralHex");
        sm_fallback_peek(emit);
    }
    
    // oct state
    sm_select_state(number_oct);
    sm_set_flag(is_oct, true);
    sm_case("01234567", number_oct);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralOct");
        sm_fallback_peek(emit);
    }
    
    // pp_directive state
    sm_select_state(pp_directive);
    sm_case("abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "_"
            "0123456789",
            pp_directive);
    sm_fallback_peek(pp_directive_emit);
    
    // pp_directive_emit
    sm_select_state(pp_directive_emit);
    sm_delim_mark_one_past_last();
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_keys_delim(pp_directive_set);
        sm_fallback_peek(emit);
    }
    
    // character state
    sm_select_state(character);
    sm_set_flag(is_char, true);
    sm_fallback_peek(string);
    
    // string state
    sm_select_state(string);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct(is_wide, "LiteralStringWide");
        sm_emit_handler_direct(is_utf8, "LiteralStringUTF8");
        sm_emit_handler_direct(is_utf16, "LiteralStringUTF16");
        sm_emit_handler_direct(is_utf32, "LiteralStringUTF32");
        sm_emit_handler_direct("LiteralString");
        sm_case_flagged(is_char, false, "\"", emit);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct(is_wide, "LiteralCharacterWide");
        sm_emit_handler_direct(is_utf8 , "LiteralCharacterUTF8");
        sm_emit_handler_direct(is_utf16, "LiteralCharacterUTF16");
        sm_emit_handler_direct(is_utf32, "LiteralCharacterUTF32");
        sm_emit_handler_direct("LiteralChar");
        sm_case_flagged(is_char, true, "\'", emit);
    }
    sm_case("\\", string_esc);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_case_peek("\n", emit);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_case_eof_peek(emit);
    }
    sm_case_flagged(is_char, true, "\"", string);
    sm_case_flagged(is_char, false, "\'", string);
    sm_fallback(string);
    
    // string escape state
    sm_select_state(string_esc);
    sm_case("\n'\"?\\abfnrtv", string);
    sm_case("01234567", string_esc_oct2);
    sm_case("x", string_esc_hex);
    sm_case("u", string_esc_universal_4);
    sm_case("U", string_esc_universal_8);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_case_peek("\n", emit);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_case_eof_peek(emit);
    }
    sm_fallback(string);
    
    
    sm_select_state(string_esc);
    sm_case("\n'\"?\\abfnrtv", string);
    sm_case("01234567", string_esc_oct2);
    sm_case("x", string_esc_hex);
    sm_case("u", string_esc_universal_4);
    sm_case("U", string_esc_universal_8);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_case_peek("\n", emit);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LexError");
        sm_case_eof_peek(emit);
    }
    sm_fallback(string);
    
    ////
    
    sm_select_state(string_esc_oct2);
    sm_case("01234567", string_esc_oct1);
    sm_fallback_peek(string);
    
    ////
    
    sm_select_state(string_esc_oct1);
    sm_case("01234567", string);
    sm_fallback_peek(string);
    
    ////
    
    sm_select_state(string_esc_hex);
    sm_case("0123456789abcdefABCDEF", string_esc_hex);
    sm_fallback_peek(string);
    
    ////
    
    sm_select_state(string_esc_universal_8);
    sm_case("0123456789abcdefABCDEF", string_esc_universal_7);
    sm_fallback_peek(string);
    
    ////
    
    sm_select_state(string_esc_universal_7);
    sm_case("0123456789abcdefABCDEF", string_esc_universal_6);
    sm_fallback_peek(string);
    
    ////
    
    sm_select_state(string_esc_universal_6);
    sm_case("0123456789abcdefABCDEF", string_esc_universal_5);
    sm_fallback_peek(string);
    
    ////
    
    sm_select_state(string_esc_universal_5);
    sm_case("0123456789abcdefABCDEF", string_esc_universal_4);
    sm_fallback_peek(string);
    
    ////
    
    sm_select_state(string_esc_universal_4);
    sm_case("0123456789abcdefABCDEF", string_esc_universal_3);
    sm_fallback_peek(string);
    
    ////
    
    sm_select_state(string_esc_universal_3);
    sm_case("0123456789abcdefABCDEF", string_esc_universal_2);
    sm_fallback_peek(string);
    
    ////
    
    sm_select_state(string_esc_universal_2);
    sm_case("0123456789abcdefABCDEF", string_esc_universal_1);
    sm_fallback_peek(string);
    
    ////
    
    sm_select_state(string_esc_universal_1);
    sm_case("0123456789abcdefABCDEF", string);
    sm_fallback_peek(string);
    
    // raw_string state
    sm_select_state(raw_string);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LiteralStringRaw");
        sm_case("`", emit);
    }
    
    ////
    
    sm_select_state(comment_block);
    sm_case("*", comment_block_try_close);
    sm_case("\n", comment_block_newline);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("BlockComment");
        sm_case_eof_peek(emit);
    }
    sm_fallback(comment_block);
    
    ////
    
    sm_select_state(comment_block_try_close);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("BlockComment");
        sm_case("/", emit);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("BlockComment");
        sm_case_eof_peek(emit);
    }
    sm_case("*", comment_block_try_close);
    sm_fallback(comment_block);
    
    ////
    
    sm_select_state(comment_block_newline);
    sm_fallback_peek(comment_block);
    
    ////
    
    sm_select_state(comment_line);
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LineComment");
        sm_case_peek("\n", emit);
    }
    {
        Emit_Rule *emit = sm_emit_rule();
        sm_emit_handler_direct("LineComment");
        sm_case_eof_peek(emit);
    }
    sm_case("\\", comment_line_backslashing);
    sm_fallback(comment_line);
    
    sm_select_state(comment_line_backslashing);
    sm_case("\r", comment_line_backslashing);
    sm_fallback(comment_line);
}