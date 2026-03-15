

bits 64

section .text

%include "string.asm"
%include "print.asm"
%include "disk_io.asm"

global _v_cursor
global _v_print
global _s_strcmp
global _io_read_sector
global _io_read_bytes


