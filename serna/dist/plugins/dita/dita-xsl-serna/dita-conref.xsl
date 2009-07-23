<?xml version='1.0'?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:se="http://www.syntext.com/XSL/Format-1.0"
                xmlns:xse="http://www.syntext.com/Extensions/XSLT-1.0"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                extension-element-prefixes="dtm"                
                version='1.0'>

<!-- =================== in-editor conref support ==================== -->

<dtm:doc dtm:status="testing" dtm:idref="attribute.conref"/>
<xsl:template match="*[@conref]" priority="100" dtm:id="attribute.conref">
  <xsl:param name="conrefs-queue"/>
  <xsl:variable name="id">
    <xsl:value-of select="generate-id(.)"/>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="string-length(@conref) = 0">
      <xsl:variable name="error-message">
        <xsl:text>[Conref is empty]</xsl:text>
      </xsl:variable>
      <xsl:call-template name="show-conref-error">
        <xsl:with-param name="message" select="$error-message"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="contains($conrefs-queue, $id)">
      <xsl:message>
        <xsl:value-of select="$conrefs-queue"/>
      </xsl:message>
      <xsl:variable name="error-message">
        <xsl:text>[Cyclic conref]</xsl:text>
      </xsl:variable>
      <xsl:call-template name="show-conref-error">
        <xsl:with-param name="message" select="$error-message"/>
      </xsl:call-template>      
    </xsl:when>
    <xsl:when test="$SHOW-CONREF-RESOLVED='yes'">
      <xsl:variable name="queue" select="concat($conrefs-queue, '/', $id)"/>
      <xsl:call-template name="href">
        <xsl:with-param name="href" select="@conref"/>
        <xsl:with-param name="type" select="'conref'"/>
        <xsl:with-param name="conrefs-queue" select="$queue"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="hide-resolved-conref"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="debug-conref">
  <xsl:variable name="element">
    <xsl:value-of select="local-name(.)"/>
  </xsl:variable>

  <xsl:variable name="file">
    <xsl:choose>
      <xsl:when test="contains(@conref,'#')">
        <xsl:value-of select="$WORKDIR"/>
        <xsl:value-of select="substring-before(@conref,'#')"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$WORKDIR"/>
        <xsl:value-of select="@conref"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:param name="topicid"/>
  <xsl:param name="elemid"/>


  <xsl:variable name="topicpos">
    <xsl:choose>
      <xsl:when test="@conref = ''">none</xsl:when>
      <xsl:when test="starts-with(@conref,'#')">samefile</xsl:when>
      <xsl:when test="contains(@conref,'#')">otherfile</xsl:when>
      <xsl:otherwise>firstinfile</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:call-template name="output-message">
    <xsl:with-param name="msg">Conref trace: </xsl:with-param>
    <xsl:with-param name="msgnum">006</xsl:with-param>
    <xsl:with-param name="msgsev">I</xsl:with-param>
  </xsl:call-template>
  <xsl:message> Element: <xsl:value-of select="name()"/>:<xsl:value-of select="$element"/></xsl:message>
  <xsl:message> Working dir: <xsl:value-of select="$WORKDIR"/></xsl:message>
  <xsl:message> File: <xsl:value-of select="$file"/></xsl:message>
  <xsl:message> Topicid: <xsl:value-of select="$topicid"/></xsl:message>
  <xsl:message> Elemid: <xsl:value-of select="$elemid"/></xsl:message>
  <xsl:message> Topic location: <xsl:value-of select="$topicpos"/></xsl:message>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="hide-resolved-conref"/>
