<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
  xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="street|street2|city|county|country|
                    postalCode|state|province|zip|suburb|ward">
  <xsl:call-template name="inline"/>
</xsl:template>

</xsl:stylesheet>
