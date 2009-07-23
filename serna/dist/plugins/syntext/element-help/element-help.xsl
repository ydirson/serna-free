<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                 xmlns:fo="http://www.w3.org/1999/XSL/Format"
                 xmlns:se="http://syntext.com/XSL/Format-1.0"
                 xmlns:h="http://www.syntext.com/Extensions/ElementHelp-1.0" version="1.0">

    <xsl:import href="../../../xml/stylesheets/xslbricks/fo/fo.xsl"/>
    <xsl:import href="../../xhtml/xhtml-xsl-serna/xhtml.xsl"/>

    <xsl:strip-space elements="*"/>

    <xsl:param name="show-attributes" select="'edit'" xse:type="string" xse:annotation="One of 'yes', 'edit', 'no'"/>
    <xsl:param name="use-serna" select="contains(system-property('xsl:vendor'), 'Syntext')"/>

<!-- -->
    <xsl:param name="body.font.size" select="'14'"/>

    <xsl:param name="page.margin.top" select="'0.1cm'"/>
    <xsl:param name="page.margin.left" select="'1cm'"/>
    <xsl:param name="page.margin.right" select="'1cm'"/>
    <xsl:param name="page.margin.bottom" select="'0.1cm'"/>

    <xsl:param name="page.padding.left" select="'1cm'"/>
    <xsl:param name="page.list.padding.left" select="'7.5cm'"/>

    <xsl:param name="page.font.family" select="'Courier'"/>
    <xsl:param name="page.font.size" select="'14'"/>

    <xsl:param name="page.text.padding" select="'0.3cm'"/>
    <xsl:param name="page.head.block.bgcolor" select="'#a7c4e1'"/>

    <xsl:param name="displ.insert_attr" select="' Insert attribute to this element'"/>



<!-- starting here -->  
    <xsl:template match="/h:serna-help">
        <xsl:call-template name="adp-file-name"/>
    
        <xsl:apply-templates/>
    </xsl:template>
    
    
    <!-- Parse two required elements -->
    <xsl:template match="h:element-help">
        <fo:block background-color="#e0e0e0" border-top-width="5pt">
            <fo:block>
                <xsl:call-template name="element-help-name"/>
            </fo:block>
            <fo:block start-indent="{$page.padding.left}" background-color="#dbf2e8" border-bottom-width="5pt" border-top-width="5pt">
                <xsl:apply-templates/>
            </fo:block>
        </fo:block>
    </xsl:template>
    
    <xsl:template match="h:attr-group">
        <fo:block background-color="#e0e0e0" border-top-width="5pt">
            <fo:block>
                <xsl:call-template name="attr-group-name"/>
            </fo:block>
            <fo:block start-indent="{$page.padding.left}" background-color="#e5f4ed" border-bottom-width="15pt">
                <xsl:apply-templates/>
            </fo:block>
        </fo:block>
    </xsl:template>
    

<!-- edit function -->
    <xsl:template match="@adp-file|@match|@name|@href" mode="edit">
        <xsl:call-template name="readInput">
            <xsl:with-param name="item" select="current()"/>
        </xsl:call-template>
    </xsl:template>