<xsl:template name="hide-resolved-conref" dtm:id="hide-resolved-conref">
  <xsl:choose>
    <xsl:when test="contains(@class,' topic/strow ') or 
                    contains(@class,' topic/row ') or 
                    contains(@class,' map/relrow ')">
      <xsl:call-template name="row.conref"/>
    </xsl:when>
    <xsl:when test="contains(@class,' topic/table ') or 
                    contains(@class,' topic/simpletable ') or 
                    contains(@class,' map/reltable ')">
      <xsl:call-template name="table.conref"/>
    </xsl:when>
    <xsl:when test="contains(@class, ' topic/thead ') or 
                    contains(@class, ' topic/tbody ') or 
                    contains(@class, ' topic/tfoot ')">
      <xsl:call-template name="tbody.conref"/>
    </xsl:when>
    <xsl:when test="contains(@class,' topic/ul ') or
                    contains(@class,' topic/ dl ') or
                    contains(@class,' topic/sl ') or
                    contains(@class,' topic/ol ')">
      <xsl:call-template name="lists.conref"/>
    </xsl:when>
    <xsl:when test="contains(@class,' topic/li ')">
      <xsl:call-template name="li.conref"/>
    </xsl:when>
    <xsl:when test="contains(@class,' topic/dlentry ')">
      <xsl:call-template name="dlentry.conref"/>
    </xsl:when>
    <xsl:when test="contains(@class,' topic/stentry ')">
      <xsl:call-template name="stentry.conref"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="process-resolved-conref"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

  <dtm:doc dtm:status="testing"
           dtm:idref="conref.apply"/>
  <xsl:template name="apply.conref" dtm:id="conref.apply">
    <xsl:param name="content"/>
    <xsl:param name="conrefs-queue"/>
    <xsl:variable name="error-message">
      <xsl:text>[Invalid conref: </xsl:text>
      <xsl:value-of select="@conref"/>
      <xsl:text>]</xsl:text>    
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="not($content)">
        <xsl:call-template name="show-conref-error">
          <xsl:with-param name="message" select="$error-message"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="self::*[contains(@class, ' topic/topic ')] or
                          self::*[contains(@class, ' map/map ')]">
            <fo:block>
              <xsl:apply-templates select="$content">
                <xsl:with-param name="conref-context" select="."/>
                <xsl:with-param name="conrefs-queue" select="$conrefs-queue"/>
              </xsl:apply-templates>
            </fo:block>
          </xsl:when>
          <xsl:otherwise>
            <xsl:apply-templates select="$content">
              <xsl:with-param name="conref-context" select="."/>
              <xsl:with-param name="conrefs-queue" select="$conrefs-queue"/>
            </xsl:apply-templates>
          </xsl:otherwise>
        </xsl:choose>      
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <dtm:doc dtm:status="testing"
           dtm:idref="process.resolved.conref"/>
  <xsl:template name="process-resolved-conref" dtm:id="process.resolved.conref">
    <xsl:param name="apply-children">1</xsl:param>
    <fo:inline background-color="#f0ffff">
      <fo:inline font-weight="bold">
          <xsl:text>&lt;</xsl:text>
          <xsl:value-of select="name()"/> 
          <xsl:text> conref="</xsl:text>
          <xsl:value-of select="@conref"/>
          <xsl:text>"&gt;</xsl:text>
      </fo:inline>
      <xsl:if test="$apply-children=1">
        <xsl:apply-templates/>
      </xsl:if>
      <xsl:if test="node()[1]">
        <fo:inline font-weight="bold">
          <xsl:text>&lt;</xsl:text>
          <xsl:value-of select="name()"/>
          <xsl:text>&gt;</xsl:text>
        </fo:inline>
      </xsl:if>
    </fo:inline>
  </xsl:template>

  <dtm:doc dtm:status="testing"
           dtm:idref="process.cyclic.conref"/>
  <xsl:template name="process-cyclic-conref" dtm:id="process.cyclic.conref">
    <fo:block>
      <xsl:variable name="error-message">
        <xsl:text>[Cyclic conref: </xsl:text>
        <xsl:value-of select="@conref"/>
        <xsl:text>]</xsl:text>
      </xsl:variable>
      <xsl:call-template name="show-conref-error">
        <xsl:with-param name="message" select="$error-message"/>
      </xsl:call-template>
      <fo:block>
        <xsl:call-template name="process-resolved-conref"/>
      </fo:block>
    </fo:block>
  </xsl:template>

<!-- conref resolved for different elements -->

<dtm:doc dtm:elements="stentry"
         dtm:status="testing" 
         dtm:idref="stentry.conref"/>
<xsl:template name="stentry.conref" dtm:id="stentry.conref">
  <fo:table-cell xsl:use-attribute-sets="frameall">
    <xsl:call-template name="stentry.colnum"/>
    <fo:block>
      <xsl:call-template name="process-resolved-conref"/>
    </fo:block>
  </fo:table-cell>
</xsl:template>

<dtm:doc dtm:elements="li"
         dtm:status="testing" 
         dtm:idref="li.conref"/>
<xsl:template name="li.conref" dtm:id="li.conref">
  <fo:list-item padding-bottom="0.4em">
    <fo:list-item-label end-indent="label-end()" text-align="end"> 
      <fo:block>
        <fo:inline>&#x2022;</fo:inline>
      </fo:block>
    </fo:list-item-label>
    <fo:list-item-body start-indent="body-start()"> 
      <fo:block> 
        <xsl:call-template name="process-resolved-conref"/>
      </fo:block> 
    </fo:list-item-body> 
  </fo:list-item>
</xsl:template>

<dtm:doc dtm:elements="ul|dl|sl|ol"
         dtm:status="testing" 
         dtm:idref="lists.conref"/>
