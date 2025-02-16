/* 

SDG                                                                                  JJ

Odin Lexing and Parsing 
for the 4coder Custom Layer

*/

#ifndef G4_ODIN_H
#define G4_ODIN_H

// literally just odins tokenizer
struct Lex_State_Odin {
    String_Const_u8 path;
    String_Const_u8 src;
    void *Error_Handler; // TODO(caleb): Change this to the actual function signature
    u32 flags;
    i32 rune;
    i64 offset;
    i64 read_offset;
    i64 line_offset;
    b8 insert_semicolon;
    i64 error_count;
};

void g4_lex_full_input_odin_init(Lex_State_Odin *state_ptr, String_Const_u8 input);
Token_List g4_lex_full_input_odin(Arena *arena, String_Const_u8 input);
b32 g4_lex_full_input_odin_breaks(Arena *arena, Token_List *list, Lex_State_Odin *state_ptr, u64 max);

#endif //G4_ODIN_H
