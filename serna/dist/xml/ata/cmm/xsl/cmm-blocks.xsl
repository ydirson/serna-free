<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="para">
    <fo:block padding-top="0.2em">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="para/grphcref">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="warning|caution|note">
    <fo:list-block padding-top="0em" padding-bottom="0em"
                   padding-left="0em" padding-right="0em"
                   provisional-label-separation="0.2em">
      <xsl:choose>
        <xsl:when test="self::note">
          <xsl:attribute name="provisional-distance-between-starts">4em</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="provisional-distance-between-starts">6em</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>

      <fo:list-item>
        <fo:list-item-label end-indent="label-end()">
          <fo:block padding-top="3pt" text-decoration="underline">
            <xsl:choose>
              <xsl:when test="self::note">
                <xsl:text>NOTE:</xsl:text>
              </xsl:when>
              <xsl:when test="self::caution">
                <xsl:text>CAUTION:</xsl:text>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>WARNING:</xsl:text>
              </xsl:otherwise>
            </xsl:choose>
          </fo:block>
        </fo:list-item-label>
        <fo:list-item-body start-indent="body-start()">
          <fo:block padding-top="0em" padding-bottom="0em"
                    padding-left="0em" padding-right="0em">
            <xsl:apply-templates/>
          </fo:block>
        </fo:list-item-body>
      </fo:list-item>
    </fo:list-block>
  </xsl:template>

  <!-- element TODO -->
  <xsl:template match="accessto|accesstodata|acctype|acro|acrodata|acrodef|acrolist|
                       action|altpnr|art|asschgdrv|author|cb|cbdata|cbname|cbpanname|
                       chap-toc|chgcond|chgdesc|chgdriver|chgnbr|chgreason|chgtyp|cmm-fmatr|
                       coceff|comploc|compname|def|defdata|deflist|deleted|
                       effect|ftnote|fullterm|gdesc|illus|ipcseq|isempty|
                       listofchap|manrev|mlep|msc|mtoc|number|numlist|numlitem|
                       pan|panaccdata|pandata|panname|part|partname|partqty|pcd|
                       permrec|recofrev|ref|refext|refint|refname|refno|result|rp|sbeff|sbs|
                       sd|sdes|sectname|shtno|tbl-cb|tbl-comploc|tbl-panel|tbl-part|tbl-ref|
                       term|toc-auto|toc-man|transltr|trindex|ttlpage|unitid|usercomm|xmit|zone">
    <xsl:choose>
      <xsl:when test="text()">
        <fo:inline color="red">
          <xsl:apply-templates/>
        </fo:inline>
      </xsl:when>
      <xsl:otherwise>
        <fo:block color="red">
          <xsl:apply-templates/>
        </fo:block>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

 
</xsl:stylesheet>