<!-- misc function -->
    <xsl:template name="showMessage">
        <xsl:param name="message"/>

        <fo:inline color="red" font-family="{$page.font.family}"
                                font-size="{$page.font.size}">
            <xsl:value-of select="$message"/>
        </fo:inline>
    </xsl:template>

    <xsl:template name="showText">
        <xsl:param name="text"/>
    
        <fo:inline font-family="{$page.font.family}"
                    padding-left="{$page.text.padding}"
                    font-size="{$page.font.size}">
            <xsl:value-of select="$text"/>
        </fo:inline>
    </xsl:template>

    <xsl:template name="readInput">
        <xsl:param name="item"/>
        <xsl:variable name="item_val" select="$item"/>

        <fo:inline font-style="italic" color="gray">
            <se:line-edit value="{$item_val}" width="11.45cm"/>
        </fo:inline>
    </xsl:template>



    <xsl:template name="adp-file-name">
        <fo:block background-color="#f5f4b1">

            <fo:list-block provisional-label-separation="0cm"
                           provisional-distance-between-starts="{$page.list.padding.left}">
                <fo:list-item>
                    <fo:list-item-label end-indent="label-end() - 1cm">
                        <xsl:call-template name="showText">
                            <xsl:with-param name="text" select="'adp-file (double click to
                                                                browse): '"/> 
                        </xsl:call-template>
                    </fo:list-item-label>
            
                    <fo:list-item-body start-indent="body-start()">
                        <fo:inline font-style="italic">
                            <xsl:choose>
                                <xsl:when test="@adp-file">
                                    <xsl:apply-templates select="@adp-file" mode="edit"/>
                                </xsl:when>
                                <xsl:otherwise>
                                    <xsl:text> </xsl:text>
                                </xsl:otherwise>
                            </xsl:choose>
                        </fo:inline>
                    </fo:list-item-body>
                </fo:list-item>
            </fo:list-block>

        </fo:block>
    </xsl:template>
    
    <xsl:template name="element-help-name">
        <fo:list-block background-color="#cac8c8"                     
                       provisional-label-separation="0.2em"
                       provisional-distance-between-starts="{$page.list.padding.left}">
            <fo:list-item>
                <fo:list-item-label end-indent="label-end()">
                    <xsl:call-template name="showText">
                        <xsl:with-param name="text" select="'Help to element: '"/>
                    </xsl:call-template>
                </fo:list-item-label>

                <fo:list-item-body start-indent="body-start()">
                    <xsl:apply-templates select="@match" mode="edit"/>
                </fo:list-item-body>
          </fo:list-item>
        </fo:list-block>
    </xsl:template>
    
    <xsl:template name="attr-group-name">
        <fo:block background-color="#cac8c8">
            <fo:list-block provisional-label-separation="0.2em"
                           provisional-distance-between-starts="{$page.list.padding.left}">
                <fo:list-item>
                    <fo:list-item-label end-indent="label-end()">
                        <xsl:call-template name="showText">
                            <xsl:with-param name="text" select="'Attribute group name: '"/>
                        </xsl:call-template>
                    </fo:list-item-label>
            
                    <fo:list-item-body start-indent="body-start()">
                        <xsl:apply-templates select="@name" mode="edit"/>
                    </fo:list-item-body>
                </fo:list-item>
            </fo:list-block>
        </fo:block>
    </xsl:template>
    
    <xsl:template match="h:short-help">
            <fo:block border-bottom-width="10pt">
                <fo:block background-color="{$page.head.block.bgcolor}">
                    <xsl:call-template name="showText">
                        <xsl:with-param name="text" select="'Short help (HTML tags allowed)'"/>
                    </xsl:call-template>
                </fo:block>
                <fo:block>
                    <xsl:choose>
                        <xsl:when test="not(text()) and not(node())">
                            <xsl:text> </xsl:text>
                        </xsl:when>
                        <xsl:otherwise>
                            <fo:inline padding-left="{$page.text.padding}">
                                <xsl:apply-templates/>
                            </fo:inline>
                        </xsl:otherwise>
                    </xsl:choose>
                </fo:block>
            </fo:block>
    </xsl:template>
    
    <xsl:template match="h:qta-help">
        <fo:block border-bottom-width="10pt">
            <fo:block background-color="{$page.head.block.bgcolor}">
                <xsl:call-template name="showText">
                    <xsl:with-param name="text" select="'QTA-help 
                                                (may contain only attributes)'"/>
                </xsl:call-template>
            </fo:block>

            <xsl:choose>
                <xsl:when test="not(@adp-file) and not(@href)">
                    <xsl:text> </xsl:text>
                </xsl:when>
                <xsl:otherwise>
                    <fo:list-block provisional-label-separation="0.2em"
                                   provisional-distance-between-starts="{$page.list.padding.left} - 2cm">
                        <xsl:if test="@adp-file">
                            <fo:list-item>
                                <fo:list-item-label end-indent="label-end()">
                                    <xsl:call-template name="showText">
                                        <xsl:with-param name="text" select="'adp-file: '"/>
                                    </xsl:call-template>
                                </fo:list-item-label> 
                                <fo:list-item-body start-indent="body-start()">
                                    <xsl:apply-templates select="@adp-file" mode="edit"/>
                                </fo:list-item-body>
                            </fo:list-item>
                        </xsl:if>
                        <xsl:if test="@href">
                            <fo:list-item>
                                <fo:list-item-label end-indent="label-end()">
                                    <xsl:call-template name="showText">
                                        <xsl:with-param name="text" select="'href: '"/>
                                    </xsl:call-template>
                                </fo:list-item-label>
                                <fo:list-item-body start-indent="body-start()">
                                    <xsl:apply-templates select="@href" mode="edit"/>
                                </fo:list-item-body>
                            </fo:list-item>
                        </xsl:if>
                    </fo:list-block>
                </xsl:otherwise>
            </xsl:choose>

        </fo:block>
    </xsl:template>
    
    <xsl:template match="h:attr-help">
        <fo:block background-color="{$page.head.block.bgcolor}">
            <fo:list-block provisional-label-separation="0.2em"
                           provisional-distance-between-starts="{$page.list.padding.left} - 1cm">
                <fo:list-item>
                    <fo:list-item-label end-indent="label-end()">
                        <xsl:call-template name="showText">
                            <xsl:with-param name="text" select="'Help to attribute: '"/>
                        </xsl:call-template>
                    </fo:list-item-label>

                    <fo:list-item-body start-indent="body-start()">
                        <xsl:apply-templates select="@name" mode="edit"/>
                    </fo:list-item-body>
                </fo:list-item>
            </fo:list-block>
        </fo:block>
        <fo:block start-indent="{$page.padding.left}*2" border-bottom-width="5pt"
                                                        border-top-widt="5pt">
            <xsl:apply-templates select="h:short-help"/>
            <xsl:apply-templates select="h:qta-help"/>
        </fo:block>
    </xsl:template>
    

    <xsl:template match="h:attr-group-ref">
        <fo:list-block background-color="#8dc0cb"
                       provisional-label-separation="0.2em"
                       provisional-distance-between-starts="{$page.list.padding.left} - 1cm">
            <fo:list-item>
                <fo:list-item-label end-indent="label-end()">
                    <xsl:call-template name="showText">
                        <xsl:with-param name="text" select="'Attribute group reference: '"/>
                    </xsl:call-template>
               </fo:list-item-label>

               <fo:list-item-body start-indent="body-start()">
                   <xsl:choose>
                        <xsl:when test="@name">
                            <xsl:apply-templates select="@name" mode="edit"/>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:call-template name="showMessage">
                                <xsl:with-param name="message" select="$displ.insert_attr"/>
                            </xsl:call-template>
                        </xsl:otherwise>
                   </xsl:choose>
               </fo:list-item-body>

            </fo:list-item>
        </fo:list-block>
    </xsl:template>


<!-- Processing other elements -->
    <xsl:template match="table/tr/td|tbody/tr/td">
        <xsl:apply-imports/>
    </xsl:template>

    <xsl:template match="td">
        <xsl:apply-templates/>
    </xsl:template>
    
    <xsl:template match="dl">
        <xsl:apply-imports/>
    </xsl:template>

    <xsl:template match="dl/dt">
        <fo:inline color="red">
            <xsl:apply-imports/>
        </fo:inline>
    </xsl:template>

    <xsl:template match="dl/dd">
        <fo:block start-indent="{$page.padding.left}">
            <!--
                TODO: 
                        waiting when will fixed the bug with strange function
                        Really the following must seems as: <xsl:apply-imports/>
            -->
            <xsl:apply-templates/>
        </fo:block>
    </xsl:template>
    
    <xsl:template match="*">
        <xsl:apply-imports/>
    </xsl:template>
    
    
</xsl:stylesheet>
