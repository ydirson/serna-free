SCRIPT  = config-template-documentor.xsl
SCHEMA  = ../../plugins/syntext/serna-config/serna-config.xsd
DB2DITA = docbook-to-dita.xsl
TOPICWRAP_XSL = config-template-topic.xsl

serna-config.dita: $(SCRIPT) $(SCHEMA) $(DB2DITA) $(TOPICWRAP_XSL)
	xsltproc $(SCRIPT) $(SCHEMA) | xsltproc $(DB2DITA) - | xsltproc -o $@ $(TOPICWRAP_XSL) -