<xsl:template name="lists.conref" dtm:id="lists.conref">
  <fo:list-block xsl:use-attribute-sets="ul">
    <fo:list-item padding-bottom="0.4em">
      <fo:list-item-label end-indent="label-end()" text-align="end">
        <fo:block>
          <fo:inline>&#x2022;</fo:inline>
        </fo:block>
      </fo:list-item-label>
      <fo:list-item-body start-indent="body-start()">
        <fo:block>
          <xsl:call-template name="process-resolved-conref"/>
        </fo:block>
      </fo:list-item-body>
    </fo:list-item>
  </fo:list-block>
</xsl:template>

<dtm:doc dtm:elements="tbody|thead|tfoot"
         dtm:status="testing" 
         dtm:idref="tbody.conref"/>
<xsl:template name="tbody.conref" dtm:id="tbody.conref">
  <fo:table-body>
    <fo:table-row>
      <xsl:variable name="table.cols" select=
          "ancestor::*[contains(@class, ' topic/tgroup ')][1]/@cols"/>
      <fo:table-cell xsl:use-attribute-sets=
           "table.cell.attributes preamble.attributes" 
           column-number="1" number-columns-spanned="$table.cols">
        <fo:block font-size="8pt">
          <xsl:call-template name="process-resolved-conref">
            <xsl:with-param name="apply-children">0</xsl:with-param>
          </xsl:call-template>
        </fo:block>
      </fo:table-cell>
    </fo:table-row>
  </fo:table-body>
</xsl:template>

<dtm:doc dtm:elements="table|reltable|simpletable"
         dtm:status="testing" 
         dtm:idref="table.conref"/>
<xsl:template name="table.conref" dtm:id="table.conref">
  <fo:block padding-top="1em">
    <fo:table xsl:use-attribute-sets="table.data frameall">
      <fo:table-body>
        <fo:table-row>
          <fo:table-cell xsl:use-attribute-sets="frameall" column-number="1">
            <fo:block>
              <xsl:call-template name="process-resolved-conref">
                <xsl:with-param name="apply-children">0</xsl:with-param>
              </xsl:call-template>
            </fo:block>
          </fo:table-cell>
        </fo:table-row>
      </fo:table-body>
    </fo:table>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="row|strow|relrow"
         dtm:status="testing" 
         dtm:idref="row.conref"/>
<xsl:template name="row.conref" dtm:id="row.conref">
  <fo:table-row>
    <fo:table-cell xsl:use-attribute-sets="frameall" column-number="1">
      <fo:block>
        <xsl:call-template name="process-resolved-conref">
          <xsl:with-param name="apply-children">0</xsl:with-param>
        </xsl:call-template>
      </fo:block>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>

<dtm:doc dtm:elements="dlentry"
         dtm:status="testing" 
         dtm:idref="dlentry.conref"/>
<xsl:template name="dlentry.conref" dtm:id="dlentry.conref">
  <fo:block> 
    <xsl:call-template name="process-resolved-conref"/>
  </fo:block> 
</xsl:template>

<dtm:doc dtm:elements="*"
         dtm:status="testing" 
         dtm:idref="show-conref-error"/>
<xsl:template name="show-conref-error" dtm:id="show-conref-error">
  <xsl:param name="message"/>
  <xsl:choose>
    <xsl:when test="self::*[contains(@class, ' topic/topic ')] or
                    self::*[contains(@class, ' map/map ')]">
      <fo:block color="red" font-weight="bold">
        <xsl:value-of select="$message"/>
      </fo:block>
    </xsl:when>
    <xsl:when test="self::*[contains(@class, ' topic/li ')]">
      <fo:list-item padding-bottom="0.4em">
        <fo:list-item-label end-indent="label-end()" text-align="end">
          <xsl:call-template name="generate-listitem-label">
            <xsl:with-param name="list-level">
              <xsl:call-template name="get-list-level"/>
            </xsl:with-param>
          </xsl:call-template>
        </fo:list-item-label>
        <fo:list-item-body start-indent="body-start()">
          <fo:block color="red" font-weight="bold">
            <xsl:value-of select="$message"/>
          </fo:block>
        </fo:list-item-body>
      </fo:list-item>
    </xsl:when>
    <xsl:when test="self::*[contains(@class, ' topic/entry ')] or
                    self::*[contains(@class, ' topic/strow ')] or
                    self::*[contains(@class, ' topic/stentry ')] or
                    self::*[contains(@class, ' topic/sthead ')]">
      <fo:table-cell>
        <fo:block color="red" font-weight="bold">
          <xsl:value-of select="$message"/>
        </fo:block>
      </fo:table-cell>
    </xsl:when>
    <xsl:otherwise>
      <fo:inline color="red" font-weight="bold">
        <xsl:value-of select="$message"/>
      </fo:inline>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>