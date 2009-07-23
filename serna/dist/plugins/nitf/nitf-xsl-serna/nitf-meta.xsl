<?xml version='1.0' encoding='utf-8'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format" version="1.0">

  <xsl:attribute-set name="bdr">
    <xsl:attribute name="border-width">1px</xsl:attribute>
    <xsl:attribute name="border-color">black</xsl:attribute>
    <xsl:attribute name="border-style">solid</xsl:attribute>
  </xsl:attribute-set>

  <xsl:template match="tobject">
    <fo:block text-align="center" padding-top="3pt" padding-bottom="3pt">
      <fo:table xsl:use-attribute-sets="bdr">
        <fo:table-body text-align="center" font-size="{$body.font.size}">
          <fo:table-row>
            <fo:table-cell xsl:use-attribute-sets="bdr">
              <xsl:if test="*">
                <xsl:attribute name="number-columns-spanned">
                  <xsl:value-of select="count(*)"/>
                </xsl:attribute>
              </xsl:if>
              <fo:block>
                <fo:block font-weight="bold">Subject code.</fo:block>
                <fo:block font-size="{$tiny.font.size}" font-style="italic"> Used to identify the type of the news material, based on a Subject Code system. </fo:block>
              </fo:block>
            </fo:table-cell>
          </fo:table-row>
          <xsl:if test="*">
            <fo:table-row>
              <xsl:for-each select="*">
                <fo:table-cell xsl:use-attribute-sets="bdr">
                  <fo:block>
                    <xsl:apply-templates select="."/>
                  </fo:block>
                </fo:table-cell>
              </xsl:for-each>
            </fo:table-row>
          </xsl:if>
        </fo:table-body>
      </fo:table>
    </fo:block>
  </xsl:template>

  <xsl:template match="docdata">
    <fo:block text-align="center" padding-top="3pt" padding-bottom="3pt">
      <fo:table xsl:use-attribute-sets="bdr">
        <fo:table-body text-align="center" font-size="{$body.font.size}">
          <fo:table-row>
            <fo:table-cell xsl:use-attribute-sets="bdr">
              <fo:block>
                <fo:block font-weight="bold">Document metadata.</fo:block>
                <fo:block font-size="{$tiny.font.size}" font-style="italic"> Container for metadata information about this particular document. </fo:block>
              </fo:block>
            </fo:table-cell>
          </fo:table-row>
          <xsl:if test="*">
            <xsl:for-each select="*">
              <fo:table-row>
                <fo:table-cell xsl:use-attribute-sets="bdr">
                  <fo:block>
                    <xsl:apply-templates select="."/>
                  </fo:block>
                </fo:table-cell>
              </fo:table-row>
            </xsl:for-each>
          </xsl:if>
        </fo:table-body>
      </fo:table>
    </fo:block>
  </xsl:template>

  <xsl:template match="tobject.property|tobject.subject|doc-id|del-list|urgency|fixture|
                       date.issue|date.release|date.expire|doc-scope|
                       series|du-key|ed-msg|doc.copyright|doc.rights|
                       key-list|from-src|keyword|correction|
                       evloc|identified-content|identified-content/*">
    <fo:block>
      <fo:inline font-weight="bold">
        <xsl:value-of select="local-name()"/>
      </fo:inline>
      <xsl:text>:</xsl:text>
      <xsl:if test="@*">
        <fo:inline font-size="{$small.font.size}" font-style="italic">
          <xsl:text> (attributes: </xsl:text>
          <xsl:for-each select="@*">
            <xsl:value-of select="local-name()"/>
            <xsl:text>=&quot;</xsl:text>
            <xsl:apply-templates select="."/>
            <xsl:text>&quot; </xsl:text>
          </xsl:for-each>
          <xsl:text>)</xsl:text>
        </fo:inline>
      </xsl:if>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="head/revision-history[1]">
    <fo:block padding-bottom="3pt" font-family="{$body.font.family}">
      <fo:block font-weight="bold" font-size="{$large.font.size}">Revision History:</fo:block>
      <fo:list-block start-indent="20pt" provisional-distance-between-starts="10em" provisional-label-separation="0.2em">
        <xsl:for-each select="../revision-history">
          <fo:list-item>
            <fo:list-item-label end-indent="label-end()">
              <xsl:choose>
                <xsl:when test="@name|@function">
                  <fo:block>
                    <xsl:if test="@function">
                      <fo:block>
                        <xsl:apply-templates select="@function"/>
                      </fo:block>
                    </xsl:if>
                    <xsl:if test="@name">
                      <fo:block>
                        <xsl:apply-templates select="@name"/>
                      </fo:block>
                    </xsl:if>
                  </fo:block>
                </xsl:when>
                <xsl:otherwise>
                  <fo:block>●</fo:block>
                </xsl:otherwise>
              </xsl:choose>
            </fo:list-item-label>
            <fo:list-item-body start-indent="body-start()">
              <xsl:choose>
                <xsl:when test="@norm|@comment">
                  <fo:block>
                    <xsl:if test="@norm">
                      <fo:block>
                        <xsl:apply-templates select="@norm"/>
                      </fo:block>
                    </xsl:if>
                    <xsl:if test="@comment">
                      <fo:block>
                        <xsl:apply-templates select="@comment"/>
                      </fo:block>
                    </xsl:if>
                  </fo:block>
                </xsl:when>
                <xsl:otherwise>
                  <fo:block>-</fo:block>
                </xsl:otherwise>
              </xsl:choose>
            </fo:list-item-body>
          </fo:list-item>
        </xsl:for-each>
      </fo:list-block>
    </fo:block>
  </xsl:template>

  <xsl:template match="revision-history"/>

  <xsl:template match="head/pubdata[1]">
    <fo:block padding-bottom="3pt" font-family="{$body.font.family}">
      <fo:block font-weight="bold" font-size="{$large.font.size}">Publication Data:</fo:block>
      <fo:block font-size="8pt" font-style="italic">Information about specific instance of an item&apos;s publication. Contains metadata about how the particular news object was used in a specific instance. </fo:block>
      <fo:list-block start-indent="20pt" provisional-distance-between-starts="10em" provisional-label-separation="0.2em">
        <xsl:for-each select="../pubdata">
          <fo:list-item>
            <fo:list-item-label end-indent="label-end()">
              <xsl:choose>
                <xsl:when test="@name|@type">
                  <fo:block>
                    <xsl:if test="@type">
                      <fo:block>
                        <xsl:apply-templates select="@type"/>
                      </fo:block>
                    </xsl:if>
                    <xsl:if test="@name">
                      <fo:block>
                        <xsl:apply-templates select="@name"/>
                      </fo:block>
                    </xsl:if>
                  </fo:block>
                </xsl:when>
                <xsl:otherwise>
                  <fo:block>●</fo:block>
                </xsl:otherwise>
              </xsl:choose>
            </fo:list-item-label>
            <fo:list-item-body start-indent="body-start()">
              <xsl:if test="@*[not(self::type)  and not(self::name)]">
                <fo:block>
                  <xsl:for-each select="@*[not(self::type)  and not(self::name)]">
                    <fo:inline font-size="{$small.font.size}" font-style="italic">
                      <xsl:value-of select="local-name()"/>
                      <xsl:text>=&quot;</xsl:text>
                      <xsl:apply-templates select="."/>
                      <xsl:text>&quot; </xsl:text>
                    </fo:inline>
                  </xsl:for-each>
                </fo:block>
              </xsl:if>
            </fo:list-item-body>
          </fo:list-item>
        </xsl:for-each>
      </fo:list-block>
    </fo:block>
  </xsl:template>

  <xsl:template match="pubdata"/>

  <xsl:template match="head/title">
    <fo:block font-size="{$large.font.size}" font-weight="bold">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="distributor">
    <fo:block text-align="right" color="gray">
      <fo:inline>Distributor:</fo:inline>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="dateline">
    <fo:block text-align="right" color="gray">
      <fo:inline>Dateline:</fo:inline>
      <fo:inline>
        <xsl:apply-templates/>
      </fo:inline>
    </fo:block>
  </xsl:template>

  <xsl:template match="abstract">
    <fo:block text-align="right" color="gray">
      <xsl:text>Abstract:</xsl:text>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="abstract/p[1]">
    <fo:inline>
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="rights">
    <fo:block text-align="right" color="gray">
      <fo:inline>Rights:</fo:inline>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="meta">
    <fo:block text-align="right" color="gray">
      <fo:inline>Meta:</fo:inline>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="body.head/series">
    <fo:block text-align="right" color="gray">
      <fo:inline>Series:</fo:inline>
      <xsl:if test="@*">
        <fo:inline font-size="{$small.font.size}" font-style="italic">
          <xsl:text> (attributes: </xsl:text>
          <xsl:for-each select="@*">
            <xsl:value-of select="local-name()"/>
            <xsl:text>=&quot;</xsl:text>
            <xsl:apply-templates select="."/>
            <xsl:text>&quot; </xsl:text>
          </xsl:for-each>
          <xsl:text>)</xsl:text>
        </fo:inline>
      </xsl:if>
    </fo:block>
  </xsl:template>

</xsl:stylesheet>

