<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
 xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
 xmlns:rss="http://purl.org/rss/1.0/"
 xmlns:dc="http://purl.org/dc/elements/1.1/"
 xmlns:syn="http://purl.org/rss/1.0/modules/syndication/"
 xmlns:taxo="http://purl.org/rss/1.0/modules/taxonomy/"
 xmlns:html="http://www.w3.org/1999/xhtml"
                version="1.0">

<xsl:template match="gi">
  <code><xsl:text>&lt;</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>&gt;</xsl:text></code>
</xsl:template>

<xsl:template match="Menu">
  <code>
    <xsl:if test="$cssFile">
      <xsl:attribute name="class">Menu</xsl:attribute>
    </xsl:if>
<xsl:text>[</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>]</xsl:text></code>
</xsl:template>

<xsl:template match="Command">
<code>
    <xsl:if test="$cssFile">
      <xsl:attribute name="class">Command</xsl:attribute>
    </xsl:if>
  <xsl:apply-templates/>
</code>
</xsl:template>

<xsl:template match="Key">
  <code>
    <xsl:if test="$cssFile">
      <xsl:attribute name="class">Key</xsl:attribute>
    </xsl:if>
&lt;<xsl:apply-templates/>&gt;</code>
</xsl:template>

<xsl:template match="Code">
  <code>
    <xsl:if test="$cssFile">
      <xsl:attribute name="class">Code</xsl:attribute>
    </xsl:if>
  <xsl:apply-templates/>
  </code>
</xsl:template>

<xsl:template match="Input">
  <code>
    <xsl:if test="$cssFile">
      <xsl:attribute name="class">Input</xsl:attribute>
    </xsl:if>
  <xsl:apply-templates/>
  </code>
</xsl:template>

<xsl:template match="Output">
  <code>
    <xsl:if test="$cssFile">
      <xsl:attribute name="class">Output</xsl:attribute>
    </xsl:if>
  <xsl:apply-templates/>
  </code>
</xsl:template>

<xsl:template match="Screen">
  <pre>
    <xsl:if test="$cssFile">
      <xsl:attribute name="class">Screen</xsl:attribute>
    </xsl:if>
  <xsl:apply-templates/>
  </pre>
</xsl:template>

<xsl:template match="Program">
  <pre>
    <xsl:if test="$cssFile">
      <xsl:attribute name="class">Program</xsl:attribute>
    </xsl:if>
  <xsl:apply-templates/>
  </pre>
</xsl:template>

<xsl:template match="Prompt">
  <code>
    <xsl:if test="$cssFile">
      <xsl:attribute name="class">Prompt</xsl:attribute>
    </xsl:if>
  <xsl:apply-templates/>
  </code>
</xsl:template>

<xsl:template match="Field">
  <code>
    <xsl:if test="$cssFile">
      <xsl:attribute name="class">Field</xsl:attribute>
    </xsl:if>
  <xsl:apply-templates/>
  </code>
</xsl:template>

<xsl:template match="Link">
  <code>
    <xsl:if test="$cssFile">
      <xsl:attribute name="class">Link</xsl:attribute>
    </xsl:if>
  <xsl:apply-templates/>
  </code>
</xsl:template>

<xsl:template match="Filespec">
  <code>
    <xsl:if test="$cssFile">
     <xsl:attribute name="class">Filespec</xsl:attribute>
</xsl:if>
  <xsl:apply-templates/>
  </code>
</xsl:template>

<xsl:template match="Button">
  <code>
    <xsl:if test="$cssFile">
      <xsl:attribute name="class">Button</xsl:attribute>
    </xsl:if>
  <xsl:apply-templates/>
  </code>
</xsl:template>

<xsl:template match="Value">
  <code>
    <xsl:if test="$cssFile">
      <xsl:attribute name="class">Value</xsl:attribute>
    </xsl:if>
  <xsl:apply-templates/>
  </code>
</xsl:template>

<xsl:template match="Keyword">
  <code>
    <xsl:if test="$cssFile">
            <xsl:attribute name="class">Keyword</xsl:attribute>
    </xsl:if>
  <xsl:apply-templates/>
  </code>
</xsl:template>

<!-- special insertions -->
<xsl:template match="xptr[@type='transclude' and @rend='rss']">
  <xsl:for-each select="document(@url)/rdf:RDF">  
  <xsl:for-each select="rss:item[position() &lt; 11]">
     <p class="rss"><a href="{normalize-space(rss:link)}">
     <xsl:apply-templates select="rss:title"/></a>
     <xsl:if test="dc:date">
      [<xsl:apply-templates select="dc:date"/>]
     </xsl:if>
      <br/>
     <xsl:apply-templates select="rss:description"/> </p>
   </xsl:for-each>
