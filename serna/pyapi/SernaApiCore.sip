%Module SernaApiCore 0

%Import QtCore/QtCoremod.sip
%Import QtGui/QtGuimod.sip

typedef long long int64;
typedef unsigned long long uint64;

%Include common/WrappedObject.sip
%Include common/SString.sip
%Include common/PropertyNode.sip
%Include common/Command.sip
%Include common/SimpleWatcher.sip
%Include common/Url.sip
%Include common/DavManager.sip
%Include common/DavProtocol.sip

%Include app/SernaConfig.sip

%Include app/UiCommand.sip
%Include app/UiItem.sip
%Include app/UiDocument.sip
%Include app/PluginLoader.sip
%Include app/SernaDoc.sip
%Include app/DocumentPlugin.sip
%Include app/DocTemplateHolder.sip
%Include app/IconProvider.sip

%Include grove/GroveEntity.sip
%Include grove/GroveNodes.sip
%Include grove/GroveWatcher.sip
%Include grove/Grove.sip
%Include grove/GrovePos.sip
%Include grove/GroveCommand.sip
%Include grove/GroveStripInfo.sip
%Include grove/GroveEditor.sip
%Include grove/GroveIdManager.sip
%Include grove/XpathUtils.sip
%Include grove/XmlValidator.sip

%Include app/StructEditor.sip
%Include app/MessageView.sip
%Include app/XsltEngine.sip
%Include app/SernaClipboard.sip
%Include app/MimeHandler.sip
