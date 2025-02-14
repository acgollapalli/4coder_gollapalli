/*

SDG                                                                         JJ

Lexer for 4coder in odin

*/

import tk "core:odin.tokenizer"

C4_String_Const :: []u8

C4_Base_Allocator_Reserve_Signature :: proc "c" (user_data: rawptr, size: u64, size_out: ^u64, location: C4_String_Const)
C4_Base_Allocator_Commit_Signature :: proc "c" (user_data: rawptr, ptr: rawptr, size: u64)
C4_Base_Allocator_Free_Signature :: proc "c" (user_data: rawptr, ptr: rawptr)
C4_Base_Allocator_Set_Access_Signature :: proc "c" (user_data: rawptr, ptr: rawptr, size: u64, flags: u32)

C4_Base_Allocator :: struct {
	reserve: ^C4_Base_Allocator_Reserve_Signature,
	commit: ^C4_Base_Allocator_Commit_Signature,
	uncommit: ^C4_Base_Allocator_Uncommit_Signature,
	free: ^C4_Base_Allocator_Free_Signature,
	set_access: ^C4_Base_Allocator_Set_Access_Signature,
}

C4_Cursor :: Struct {
	base: ^u8,
	pos: u64,
	cap: u64,
}

C4_Cursor_Node {
	#raw_union {
		next: ^C4_Cursor_Node,
		prev: ^C4_Cursor_Node,
	}
	cursor: C4_Cursor,
}

// TODO(caleb): Turn this into an odin allocator 
C4_Arena :: struct {
	base_allocator: ^C4_Base_Allocator,
	cursor_node: ^Cursor_Node,
	chunk_size: u64,
	alignment: u64,
}

C4_Token_Base_Kind :: enum i16 {
	// TokenBaseKind_EOF = 0,
    // TokenBaseKind_Whitespace = 1,
    // TokenBaseKind_LexError = 2,
    // TokenBaseKind_Comment = 3,
    // TokenBaseKind_Keyword = 4,
    // TokenBaseKind_Preprocessor = 5,
    // TokenBaseKind_Identifier = 6,
    // TokenBaseKind_Operator = 7,
    // TokenBaseKind_LiteralInteger = 8,
    // TokenBaseKind_LiteralFloat = 9,
    // TokenBaseKind_LiteralString = 10,
    // TokenBaseKind_ScopeOpen = 11,
    // TokenBaseKind_ScopeClose = 12,
    // TokenBaseKind_ParentheticalOpen = 13,
    // TokenBaseKind_ParentheticalClose = 14,
    // TokenBaseKind_StatementClose = 15,
    
    // TokenBaseKind_COUNT = 16,

    EOF = 0,
    Whitespace = 1,
    LexError = 2,
    Comment = 3,
    Keyword = 4,
    Preprocessor = 5,
    Identifier = 6,
    Operator = 7,
    LiteralInteger = 8,
    LiteralFloat = 9,
    LiteralString = 10,
    ScopeOpen = 11,
    ScopeClose = 12,
    ParentheticalOpen = 13,
    ParentheticalClose = 14,
    StatementClose = 15,
    
    COUNT = 16,
}

C4_Token :: struct {
	pos: i64,
	size: i64,
	kind: C4_Token_Base_Kind, 
	flags: u16,
	sub_kind: i16,
	sub_flags: u16,
}

C4_Token_Block :: struct {
	next: ^C4_Token_Block,
	prev: ^C4_Token_Block,
	tokens: [^]C4_Token,
	count: i64,
	max: i64,
}

C4_Token_List -> {
	first: ^C4_Token_Block,
	last: ^C4_Token_Block,
	node_count: i64,
	total_count: i64,
}


foreign c4 {
	token_list_push :: proc "c" (arena: ^C4_Arena, list: C4_Token_List, token: C4_Token) ---
}

token_list_push :: proc "contextless" (arena: ^C4_Arena, list: ^C4_Token_List, token: C4_Token) {
	block := list.last
	if (block == nil || block.count + 1 > block.max) {
		block = transmute(^C4_Token_List)c4.wrap_uninitialized(c4.push(arena, size_of(C4_Token_Block), nil))
		block.next = 0
		block.prev = 0
		new_max := 4096
		block.tokens = 
}

@(export, link_prefix="g4_"){
	lex_full_input_odin :: proc "c" (arena: ^C4_Arena, input: C4_String_Const) -> C4_Token_List {
		// what does this do...
		tokenizer : tk.Tokenizer
		tk.init(&tokenizer, transmute(string)input, "4coder_input") // TODO(caleb): add error handler
		
		list : C4_Token_List
	
		// TODO(caleb): This will remove any actual EOF's from the input
		for token := tk.scan(tokenizer) token.kind != .EOF {
	
			c4_token := C4_Token {
				pos = token.position,
				size = len(token.text),
				flags = 0,
				// TODO(caleb): sub kind and sub flags here if I ever need them
			}	
	
			// TODO(caleb): we need to handle whitespace as well
			#partial switch token.kind {
			case .Invalid:								c4_token.kind = .LexError
			case .EOF: 	   							c4_token.kind = .EOF
			case .Comment:								c4_token.kind = .Comment
			case .File_Tag:   							c4_token.kind = .Preprocessor
			case .Ident: 	 							c4_token.kind = .Identifier
			case .Integer:								c4_token.kind = .LiteralInteger
			case .Float:	  							c4_token.kind = .LiteralFloat
			case .Imag: 	  							c4_token.kind = .LiteralInteger
			case .Rune: 	  							c4_token.kind = .LiteralString
			case .String: 								c4_token.kind = .LiteralString
			case .B_Operator_Begin..<.B_Comparison_End:   c4_token.kind = .Operator
			case .Open_Paren:							 c4_token.kind = .ParentheticalOpen
			case .Close_Paren:							c4_token.kind = .ParentheticalClose
			case .Open_Bracket:						   c4_token.kind = .ParentheticalOpen
			case .Close_Bracket:						  c4_token.kind = .ParentheticalClose
			case .Open_Brace:							 c4_token.kind = .ScopeOpen
			case .Close_Brace:							c4_token.kind = .ScopeClose
			case .Colon..<.B_Operator_End:				c4_token.kind = .Operator
			case .B_Keyword_Begin..<.B_Keyword_End:	   c4_token.kind = .Keyword
			}
		
			c4.token_list_push(arena, &list, &token)
		}
		
		return list
	}
}