</xsl:for-each>
</xsl:template>

<xsl:template match="xptr[@type='transclude' and @rend='rssfirst']">
  <xsl:for-each select="document(@url)/rdf:RDF">  
  <xsl:for-each select="rss:item[position() = 1]">
     <p class="rss"><a href="{normalize-space(rss:link)}">
     <xsl:apply-templates select="rss:title"/></a>
      <br/>
     <xsl:apply-templates select="rss:description"/> </p>
   </xsl:for-each>
</xsl:for-each>

</xsl:template>

<xsl:template match="xref[@type='transclude' and @rend='rss']">
  <xsl:for-each select="document(@url)/rdf:RDF">  
  <xsl:for-each select="rss:item">
     <p class="rss"><a href="{normalize-space(rss:link)}">
     <xsl:apply-templates select="rss:title"/></a></p>
     <p class="rss"><xsl:apply-templates select="rss:description"/></p>
   </xsl:for-each>
 <p class="small">
   <xsl:text> (last updated </xsl:text><xsl:value-of select="rss:channel/dc:date"/>)
 </p>
</xsl:for-each>
</xsl:template>

<xsl:template match="processing-instruction()[name(.)='html']">
  <xsl:value-of disable-output-escaping="yes" select="."/>
</xsl:template>

<xsl:template match="html:*">
     <xsl:element name="{local-name()}">
       <xsl:copy-of select="@*"/>
       <xsl:apply-templates/>
     </xsl:element>
</xsl:template>

<xsl:template match="formerrors">
    <xsl:apply-templates select="..//error"/>
</xsl:template>

<xsl:template match="error">
  <br/>
   <span class="form_error"><xsl:value-of select="."/></span>
</xsl:template>

<xsl:template match="textfield">
    <input 
        type="text"
        name="{@name|name}" 
        value="{@value|value}" 
        size="{@width|width}" 
        maxlength="{@maxlength|maxlength}" />
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="password">
    <input 
        type="password"
        name="{@name|name}" 
        value="{@value|value}" 
        size="{@width|width}" 
        maxlength="{@maxlength|maxlength}" />
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="checkbox">
    <input
        type="checkbox"
        name="{@name|name}"
        value="{@value|value}" >
      <xsl:if test="@checked|checked">
        <xsl:attribute name="checked">
	  <xsl:text>@checked|checked</xsl:text>
	</xsl:attribute>
      </xsl:if>
    </input>
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="submit_button">
    <input
        type="submit"
        name="{@name|name}"
        value="{@value|value}" />
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="hidden">
    <input
        type="hidden"
        name="{@name|name}"
        value="{@value|value}" />
</xsl:template>

<xsl:template match="options/option">
  <option value="{@value|value}">
    <xsl:if test="selected[. = 'selected'] | @selected[. = 'selected']">
      <xsl:attribute name="selected">selected</xsl:attribute>
    </xsl:if>
    <xsl:value-of select="@text|text"/>
  </option>
</xsl:template>

<xsl:template match="single_select">
    <select name="{@name|name}">
    <xsl:if test="@size">
      <xsl:attribute name="size"><xsl:value-of select="@size"/></xsl:attribute>
    </xsl:if>
        <xsl:apply-templates select="options/option"/>
    </select>
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="multi_select">
    <select multiple="multiple" size="5" name="{@name|name}">
    <xsl:if test="@size">
      <xsl:attribute name="size"><xsl:value-of select="@size"/></xsl:attribute>
    </xsl:if>
        <xsl:apply-templates select="options/option"/>
    </select>
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="textarea">
    <textarea name="{@name|name}" cols="{@cols|cols}" rows="{@rows|rows}">
    <xsl:if test="@wrap|wrap"><xsl:attribute name="wrap">physical</xsl:attribute></xsl:if>
    <xsl:value-of select="@value|value"/>
    </textarea> <br />
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="form">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>


<xsl:template match="xptr[@url and not(@type='transclude')]">
   <xsl:element name="{$fontURL}">
     <a class="xptr" href="{@url}">
     <xsl:call-template name="urltargets"/>
 <xsl:choose>
  <xsl:when test="starts-with(@url,'mailto:')">
     <xsl:value-of select="substring-after(@url,'mailto:')"/>
  </xsl:when>
  <xsl:when test="starts-with(@url,'http://')">
     <xsl:value-of select="substring-after(@url,'http://')"/>
  </xsl:when>
  <xsl:otherwise>
     <xsl:value-of select="@url"/>
  </xsl:otherwise>
  </xsl:choose>
  </a>
 </xsl:element>   
</xsl:template>


</xsl:stylesheet>
