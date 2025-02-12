/*

SDG                                                                                     JJ

*/

#include <assert.h>

enum g4_EditorMode {
  g4_NavigateMode,
  g4_InsertMode,
  g4_BlockMode, // TODO(caleb): Implement this
  g4_DoMany,
};

static g4_EditorMode g4_CurrentMode = g4_NavigateMode;
static int g4_ThisManyTimes = 0;

// TODO(caleb): The RIGHT thing to do is to make use
// of the bindings api
CUSTOM_COMMAND_SIG(g4_write_text_input)
CUSTOM_DOC("Inserts text from a keycode event")
{
  User_Input in = get_current_input(app);		
  assert(in.event.kind == InputEventKind_KeyStroke); 
  assert(in.event.key.first_dependent_text->kind == InputEventKind_TextInsert); 
  String_Const_u8 ins_st= in.event.key.first_dependent_text->text.string;		
  write_text(app, ins_st);
}

// NOTE(caleb): if you have stuff executing twice
// then come back here to look at this macro
#define N_MODAL_INSERT_BLOCK(block, insert)	{		\
	switch (g4_CurrentMode) {					    \
	case g4_NavigateMode:						    \
	  block										    \
	  break;									    \
	case g4_InsertMode:							    \
	  insert(app);					                \
	  break;									    \
	case g4_DoMany:								    \
	  g4_CurrentMode = g4_NavigateMode;				\
	  while(g4_ThisManyTimes-- > 0)					\
      block											\
	  break;										\
	}												\
  }

#define N_MODAL_INSERT_COMMAND(command, insert) N_MODAL_INSERT_BLOCK({command(app);}, insert)
#define N_MODAL_BLOCK(block) N_MODAL_INSERT_BLOCK(block, g4_write_text_input)
#define N_MODAL_COMMAND(command) N_MODAL_BLOCK({command(app);})

// NOTE(caleb): if you have stuff executing twice
// then come back here to look at this macro
#define MODAL_INSERT_BLOCK(block, insert)	{		\
	switch (g4_CurrentMode) {					    \
	case g4_NavigateMode:						    \
	  block										    \
	  break;									    \
	case g4_InsertMode:							    \
	  insert(app);					                \
	  break;									    \
	case g4_DoMany:								    \
	  block											\
	  break;										\
	}												\
  }
#define MODAL_COMMAND(command) MODAL_INSERT_BLOCK({command(app);}, g4_write_text_input)
#define MODAL_INSERT(command) MODAL_INSERT_BLOCK({ \
	  command(app);										\
	  g4_CurrentMode = g4_InsertMode;					\
	}, g4_write_text_input)

/*
  Preliminaries
*/

CUSTOM_COMMAND_SIG(g4_return_to_nav_mode)
CUSTOM_DOC("Exits mode or command. Returns to Navigation Mode")
{
	g4_CurrentMode = g4_NavigateMode;
	g4_ThisManyTimes = 0;
  }

CUSTOM_COMMAND_SIG(g4_enter_insert_mode)
CUSTOM_DOC("In Navigate Mode, enters insert mode.")
N_MODAL_BLOCK({
  g4_CurrentMode = g4_InsertMode;
  g4_ThisManyTimes = 0;
})

CUSTOM_COMMAND_SIG(g4_n_keys)
CUSTOM_DOC("In Do-Many mode, sets how many times an action is executed, else inserts text.")
{
  switch (g4_CurrentMode) {
  case g4_NavigateMode:
	project_fkey_command(app);
	break;
  case g4_InsertMode:							    
	g4_write_text_input(app);					                
	break;
  case g4_DoMany:
	User_Input in = get_current_input(app);
	String_Const_u8 ins_st= in.event.key.first_dependent_text->text.string;;
	g4_ThisManyTimes *= 10;
	g4_ThisManyTimes += (u8)(*ins_st.str) - '0';
	break;
  }
}

CUSTOM_COMMAND_SIG(g4_modal_null)
CUSTOM_DOC("In navigation mode, don't do anything")
MODAL_INSERT_BLOCK({}, g4_write_text_input)

