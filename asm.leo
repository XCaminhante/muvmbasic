<?xml version="1.0" encoding="utf-8"?>
<!-- Created by Leo: http://leoeditor.com/leo_toc.html -->
<leo_file xmlns:leo="http://leoeditor.com/namespaces/leo-python-editor/1.1" >
<leo_header file_format="2"/>
<globals/>
<preferences/>
<find_panel_settings/>
<vnodes>
<v t="caminhante.20220906181437.1"><vh>mu0asm</vh>
<v t="caminhante.20220908221159.1"><vh>@clean Makefile</vh></v>
<v t="caminhante.20220906183805.1"><vh>@file muasm.c</vh></v>
</v>
</vnodes>
<tnodes>
<t tx="caminhante.20220906181437.1"></t>
<t tx="caminhante.20220908221159.1">@tabwidth 4
CFLAGS := -std=gnu99 -Wall -Werror -Wno-unused-function -Wfatal-errors -Os

.PHONY: all
all: muasm

muasm: muasm.c
	gcc $(CFLAGS) $&lt; -o $@</t>
</tnodes>
</leo_file>
