SECTIONS {
 kernel_load_address = ((0x108000) + 0xffffffff80000000);
 .unmapped (0x108000 + SIZEOF_HEADERS): AT (0x108000 + SIZEOF_HEADERS) {
  unmapped_start = .;
  KEEP(*(K_TEXT_START));
  KEEP(*(K_DATA_START));
  KEEP(*(K_INI_PTLS));
  unmapped_end = .;
 }
 .text (((0x108000) + 0xffffffff80000000) + SIZEOF_HEADERS + SIZEOF(.unmapped)) : AT (0x108000 + SIZEOF_HEADERS + SIZEOF(.unmapped)) {
  ktext_start = .;
  *(.text .text.*);
  ktext_end = .;
 }
 .eh_frame_hdr : {
  eh_frame_hdr_start = .;
  *(.eh_frame_hdr) *(.eh_frame_entry .eh_frame_entry.*);
  eh_frame_hdr_end = .;
 }
 .eh_frame : {
  eh_frame_start = .;
  KEEP(*(.eh_frame .eh_frame.*));
  eh_frame_end = .;
 }
 .data : {
  kdata_start = .;
  *(.rodata .rodata.*);
  *(.data .data.*);
   *(.bss .bss.*);
  *(COMMON);
  kdata_end = .;
 }
 .comment 0 : { *(.comment); }
 .debug_abbrev 0 : { *(.debug_abbrev); }
 .debug_abbrev.dwo 0 : { *( .debug_abbrev.dwo); }
 .debug_addr 0 : { *(.debug_addr); }
 .debug_aranges 0 : { *(.debug_aranges); }
 .debug_cu_index 0 : { *(.debug_cu_index); }
 .debug_frame 0 : { *(.debug_frame); }
 .debug_frame_hdr 0 : { *(.debug_frame_hdr); }
 .debug_info 0 : { *(.debug_info); }
 .debug_info.dwo 0 : { *(.debug_info.dwo); }
 .debug_line 0 : { *(.debug_line); }
 .debug_line.dwo 0 : { *(.debug_line.dwo); }
 .debug_line_str 0 : { *(.debug_line_str); }
 .debug_loc 0 : { *(.debug_loc); }
 .debug_loclists 0 : { *(.debug_loclists); }
 .debug_loclists.dwo 0 : { *(.debug_loclists.dwo); }
 .debug_macinfo 0 : { *(.debug_macinfo); }
 .debug_macro 0 : { *(.debug_macro); }
 .debug_macro.dwo 0 : { *(.debug_macro.dwo); }
 .debug_names 0 : { *(.debug_names); }
 .debug_pubnames 0 : { *(.debug_pubnames); }
 .debug_pubtypes 0 : { *(.debug_pubtypes); }
 .debug_ranges 0 : { *(.debug_ranges); }
 .debug_rnglists 0 : { *(.debug_rnglists); }
 .debug_str 0 : { *(.debug_str); }
 .debug_str.dwo 0 : { *(.debug_str.dwo); }
 .debug_str_offsets 0 : { *(.debug_str_offsets); }
 .debug_str_offsets.dwo 0 : { *(.debug_str_offsets.dwo); }
 .debug_tu_index 0 : { *(.debug_tu_index); }
 .debug_types 0 : { *(.debug_types); }
 /DISCARD/ : {
  *(*);
 }
 ap_boot = unmapped_ap_boot - 0x108000 + 0x008000;
 ap_gdtr = unmapped_ap_gdtr - 0x108000 + 0x008000;
 protected_ap_gdtr = ((ap_gdtr) + 0xffffffff80000000);
}