/*
  Navigation Commands
*/

// W
CUSTOM_COMMAND_SIG(g4_move_up)
CUSTOM_DOC("In Navigate Mode, moves the cursor up one line, else inserts text")
N_MODAL_COMMAND(move_up)

// A
CUSTOM_COMMAND_SIG(g4_move_left)
CUSTOM_DOC("In Navigate Mode, moves the cursor one character left, else inserts text")
N_MODAL_COMMAND(move_left)

// S
CUSTOM_COMMAND_SIG(g4_move_down)
CUSTOM_DOC("In Navigate Mode, moves the cursor down one line, else inserts text")
N_MODAL_COMMAND(move_down)

// D
CUSTOM_COMMAND_SIG(g4_move_right)
CUSTOM_DOC("In Navigate Mode, moves the cursor one character right, else inserts text")
N_MODAL_COMMAND(move_right)

// SHIFT-W
CUSTOM_COMMAND_SIG(g4_page_up)
CUSTOM_DOC("In Navigate Mode, moves the cursor up one page, else inserts text")
N_MODAL_COMMAND(page_up)

// SHIFT-A
CUSTOM_COMMAND_SIG(g4_move_left_whitespace_boundary)
CUSTOM_DOC("In Navigate Mode, moves left to the next whitespace boundary, else inserts text")
N_MODAL_COMMAND(move_left_whitespace_boundary)

// SHIFT-S
CUSTOM_COMMAND_SIG(g4_page_down)
CUSTOM_DOC("In Navigate Mode, moves the cursor down one page, else inserts text")
N_MODAL_COMMAND(page_down)

// SHIFT-D
CUSTOM_COMMAND_SIG(g4_move_right_whitespace_boundary)
CUSTOM_DOC("In Navigate Mode, moves right to the next whitespace boundary, else inserts text")
N_MODAL_COMMAND(move_right_whitespace_boundary)

// CTRL-W
CUSTOM_COMMAND_SIG(g4_move_up_to_blank_line_end)
CUSTOM_DOC("In Navigate Mode, seeks the cursor up to the next blank line and places it at the end of the line, else inserts text")
N_MODAL_COMMAND(move_up_to_blank_line_end)

// CTRL-A
CUSTOM_COMMAND_SIG(g4_move_left_alpha_numeric_boundary)
CUSTOM_DOC("In Navigate Mode, seek left for boundary between alphanumeric characters and non-alphanumeric characters, else inserts text")
N_MODAL_COMMAND(move_left_alpha_numeric_boundary)

// CTRL-S
CUSTOM_COMMAND_SIG(g4_move_down_to_blank_line_end)
CUSTOM_DOC("In Navigate Mode, seeks the cursor down to the next blank line and places it at the end of the line, else inserts text")
N_MODAL_COMMAND(move_down_to_blank_line_end)

// CTRL-D
CUSTOM_COMMAND_SIG(g4_move_right_alpha_numeric_boundary)
CUSTOM_DOC("In Navigate Mode, seek right for boundary between alphanumeric characters and non-alphanumeric characters, else inserts text")
N_MODAL_COMMAND(move_right_alpha_numeric_boundary)

// ALT-W
CUSTOM_COMMAND_SIG(g4_move_line_up)
CUSTOM_DOC("In Navigate Mode, swaps the line under the cursor with the line above it, and moves the cursor up with it, else inserts text")
N_MODAL_COMMAND(move_line_up)

// ALT-A
CUSTOM_COMMAND_SIG(g4_move_left_alpha_numeric_or_camel_boundary)
CUSTOM_DOC("In Navigate Mode, seek right for boundary between alphanumeric characters or camel case word and non-alphanumeric characters, else inserts text")
N_MODAL_COMMAND(move_left_alpha_numeric_or_camel_boundary)

// Alt-S
CUSTOM_COMMAND_SIG(g4_move_line_down)
CUSTOM_DOC("In Navigate Mode, swaps the line under the cursor with the line below it, and moves the cursor down with it, else inserts text")
N_MODAL_COMMAND(move_line_down)

