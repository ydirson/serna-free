<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                version='1.0'>

  <xsl:import href="../xslbricks/fo/fonts.xsl"/>

  <!-- Calculation of the timesheet comes from the following formula: 
       
       Sign "A|B|C" means "if exist, for calculation prefer A over B, prefer B over C".

       Completeness of each task is:

       tprc = limit[max=1] {per-cent|complete|actual/(corrected|planned)}

       Total is:

       Total = S {(corrected|planned/officialdays) * tprc}

       -->

  <xsl:strip-space elements="*"/>
  <xsl:output method="xml"/>

  <!-- The variables -->
  <xsl:variable name="font-size" select="'12pt'"/>
  <xsl:variable name="help-font-size" select="'10pt'"/>
  <xsl:variable name="header-font-size" select="'12pt'"/>
  <xsl:variable name="font-family" select="$serif.font.family"/>
  <xsl:variable name="indent-shift" select="'20'"/>
  <xsl:variable name="mwidth" select="'320'"/>
  <xsl:variable name="columns">
    <fo:table-column column-width="8cm"/>
    <fo:table-column column-width="2.5cm" number-columns-repeated="4"/>
  </xsl:variable>
  <!-- -->

  <xsl:include href="simple_master_set.xsl"/>

  <!-- Templates for counting sophisticated sums -->

  <xsl:template name="planned-sum">
    <xsl:param name="nodes" select="."/>
    <xsl:variable name="first">
      <xsl:choose>
        <xsl:when test="$nodes[1]/corrected/text()">
          <xsl:value-of select="$nodes[1]/corrected"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$nodes[1]/planned"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="count($nodes) > 1">
        <xsl:variable name="rest">
          <xsl:call-template name="planned-sum">
            <xsl:with-param name="nodes" select="$nodes[position() > 1]">
            </xsl:with-param>
          </xsl:call-template>
        </xsl:variable>
        <xsl:value-of select="$rest + $first"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$first"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="sum">
    <xsl:param name="nodes" select="."/>
    <xsl:variable name="first">
      <xsl:call-template name="calculate-task-portion">
        <xsl:with-param name="node" select="$nodes[1]"/>
      </xsl:call-template>          
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="count($nodes) > 1">
        <xsl:variable name="rest">
          <xsl:call-template name="sum">
            <xsl:with-param name="nodes" select="$nodes[position() > 1]">
            </xsl:with-param>
          </xsl:call-template>
        </xsl:variable>
        <xsl:value-of select="$rest + $first"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$first"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="calculate-per-cent">
    <xsl:param name="node" select="."/>

    <xsl:variable name="per-cent">
      <xsl:choose>
        <xsl:when test="$node/per-cent">
          <xsl:value-of select="number($node/per-cent) div 100"/>
        </xsl:when>
        <xsl:when test="$node/@complete">
          <xsl:text>1</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:choose>
            <xsl:when test="$node/corrected">
              <xsl:value-of select="$node/actual div $node/corrected"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="$node/actual div $node/planned"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:variable name="limited">
      <xsl:choose>
        <xsl:when test="$per-cent &gt; 1">
          <xsl:text>1</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$per-cent"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:value-of select="round($limited * 100)"/>
  </xsl:template>

  <xsl:template name="calculate-task-portion">
    <xsl:param name="node" select="."/>
    <xsl:param name="workdays" select="/report/period/officialdays"/>

    <xsl:variable name="bottomline">
      <xsl:choose>
        <xsl:when test="$node/corrected">
          <xsl:value-of select="$node/corrected"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$node/planned"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:variable name="per-cent">
      <xsl:call-template name="calculate-per-cent">
        <xsl:with-param name="node" select="$node"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:value-of select="$bottomline div $workdays * $per-cent"/>
  </xsl:template>

  <!-- End of templates for counting sophisticated sums -->

  <xsl:template match="/report">
    <fo:block 
      font-size="{$font-size}" 
      font-family="{$font-family}">
      <xsl:apply-templates select="*[not(self::tasks)]"/>
      <xsl:variable name="total">
        <xsl:call-template name="sum">
          <xsl:with-param name="nodes" select="/report/tasks//task-metrics"/>
        </xsl:call-template>
      </xsl:variable>
      <fo:block>
        <fo:inline font-weight="bold" font-size="16pt">
          <xsl:text>Total: </xsl:text>
          <xsl:value-of select="round($total)"/>
          <xsl:text>%</xsl:text>
        </fo:inline>
      </fo:block>
      <xsl:apply-templates select="tasks"/>
      <fo:block font-size="{$help-font-size}">
        <fo:block border-bottom-width="0.5em">
          <xsl:text>The following rules apply:</xsl:text>
        </fo:block>
      <fo:list-block provisional-distance-between-starts="1em"
            provisional-label-separation="0.5em">
        <xsl:call-template name="gen-list-item">
          <xsl:with-param name="content">
            <xsl:text>"Planned" is the planned days for the task. If "planned" was a lot different from reality, then "corrected" should be considered instead (leave "planned" untoched!").</xsl:text>
          </xsl:with-param>
        </xsl:call-template>
        <xsl:call-template name="gen-list-item">
          <xsl:with-param name="content">
            <xsl:text>If task took less days, but anyway complete, then mark appropriate "task-metrics" with attribute "complete=1", percentage will then be 100% and red.</xsl:text>
          </xsl:with-param>
        </xsl:call-template>
        <xsl:call-template name="gen-list-item">
          <xsl:with-param name="content">
            <xsl:text>Task completeness (tprc) is: actual/(corrected|planned). If this persentage should be different you can override it with value in per-cent tag.</xsl:text>
          </xsl:with-param>
        </xsl:call-template>
        </fo:list-block>
        <fo:block padding-top="1em">The total value of work is: SUM { (corrected|planned)/wdays * tprc }</fo:block>
      </fo:block>
    </fo:block>
  </xsl:template>

  <!-- Helper template for rendering items in help itemizedlist -->
  <xsl:template name="gen-list-item">
    <xsl:param name="content" select="'None'"/>
    <fo:list-item>
      <fo:list-item-label end-indent="label-end()">
        <fo:block>&#x2022;</fo:block>
      </fo:list-item-label>
      <fo:list-item-body start-indent="body-start()">
        <fo:block>
          <xsl:copy-of select="$content"/>
        </fo:block>
      </fo:list-item-body>
    </fo:list-item>
  </xsl:template>

  <xsl:template match="author">
    <fo:block>
      <fo:inline font-weight="bold" font-size="16pt">Author: </fo:inline>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="period">
    <fo:block>
      <fo:inline font-weight="bold" font-size="16pt">
        <xsl:text>Timesheet: </xsl:text>
      </fo:inline>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="officialdays">
    <fo:block>
      <fo:inline font-weight="bold" font-size="16pt">
        <xsl:text>Workdays: </xsl:text>
      </fo:inline>
      <fo:inline><xsl:apply-templates/></fo:inline>
    </fo:block>
  </xsl:template>

  <xsl:template match="month">
    <fo:inline>
      <xsl:if test="text()">
        <xsl:apply-templates/>
        <xsl:text>, </xsl:text>
      </xsl:if>
    </fo:inline>
  </xsl:template>

  <xsl:template match="year">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="nick|firstname|surname|email">
    <fo:inline font-style="italic"><xsl:apply-templates/>&#160;&#160;</fo:inline>
  </xsl:template>

  <!-- Helper template for rendering header cells -->
  <xsl:template name="gen-task-header">
    <xsl:param name="content" select="'None'"/>
    <fo:table-cell>
      <fo:block
        border-style="solid"                 
        border-top-width="0pt"
        border-left-width="0pt"
        border-right-width="0pt"
        border-bottom-width="1pt"
        border-bottom-color="#f00000">
        <xsl:copy-of select="$content"/>
      </fo:block>
    </fo:table-cell>
  </xsl:template>

  <!-- Helper template for rendering footer cells -->
  <xsl:template name="gen-task-footer">
    <xsl:param name="content" select="'None'"/>
    <fo:table-cell>
      <fo:block
        border-style="solid"                 
        border-bottom-width="0pt"
        border-left-width="0pt"
        border-right-width="0pt"
        border-top-width="1pt"
        border-top-color="#f00000">
        <xsl:copy-of select="$content"/>
      </fo:block>
    </fo:table-cell>
  </xsl:template>

  <xsl:template match="tasks">
    <xsl:variable name="total">
      <xsl:call-template name="sum">
        <xsl:with-param name="nodes" select="/report/tasks//task-metrics"/>
      </xsl:call-template>
    </xsl:variable>
    <fo:block
        border-style="solid"
        padding-top="1em"
        border-top-width="0pt"
        border-left-width="0pt"
        border-right-width="0pt"
        border-bottom-width="0pt"
        padding-bottom="1em">
      <fo:table>
        <xsl:copy-of select="$columns"/>
        <fo:table-body>
          <fo:table-row
            font-weight="bold" 
            font-size="{$header-font-size}">
            <xsl:call-template name="gen-task-header">
              <xsl:with-param name="content">
                <xsl:text>Task Name</xsl:text>
              </xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="gen-task-header">
              <xsl:with-param name="content">
                <xsl:text>Planned</xsl:text>
              </xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="gen-task-header">
              <xsl:with-param name="content">
                <xsl:text>Corrected</xsl:text>
              </xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="gen-task-header">
              <xsl:with-param name="content">
                <xsl:text>Actual</xsl:text>
              </xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="gen-task-header">
              <xsl:with-param name="content">
                <xsl:text>Complete</xsl:text>
              </xsl:with-param>
            </xsl:call-template>
          </fo:table-row>
          <xsl:apply-templates/>
          <fo:table-row
            font-weight="bold" 
            font-size="{$header-font-size}">
            <xsl:call-template name="gen-task-footer">
              <xsl:with-param name="content">
                <xsl:text>Total:</xsl:text>
              </xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="gen-task-footer">
              <xsl:with-param name="content">
                <xsl:call-template name="planned-sum">
                  <xsl:with-param name="nodes" select="/report/tasks//task-metrics"/>
                </xsl:call-template>
              </xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="gen-task-footer">
              <xsl:with-param name="content">
                <xsl:text>-</xsl:text>
              </xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="gen-task-footer">
              <xsl:with-param name="content">
                <xsl:value-of select="sum(/report/tasks//actual)"/>
              </xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="gen-task-footer">
              <xsl:with-param name="content">
                <xsl:value-of select="round($total)"/>
                <xsl:text>%</xsl:text>
              </xsl:with-param>
            </xsl:call-template>
          </fo:table-row>
        </fo:table-body>
      </fo:table>
    </fo:block>
  </xsl:template>

  <xsl:template match="task">
    <xsl:param name="indent">0</xsl:param>
      <xsl:choose>
        <xsl:when test="task-metrics">
          <fo:table-row>
            <fo:table-cell>
              <xsl:apply-templates select="task-name">
                <xsl:with-param name="indent" select="$indent"/>
              </xsl:apply-templates>
              <!-- TRICKY: If task-metrics is not there, but se:choice -->
              <xsl:apply-templates select="processing-instruction('se:choice')"/>
            </fo:table-cell>
              <!-- TRICKY: Match here task-metrics and never se:choice -->
              <xsl:apply-templates select="task-metrics[not(self::processing-instruction('se:choice'))]"/>
          </fo:table-row>
        </xsl:when>
        <xsl:otherwise>
          <fo:table-row>
            <fo:table-cell number-columns-spanned="5">
              <!-- For each subtast we generated included table
                   otherwise subtasks will not be shown as child of its parent. -->
              <fo:table border-top-width="0pt"
                                    border-bottom-width="0pt">
                <xsl:copy-of select="$columns"/>
                <fo:table-body>
                  <fo:table-row>
                    <fo:table-cell  number-columns-spanned="5">
                      <fo:block     border-top-width="0pt"
                                    border-bottom-width="0pt"
                                    background-color="#e5e5e5">
                      <xsl:apply-templates select="task-name">
                        <xsl:with-param name="indent" select="$indent"/>
                      </xsl:apply-templates>
                      </fo:block>
                    </fo:table-cell>
                  </fo:table-row>
                  <xsl:apply-templates select="task">
                    <xsl:with-param name="indent" select="$indent + $indent-shift"/>
                  </xsl:apply-templates>
                </fo:table-body>
              </fo:table>
            </fo:table-cell>
          </fo:table-row>
        </xsl:otherwise>
      </xsl:choose>
  </xsl:template>

  <xsl:template match="task-name">
    <xsl:param name="indent">0</xsl:param>
    <fo:block    
      start-indent="{concat($indent, 'pt')}"
      border-bottom-width="0pt"
      border-top-width="0pt">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="task-metrics">
    <xsl:choose>
      <xsl:when test="planned">
        <xsl:apply-templates select="planned"/>
      </xsl:when>
      <xsl:otherwise>
        <fo:table-cell>
          <fo:block><xsl:text>-</xsl:text></fo:block>
        </fo:table-cell>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="corrected">
        <fo:table-cell>
          <fo:block><xsl:apply-templates select="corrected"/></fo:block>
        </fo:table-cell>
      </xsl:when>
      <xsl:otherwise>
        <fo:table-cell>
          <fo:block><xsl:text>-</xsl:text></fo:block>
        </fo:table-cell>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="actual">
        <fo:table-cell>
          <fo:block><xsl:apply-templates select="actual"/></fo:block>
        </fo:table-cell>
      </xsl:when>
      <xsl:otherwise>
        <fo:table-cell>
          <fo:block><xsl:text>-</xsl:text></fo:block>
        </fo:table-cell>
      </xsl:otherwise>
    </xsl:choose>
    <fo:table-cell>
      <fo:block>
      <xsl:choose>
        <xsl:when test="per-cent">
          <xsl:apply-templates select="per-cent"/>
        </xsl:when>
        <xsl:otherwise>
          <fo:inline>
            <xsl:if test="@complete">
              <xsl:attribute name="color">
                <xsl:text>#f00000</xsl:text>
              </xsl:attribute>
            </xsl:if>
            <xsl:call-template name="calculate-per-cent"/>
            <xsl:text>%</xsl:text>
          </fo:inline>
        </xsl:otherwise>
      </xsl:choose>
      </fo:block>
    </fo:table-cell>
  </xsl:template>

  <xsl:template match="planned">
    <fo:table-cell>
      <fo:block>
        <xsl:apply-templates/>
      </fo:block>
    </fo:table-cell>
  </xsl:template>

  <xsl:template match="actual|corrected">
        <fo:inline>
          <xsl:apply-templates/>
        </fo:inline>
  </xsl:template>

  <xsl:template match="per-cent">
    <fo:inline font-style="italic"><xsl:apply-templates/>%</fo:inline>
  </xsl:template>

</xsl:stylesheet>
