<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.1'>

<xsl:output method="text" encoding="utf-8"/>

<xsl:include href="param.xsl"/>

<xsl:template match="/">
/// THIS FILE IS GENERATED AUTOMATICALLY FROM serna-ui.xml. DO NOT EDIT.

#include "<xsl:value-of select="local-name(/child::*[1])"/>Actions.hpp"
#include "<xsl:value-of select="local-name(/child::*[1])"/>EventDecls.hpp"
#include "ui/UiItems.h"
#include "common/CommandEvent.h"
#include "common/PropertyTree.h"
#include "core/DocBuilders.h"
#include "docview/EventTranslator.h"

#include &lt;iostream&gt;

using namespace Common;
using namespace Sui;

void <xsl:value-of select="local-name(/child::*[1])"/>Builder::buildActions(Sui::ActionDispatcher* dispatcher, Sui::ActionSet* actionSet)
{
    <!-- Load builtin actions -->

    loadSui();
    makeActions(actionSet);
    
    <!-- Register loaded actions -->

    EventTranslator&amp; eventTranslator = 
        *dynamic_cast&lt;EventTranslator*>(dispatcher);
    <xsl:value-of select="local-name(/child::*[1])"/>Actions* builtinActions =
        new <xsl:value-of select="local-name(/child::*[1])"/>Actions;
    eventTranslator.setUiActions(builtinActions);
        
    <xsl:apply-templates select="*"/>
}   

PropertyNode* <xsl:value-of select="local-name(/child::*[1])"/>Builder::loadSui()
{
    load_sui("<xsl:value-of select='local-name(/child::*[1])'/>");
    return suiProps_.pointer();
}   
</xsl:template>

<!-- Registering ui actions -->

<xsl:template match="uiActions">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="uiAction">
  <xsl:if test="not(contains(@options, $eopt))">
    set_action(actionSet, builtinActions-><xsl:value-of select="name"/>_,
        "<xsl:value-of select="name"/>");
    <xsl:if test="commandEvent">
        <xsl:text>
    eventTranslator.registerEvent(builtinActions-></xsl:text>
        <xsl:value-of select="name"/>
        <xsl:text>(), 
                                  makeCommand&lt;</xsl:text>
        <xsl:value-of select="commandEvent"/>
        <xsl:text>>);
        </xsl:text>
    </xsl:if>
  </xsl:if>
</xsl:template>


<xsl:template match="actionGroups">
    <xsl:value-of select="local-name(/child::*[1])"/>ActionGroups* 
    actionGroups =
        new <xsl:value-of select="local-name(/child::*[1])"/>ActionGroups;
    eventTranslator.setActionGroups(actionGroups);
    <xsl:apply-templates mode="group"/>
</xsl:template>

<xsl:template match="action" mode="group">
    actionGroups-><xsl:value-of select="local-name(ancestor::*[1])"/>_.addAction(builtinActions-><xsl:value-of select="."/>());
</xsl:template>


<xsl:template match="text()"/>
                
</xsl:stylesheet>