// ALT-D
CUSTOM_COMMAND_SIG(g4_move_right_alpha_numeric_or_camel_boundary)
CUSTOM_DOC("In Navigate Mode, seek right for boundary between alphanumeric characters or camel case word and non-alphanumeric characters, else inserts text")
N_MODAL_COMMAND(move_right_alpha_numeric_or_camel_boundary)

// SPACE
CUSTOM_COMMAND_SIG(g4_set_mark)
CUSTOM_DOC("In Navigate Mode, sets the mark to the current position of the cursor")
N_MODAL_COMMAND(set_mark)

// SHIFT-SPACE
CUSTOM_COMMAND_SIG(g4_cursor_mark_swap_or_insert_underscore)
CUSTOM_DOC("In Navigate Mode, swaps the position of the cursor and mark. In Insert mode, inserts an underscore. In do-many mode, swaps the cursor and mark repeatedly.")
N_MODAL_INSERT_COMMAND(cursor_mark_swap, write_underscore)

CUSTOM_COMMAND_SIG(g4_interactive_open)
CUSTOM_DOC("In Navigate Mode, open file")
MODAL_COMMAND(interactive_open)

CUSTOM_COMMAND_SIG(g4_interactive_new)
CUSTOM_DOC("In Navigate Mode, open a new file")
MODAL_COMMAND(interactive_new)

CUSTOM_COMMAND_SIG(g4_open_in_other)
CUSTOM_DOC("In Navigate Mode, open file in other panel")
N_MODAL_COMMAND(open_in_other)

CUSTOM_COMMAND_SIG(g4_goto_next_jump)
CUSTOM_DOC("In Navigate Mode, go to the next entry in the jump list")
N_MODAL_COMMAND(goto_next_jump)

CUSTOM_COMMAND_SIG(g4_goto_prev_jump)
CUSTOM_DOC("In Navigate Mode, go to the previous entry in the jump list")
N_MODAL_COMMAND(goto_prev_jump)

CUSTOM_COMMAND_SIG(g4_change_active_panel)
CUSTOM_DOC("In Navigate Mode, change_active_panel")
N_MODAL_COMMAND(change_active_panel)

CUSTOM_COMMAND_SIG(g4_change_active_panel_backwards)
CUSTOM_DOC("In Navigate Mode, change_active_panel_backwards")
N_MODAL_COMMAND(change_active_panel_backwards)

CUSTOM_COMMAND_SIG(g4_open_panel_vsplit)
CUSTOM_DOC("In Navigate Mode, open_panel_vsplit")
MODAL_COMMAND(open_panel_vsplit)

CUSTOM_COMMAND_SIG(g4_close_panel)
CUSTOM_DOC("In Navigate Mode, close_panel")
MODAL_COMMAND(close_panel)



/*
  Editing Commands
*/

CUSTOM_COMMAND_SIG(g4_delete_char)
CUSTOM_DOC("Delete char in insert mode, otherwise do nothing")
MODAL_INSERT_BLOCK({}, delete_char)

CUSTOM_COMMAND_SIG(g4_backspace_char)
CUSTOM_DOC("Backspace char in insert mode, otherwise do nothing")
MODAL_INSERT_BLOCK({}, backspace_char)

CUSTOM_COMMAND_SIG(g4_kill_range)
CUSTOM_DOC("In Navigate Mode, kills the range between mark and cursor, then enters insert mode")
MODAL_COMMAND(delete_range)

CUSTOM_COMMAND_SIG(g4_kill_range_and_insert)
CUSTOM_DOC("In Navigate Mode, kills the range between mark and cursor, then enters insert mode")
MODAL_INSERT(delete_range)

CUSTOM_COMMAND_SIG(g4_paste)
CUSTOM_DOC("In Navigate Mode, kills the range between mark and cursor")
N_MODAL_BLOCK({
	paste(app);
  })

