<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                version='1.0'>
  <xsl:import href="timesheet.xsl"/>
  <xsl:output method="text" indent="no"/>
  <xsl:strip-space elements="report author nick period month year officialdays
                             tasks task task-metrics"/>

  <xsl:variable name="dash">
    <xsl:text>------------------------------------------------------------------</xsl:text>
  </xsl:variable>
  <xsl:variable name="ws">
    <xsl:text>                                                                  </xsl:text>
  </xsl:variable>
  <xsl:variable name="indent-shift" select="'4'"/>

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="/report">
    <xsl:apply-templates select="*[not(self::tasks)]"/>
    <xsl:variable name="total">
      <xsl:call-template name="sum">
        <xsl:with-param name="nodes" select="/report/tasks//task-metrics">
        </xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:text>Total:     </xsl:text>
    <xsl:value-of select="round($total)"/>
    <xsl:text>%&#xA;&#xA;</xsl:text>
    <xsl:apply-templates select="tasks"/>
    <xsl:text>&#xA;Total:</xsl:text>
    <xsl:value-of select="substring($ws, 1, 44)"/>
    <xsl:call-template name="planned-sum">
      <xsl:with-param name="nodes" select="/report/tasks//task-metrics"/>
    </xsl:call-template>
    <xsl:text>/</xsl:text>
    <xsl:value-of select="sum(/report/tasks//actual)"/>
    <xsl:text>  -   </xsl:text>
    <xsl:value-of select="round($total)"/>
    <xsl:text>%&#xA;</xsl:text>
  </xsl:template>

  <xsl:template match="author">
    <xsl:text>Author:    </xsl:text>
    <xsl:apply-templates/>
    <xsl:text>&#xA;</xsl:text>
  </xsl:template>

  <xsl:template match="period">
    <xsl:text>Timesheet: </xsl:text>
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="officialdays">
    <xsl:text>Workdays:  </xsl:text>
    <xsl:apply-templates/>
    <xsl:text>&#xA;</xsl:text>
  </xsl:template>

  <xsl:template match="month">
    <xsl:if test="text()">
      <xsl:apply-templates/>
      <xsl:text>, </xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="year">
    <xsl:apply-templates/>
    <xsl:text>&#xA;</xsl:text>
  </xsl:template>

  <xsl:template match="nick|firstname|surname|email">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="tasks">
    <xsl:text>Tasks (planned[corrected]/actual - complete %)</xsl:text>
    <xsl:text>&#xA;</xsl:text>
    <xsl:value-of select="$dash"/>
    <xsl:text>&#xA;</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>&#xA;</xsl:text>
    <xsl:value-of select="$dash"/>
  </xsl:template>

  <xsl:template match="task">
    <xsl:param name="indent">0</xsl:param>
    <xsl:text>&#xA;</xsl:text>
    <xsl:value-of select="substring($ws, 1, $indent)"/>
      <xsl:apply-templates select="task-name"/>
      <xsl:apply-templates select="task">
        <xsl:with-param name="indent" select="$indent + $indent-shift"/>
      </xsl:apply-templates>
      <xsl:value-of select="substring($ws, 1, (50 - $indent - string-length(task-name)))"/>
      <xsl:apply-templates select="task-metrics"/>
  </xsl:template>

  <xsl:template match="task-name">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="task-metrics">
    <xsl:variable name="days">
      <xsl:apply-templates select="planned"/>
      <xsl:if test="corrected">
        <xsl:text> [</xsl:text>
        <xsl:apply-templates select="corrected"/>
        <xsl:text>] </xsl:text>
      </xsl:if>
      <xsl:text>/</xsl:text>
      <xsl:apply-templates select="actual"/>
    </xsl:variable>
    <xsl:value-of select="$days"/>
    <xsl:value-of select="substring($ws, 1, 13 - string-length($days))"/>
      <xsl:choose>
        <xsl:when test="per-cent">
          <xsl:apply-templates select="per-cent"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:call-template name="calculate-per-cent"/>
            <xsl:text>%</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
  </xsl:template>

  <xsl:template match="planned|actual|corrected">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="per-cent">
    <xsl:apply-templates/>
    <xsl:text>%</xsl:text>
  </xsl:template>

</xsl:stylesheet>
