<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xs="http://www.w3.org/2001/XMLSchema"
                version='1.1'>
  <!-- Run with command line: -->
  <!-- xsltproc -o gg.xml gui-items-documentor.xsl guiItems.xsd -->

  <xsl:output method="xml"/>
  <xsl:strip-space elements="*"/>


  <!-- Selects all the ui Items and shows them it two modes:

       - as table
       - as reference

       They are disticted because they are the only top-level
       xs:element with @name.
       
       In future it is better to make selection via
       a hint-attribute doc="uiItem" and select only the 
       xs:elements with this doc attribute value. -->

  <xsl:template match="xs:schema">
  <book id="gui-item-reference-guide">
    <xsl:processing-instruction name="dbhtml">
      <xsl:text>filename="index.html" dir="gui_items_guide"</xsl:text>
    </xsl:processing-instruction>
    <title>Serna GUI Item Reference Guide</title>
    <chapter id="gui-item-table">
      <xsl:processing-instruction name="dbhtml">
        <xsl:text>filename="gui-item-table.html"</xsl:text>
      </xsl:processing-instruction>
      <title>GUI Item Table</title>
      <informaltable frame="all">
        <tgroup cols="4" colsep="1">
          <colspec colnum="1" colname="c1" colwidth="1.5*"/>
          <colspec colnum="2" colname="c2" colwidth="3*"/>
          <colspec colnum="3" colname="c3" colwidth="2*"/>
          <colspec colnum="4" colname="c4" colwidth="2*"/>
          <thead>
            <row>
              <entry align="center">Item Name</entry>
              <entry align="center">Description</entry>
              <entry align="center">Properties</entry>
              <entry align="center">Child Items</entry>
            </row>
          </thead>
          <tbody>
            <xsl:apply-templates select="xs:element[@name]" mode="uiItem">
              <xsl:sort select="@name"/>
            </xsl:apply-templates>
          </tbody>
        </tgroup>
      </informaltable>
    </chapter>
    <chapter id="gui-item-detailed-desc">
      <xsl:processing-instruction name="dbhtml">
        <xsl:text>filename="gui-item-detailed-desc.html"</xsl:text>
      </xsl:processing-instruction>
      <title>GUI Items Detailed Description</title>
      <xsl:apply-templates select="xs:element[@name]" mode="ref">
        <xsl:sort select="@name"/>
      </xsl:apply-templates>
    </chapter>
  </book>
  </xsl:template>


  <!-- Brief uiItem documentation (in table) -->
  <xsl:template match="xs:element" mode="uiItem">
    <row>
      <entry>
        <link>
          <xsl:attribute name="linkend">
            <xsl:text>ui-item-</xsl:text>
            <xsl:value-of select="@name"/>
          </xsl:attribute>
          <xsl:value-of select="@name"/>
        </link>
      </entry>
      <entry>
        <xsl:value-of select="xs:annotation/xs:documentation[1]"/>
      </entry>
      <entry>
        <xsl:call-template name="property-list"/>
      </entry>
      <entry>
        <xsl:call-template name="children-list"/>
      </entry>
    </row>
  </xsl:template>


  <!-- Full uiItem documentation (as reference section) -->
  <xsl:template match="xs:element" mode="ref">
    <refentry>
      <xsl:attribute name="id">
        <xsl:text>refentry-ui-item-</xsl:text>
        <xsl:value-of select="@name"/>
      </xsl:attribute>
      <refnamediv>
        <refname><xsl:value-of select="@name"/></refname>
        <refpurpose>
          <anchor>
            <xsl:attribute name="id">
              <xsl:text>ui-item-</xsl:text>
              <xsl:value-of select="@name"/>
            </xsl:attribute>
          </anchor>
          <xsl:copy-of select="xs:annotation/xs:documentation[1]/node()"/>
        </refpurpose>
      </refnamediv>
      <refsect1>
        <xsl:attribute name="id">
          <xsl:text>desc-ui-item-</xsl:text>
          <xsl:value-of select="@name"/>
        </xsl:attribute>
        <title>Description</title>
        <xsl:copy-of select="xs:annotation/xs:documentation[2]/node()"/>
        <refsect2>
          <xsl:attribute name="id">
            <xsl:text>properties-ui-item-</xsl:text>
            <xsl:value-of select="@name"/>
          </xsl:attribute>
          <title>Properties</title>
          <xsl:call-template name="property-list">
            <xsl:with-param name="description" select="'full'"/>
            <xsl:with-param name="item-name" select="@name"/>
          </xsl:call-template>
        </refsect2>
        <refsect2>
          <xsl:attribute name="id">
            <xsl:text>included-in-ui-item-</xsl:text>
            <xsl:value-of select="@name"/>
          </xsl:attribute>
          <title>Included Items</title>
          <xsl:call-template name="children-list"/>
        </refsect2>
      </refsect1>
    </refentry>
  </xsl:template>


  <!-- Collect the properties of the uiItem as nodesets and pass them
       to the template that buidls the <itemizedlist> of the
       properties (propertyType mode template) -->

  <xsl:template name="property-list">
    <xsl:param name="description" select="'brief'"/>
    <xsl:param name="item-name" select="'undefined'"/>
    <xsl:choose>

      <!-- In this case we select properties if they are declared
           in external complexType, referred by @type -->
      <xsl:when test="@type">
        <xsl:variable name="itemType" 
          select="string(@type)"/>
        <xsl:variable name="propertyType" 
          select="//xs:complexType[@name=$itemType]//xs:element[@name='properties']/@type"/>
        
        <xsl:apply-templates 
          select="//xs:complexType[@name=$propertyType]" 
          mode="propertyType">
          <xsl:with-param name="description" select="$description"/>
          <xsl:with-param name="item-name" select="@name"/>
        </xsl:apply-templates>
      </xsl:when>

      <!-- In this case we select properties if they are declared
           in internal complexType -->
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test=".//xs:element[@name='properties']">
            <xsl:variable name="propertyType" 
              select=".//xs:element[@name='properties']/@type"/>
            <xsl:apply-templates 
              select="//xs:complexType[@name=$propertyType]" 
              mode="propertyType">
              <xsl:with-param name="description" select="$description"/>
              <xsl:with-param name="item-name" select="@name"/>
            </xsl:apply-templates>
          </xsl:when>
          <xsl:otherwise>
            <para><xsl:text>None</xsl:text></para>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Collect all child uiItems as nodesets and pass them to 
       the template that builds the <itemizedlist> of the
       children (itemTypeChildren mode template) -->

  <xsl:template name="children-list">
      <xsl:choose>
        
        <!-- Here we collect children of uiItem, if uiItem refers to
             its type via @type, and does not declare children within. -->
        <xsl:when test="@type">
          <xsl:variable name="itemType" 
            select="@type"/>
          <xsl:apply-templates 
            select="//xs:complexType[@name=$itemType]" 
            mode="itemTypeChildren"/>
        </xsl:when>

        <!-- Here we collect children of uiItem, if uiItem declares
             children within itself. -->
        <xsl:otherwise>
          <xsl:choose>
            <xsl:when test=".//xs:element[@ref]">
              <xsl:apply-templates 
                select=".//xs:complexType" 
                mode="itemTypeChildren"/>
            </xsl:when>
            <xsl:otherwise>
              <para><xsl:text>None</xsl:text></para>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:otherwise>
      </xsl:choose>
  </xsl:template>


  <!-- Builds child uiItems <itemizedlist> of a uiItem -->

  <xsl:template match="xs:complexType" mode="itemTypeChildren">
    <xsl:variable name="grp" 
      select=".//xs:group/@ref"/>
    <xsl:variable name="children"
      select="//xs:group[@name=$grp]//xs:element|.//xs:element[@ref]"/>
    <xsl:choose>
      <xsl:when test="count($children)">
        <itemizedlist>
          <xsl:for-each select="$children">
            <xsl:sort select="@ref"/>
            <xsl:apply-templates select="." mode="childName"/>
          </xsl:for-each>
        </itemizedlist>
      </xsl:when>
      <xsl:otherwise>
        <para><xsl:text>None</xsl:text></para>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Called from the itemTypeChildren mode template for
       documenting a child uiItem -->
  <xsl:template match="xs:element" mode="childName">
    <listitem>
      <para>
        <link>
          <xsl:attribute name="linkend">
            <xsl:text>ui-item-</xsl:text>
            <xsl:value-of select="@ref"/>
          </xsl:attribute>
          <xsl:value-of select="@ref"/>
        </link>
      </para>
    </listitem>
  </xsl:template>


  <!-- Builds the <itemizedlist> of properties of uiItem:
       
       From the current property type of the uiItem.
       From the base class of the property type.
       
       -->
  <xsl:template match="xs:complexType" mode="propertyType">
    <xsl:param name="description" select="'brief'"/>
    <xsl:param name="item-name" select="'undefined'"/>
    <xsl:variable name="ext" 
      select=".//xs:extension/@base"/>
    <xsl:variable name="properties" 
      select=".//xs:element[not(ancestor::xs:element)]|
              //xs:complexType[@name=$ext]//xs:element[not(ancestor::xs:element)]"/>
    <itemizedlist>
      <xsl:for-each select="$properties">
        <xsl:sort select="@name"/>
        <xsl:apply-templates select="." mode="propertyName">
          <xsl:with-param name="description" select="$description"/>
          <xsl:with-param name="item-name" select="$item-name"/>
        </xsl:apply-templates>
      </xsl:for-each>
    </itemizedlist>
  </xsl:template>

  <!-- Called from the propertyType template for
       documenting a single property -->
  <xsl:template match="xs:element" mode="propertyName">
    <xsl:param name="description" select="'brief'"/>
    <xsl:param name="item-name" select="'undefined'"/>
    <xsl:variable name="depth" select="count(ancestor::xs:element)"/>
    <xsl:variable name="id">
      <xsl:value-of select="$item-name"/>
      <xsl:text>-</xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>-</xsl:text>
      <xsl:value-of select="$depth"/>
    </xsl:variable>
    <listitem>
      <xsl:choose>
        <xsl:when test="$description = 'brief'">
          <para>
            <link>
              <xsl:attribute name="linkend">
                <xsl:value-of select="$id"/>
              </xsl:attribute>
              <xsl:value-of select="@name"/>
            </link>
          </para>
        </xsl:when>
        <xsl:otherwise>
          <formalpara>
            <title>
              <xsl:value-of select="@name"/>
              <anchor>
                <xsl:attribute name="id">
                  <xsl:value-of select="$id"/>
                </xsl:attribute>
              </anchor>
            </title>
            <para><xsl:value-of select="xs:annotation/xs:documentation[1]"/></para>
          </formalpara>
        </xsl:otherwise>
      </xsl:choose>

      <xsl:if test=".//xs:element">
        <itemizedlist>
          <xsl:for-each select=".//xs:element[(count(ancestor::xs:element) - $depth) = 1]">
            <xsl:sort select="@name"/>
            <xsl:apply-templates select="." mode="propertyName">
              <xsl:with-param name="description" select="$description"/>
              <xsl:with-param name="item-name" select="$item-name"/>
            </xsl:apply-templates>
          </xsl:for-each>
        </itemizedlist>
      </xsl:if>
    </listitem>
  </xsl:template>

</xsl:stylesheet>
