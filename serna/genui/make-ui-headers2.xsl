<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<xsl:output method="text"/>

<xsl:template match="/">
/// THIS FILE IS GENERATED AUTOMATICALLY FROM serna-ui.xml. DO NOT EDIT.

#include "ui/UiAction.h"
#include "ui/UiActionGroup.h"

#include "docview/BuiltinUiCommands.h"

    <xsl:apply-templates select="*"/>
</xsl:template>

<xsl:template match="uiActions">
  class <xsl:value-of select="local-name(/child::*[1])"/>Actions : public BuiltinUiActions {
    <xsl:apply-templates/>
    friend class <xsl:value-of select="local-name(/child::*[1])"/>; 
    friend class <xsl:value-of select="local-name(/child::*[1])"/>Builder; 
};
</xsl:template>

<xsl:template match="uiAction">
public:
    Sui::Action* <xsl:value-of select="name"/>() const { return <xsl:value-of select="name"/>_.pointer(); }
private:
    Sui::ActionPtr <xsl:value-of select="name"/>_;
</xsl:template>

<!-- ///////////////////////////////////////////////////////////// -->

<xsl:template match="actionGroups">
class <xsl:value-of select="local-name(/child::*[1])"/>ActionGroups : public BuiltinActionGroups {
    <xsl:apply-templates select="*" mode="action_group"/>
    friend class <xsl:value-of select="local-name(/child::*[1])"/>; 
    friend class <xsl:value-of select="local-name(/child::*[1])"/>Builder; 
};
</xsl:template>

<xsl:template match="*" mode="action_group">
public:
    const Sui::ActionGroup&amp; <xsl:value-of select="local-name()"/>() const { return <xsl:value-of select="local-name()"/>_; } 
private:
    Sui::ActionGroup <xsl:value-of select="local-name()"/>_; 
</xsl:template>

<!-- ///////////////////////////////////////////////////////////// -->

<xsl:template match="text()"/>

</xsl:stylesheet>
