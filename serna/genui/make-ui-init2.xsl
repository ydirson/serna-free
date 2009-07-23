<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<xsl:include href="param.xsl"/>

<xsl:output method="text" encoding="utf-8"/>

<xsl:template match="/">
/// THIS FILE IS GENERATED AUTOMATICALLY FROM serna-ui.xml. DO NOT EDIT.

#include "<xsl:value-of select="local-name(/child::*[1])"/>Actions.hpp"
#include "<xsl:value-of select="local-name(/child::*[1])"/>EventDecls.hpp"
#include "ui/UiItems.h"
#include "common/CommandEvent.h"
#include "common/PropertyTree.h"
#include "core/DocBuilders.h"
#include "docview/EventTranslator.h"
#include "proputils/PropertyTreeSaver.h"

#include "qapplication.h"
#include "qmessagebox.h"

#include &lt;iostream&gt;

using namespace Common;
using namespace Sui;

void <xsl:value-of select="local-name(/child::*[1])"/>Builder::buildActions(Sui::ActionDispatcher* dispatcher, Sui::ActionSet* actionSet) const 
{
    <!-- Load builtin actions -->

    String name = "<xsl:value-of select="local-name(/child::*[1])"/>";
    PathName path(config().getDataDir());
    path.append("ui").append(name + ".sui");
    PropertyNodePtr prop = new PropertyNode(name);
    PropUtils::PropertyTreeSaver loader(prop.pointer(), prop->name());
    if (!loader.readPropertyTree(path.name())) {
        QMessageBox::critical(qApp->activeWindow(), 
                              "Error Loading UI Description", 
                              loader.errmsg());
        exit(0);
    }
    PropertyNode* action_list = prop->getProperty("/properties/uiActions");
    if (!action_list) {
        QMessageBox::critical(qApp->activeWindow(), 
                              "Invalid UI Description", 
                              "Builtin UI actions are not defined");
        exit(0);
    }
    for (PropertyNode* c = action_list-&gt;firstChild(); 
         c; c = c->nextSibling()) {
        if ("uiAction" != c->name())
            continue;
        actionSet->makeAction(c);
    }

    <!-- Register loaded actions -->

    EventTranslator&amp; eventTranslator = 
        *dynamic_cast&lt;EventTranslator*>(dispatcher);
    <xsl:value-of select="local-name(/child::*[1])"/>Actions* builtinActions =
        new <xsl:value-of select="local-name(/child::*[1])"/>Actions;
    eventTranslator.setUiActions(builtinActions);
        
    <xsl:if test="//actionGroups"> 
    </xsl:if>

    <xsl:apply-templates select="*"/>
}   

void <xsl:value-of select="local-name(/child::*[1])"/>Builder::buildInterface(PropertyNode* prop) const
{
    String name = "<xsl:value-of select="local-name(/child::*[1])"/>";
    PathName path(config().getDataDir());
    path.append("ui").append(name + ".sui");
    PropUtils::PropertyTreeSaver loader(prop, prop->name());
    if (!loader.readPropertyTree(path.name())) {
        QMessageBox::critical(qApp->activeWindow(), 
                              "Error Loading UI Description", 
                              loader.errmsg());
        exit(0);
    }
}   
</xsl:template>

<!-- Registering ui actions -->

<xsl:template match="uiActions">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="uiAction">
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