CUSTOM_COMMAND_SIG(g4_kill_range_and_paste)
CUSTOM_DOC("In Navigate Mode, kills the range between mark and cursor")
N_MODAL_BLOCK({
	delete_range(app);
	paste(app);
  })

CUSTOM_COMMAND_SIG(g4_paste_next)
CUSTOM_DOC("In Navigate Mode, replaces the previous paste with the next item from the clipboard")
N_MODAL_BLOCK({
	delete_range(app);
	paste_next(app);
  })

CUSTOM_COMMAND_SIG(g4_keyboard_macro_replay)
CUSTOM_DOC("In Navigate Mode, replays the macro. In Do-Many mode does the macro n-many times, else inserts text")
N_MODAL_COMMAND(keyboard_macro_replay)

// TODO(caleb): make this a toggle by altering the macro recording code
CUSTOM_COMMAND_SIG(g4_keyboard_macro_start_recording)
CUSTOM_DOC("In Navigate Mode, record the macro.")
MODAL_COMMAND(keyboard_macro_start_recording)

CUSTOM_COMMAND_SIG(g4_keyboard_macro_finish_recording)
CUSTOM_DOC("In Navigate Mode, finish recording the macro.")
MODAL_COMMAND(keyboard_macro_finish_recording)


/*
  Build commands
*/

CUSTOM_COMMAND_SIG(g4_build_in_build_panel)
CUSTOM_DOC("In Navigate Mode, build in build panel")
MODAL_COMMAND(build_in_build_panel)

/*
  Lister Funtions
*/
CUSTOM_COMMAND_SIG(g4_command_lister)
CUSTOM_DOC("In Navigate Mode, Opens command Lister (like M-x in emacs)")
MODAL_COMMAND(command_lister)

CUSTOM_COMMAND_SIG(g4_project_command_lister)
CUSTOM_DOC("In Navigate Mode, opens project command lister")
MODAL_COMMAND(project_command_lister)

/*
  Project Commands
*/

/*
  Search Commands
*/
// TODO(caleb): Implement a find in project

CUSTOM_COMMAND_SIG(g4_search)
CUSTOM_DOC("In Navigate Mode, search the file")
MODAL_COMMAND(search)

// TODO(caleb): implement a wrapping search
CUSTOM_COMMAND_SIG(g4_reverse_search)
CUSTOM_DOC("In Navigate Mode, search in reverse because you're too lazy to implement wrapping search.")
MODAL_COMMAND(reverse_search)

CUSTOM_COMMAND_SIG(g4_list_all_locations)
CUSTOM_DOC("In Navigate Mode, lists all locations of the case sensitive string in all buffers")
MODAL_COMMAND(list_all_locations)

// MAYBE(caleb): it'd be nice if this just set the range to the whole definition of
// symbol
// TODO(caleb): Implement a list all SYMBOLS in file
CUSTOM_COMMAND_SIG(g4_list_all_functions_current_buffer_lister)
CUSTOM_DOC("In Navigate Mode, Opens a lister with all functions in current buffer")
MODAL_COMMAND(list_all_functions_current_buffer_lister)

CUSTOM_COMMAND_SIG(g4_list_all_functions_all_buffers_lister)
CUSTOM_DOC("In Navigate Mode, Opens a lister with all functions in all open buffers")
MODAL_COMMAND(list_all_functions_all_buffers_lister)


// HACK(caleb): it'd be better to specify multiple commands in the bindings.4coder
// in case I want to rebind later, but w/e
CUSTOM_COMMAND_SIG(g4_save_or_return)
CUSTOM_DOC("In Navigate Mode, saves the file, otherwise runs the return command")
MODAL_INSERT_BLOCK({ save(app); }, if_read_only_goto_position)

CUSTOM_COMMAND_SIG(g4_save_all_or_shift_return)
CUSTOM_DOC("In Navigate Mode, saves the file, otherwise runs the return command")
MODAL_INSERT_BLOCK({ save_all_dirty_buffers(app); }, if_read_only_goto_position_same_panel)

// TODO(caleb): implement workspaces here
