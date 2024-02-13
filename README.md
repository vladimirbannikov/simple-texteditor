## 
Simple vim-like text editor with TUI. There are 4 modes - navigation, input, search and command mode.
![Image alt]()
### Commands
#### NAVIGATION MODE
* B - Enter text before the cursor
* I - Go to the beginning of the line and start entering text
* S - Go to the end of the line and start typing
* A - Delete the contents of the row and start typing
* : - Activating the command input mode
* / - Activates search mode until the end of the document
* ? - Activates search mode until the start of the document
* RIGHT - Moves the cursor one position to the right if it is the end of the line moves to the first position of the next line
* LEFT - Moves the cursor one position to the left if this is the beginning of the line moves to the end of the previous line
* UP - Moves the cursor up one position (if next line shorter, then moves to the end of this line) if this is the beginning of the screen doing a scroll
* DOWN - Moves the cursor one position (if next line shorter, then moves to the end of this line) down if it is the end of the screen doing a scroll
* PAGE_UP - Raises the cursor one page
* PAGE_DOWN - Moves the cursor down one page
* gg - Moves the cursor to the first page of the text
* G - Moves the cursor to the last page of the text
* ^ - Moves the cursor to the start of the line
* $ - Moves the cursor to the end of the line
* w - Move the cursor to the end of a word to the right of the cursor (if the cursor is at the end of a word, then move the cursor to the end of the next word)
* b - Move the cursor to the beginning of the word to the left of the cursor (if the cursor is at the beginning of the word, then move the cursor to the beginning of the previous word)
* x - Delete the character after the cursor
* di - Delete the word under the cursor, including one space on the right
* dd - Cut (copy and delete) the current line
* y - Copy the current line
* dc - Copy the word under the cursor
* p - Insert after the cursor
* NG - Go to the line with the number N

#### COMMAND MODE
* o <filename> - Open the file <filename>
* x - Write to the current file and close it
* w - Write to the current file
* w <filename> - Write to file <filename>
* q - Exit program
* q! - Exit program without saving current file
* wq! - Write to the current file and close it
* line <num> - Go to the line with the number <num>
* set num - Turn on/To disable line numbering

#### FIND MODE
* / <text> - Search for the <text> string from the cursor to the end of the file. If a line is found, move the cursor to the beginning of the line.
* ? <text> - Search for the <text> string from the cursor to the beginning of the file. If a line is found, move the cursor to the beginning of the line.
* n - Repeat search forward.
* p - Repeat search back.


