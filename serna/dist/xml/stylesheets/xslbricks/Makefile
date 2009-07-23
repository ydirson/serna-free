# WARNING: WORKS WITH GNU MAKE ONLY!

XT				= saxon

SITE_FILES 		:= $(shell find . -name '*.html' -or -name '*.xhtml'\
-or -name '*.htm' -or -name '*.gif' -or -name '*.png' -or -name '*.jpg'\
-or -name '*.xhtm' -or -name '*.rtf' -or -name '*.zip' -or -name '*.tgz'\
-or -name '*.txt' -or -name '*.xsd' -or -name '*.dtd' -or -name '*.css')

SITE_DIRS		:= $(shell find . -type d -and \! -name '*CVS')

.SUFFIXES: .html .htm .xsl .xml .xhtml .htm .html

all: html/inlines.xsl fo/inlines.xsl html/blocks.xsl fo/blocks.xsl\
fo/lists.xsl html/lists.xsl fo/simple-tables.xsl html/simple-tables.xsl

html/inlines.xsl: inlines_template.xsl backend-generator.xsl
	$(XT) inlines_template.xsl backend-generator.xsl $@ output='html'

fo/inlines.xsl: inlines_template.xsl backend-generator.xsl
	$(XT) inlines_template.xsl backend-generator.xsl $@ output='fo'

html/blocks.xsl: blocks_template.xsl backend-generator.xsl
	$(XT) blocks_template.xsl backend-generator.xsl $@ output='html'

fo/blocks.xsl: blocks_template.xsl backend-generator.xsl
	$(XT) blocks_template.xsl backend-generator.xsl $@ output='fo'

html/lists.xsl: lists_template.xsl backend-generator.xsl
	$(XT) lists_template.xsl backend-generator.xsl $@ output='html'

fo/lists.xsl: lists_template.xsl backend-generator.xsl
	$(XT) lists_template.xsl backend-generator.xsl $@ output='fo'

html/simple-tables.xsl: simple-tables_template.xsl backend-generator.xsl
	$(XT) simple-tables_template.xsl backend-generator.xsl $@ output='html'

fo/simple-tables.xsl: simple-tables_template.xsl backend-generator.xsl
	$(XT) simple-tables_template.xsl backend-generator.xsl $@ output='fo'
