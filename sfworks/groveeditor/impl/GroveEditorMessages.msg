#
# 
#
#
15
#
removeText                  Can't remove empty text
removeGroup                 RemoveGroup: corner nodes are not in the same parent
textPos                     Invalid GrovePos: must be TextPos
splitEmpty                  SplitSelection: empty split?
splitBadParents             SplitSelection: uneven parent nodes
removeSelection             RemoveSelection: uneven parent nodes
cantRemoveRootElement       Cannot remove root element
addAttribute                AddAttribute: Attribute already exists
splitTextPos                Split: must be TextPos
splitTextCorner             Text can`t be splitted on it`s corner
splitElemPos                Split: must be ElementPos
splitElemNode               Can't split non-element node
joinEmpty                   Join error: no nodes to join
joinDifferent               Can`t join nodes with different types
joinNeighbours              Join error: nodes are not neighbours
joinElem                    Can't join nonelements
splitJoin                   Can't split unexpected node
copyAsFragment              Can`t copy as fragment
copySelection               Cannot copy selection
pasteElemPos                Paste: must be ElementPos
changePI                    ChangePI: must be a PI node
changeComment               ChangeComment: must be a comment node
posError                    Can't determine GrovePos type
#
# Entity messages
#
entityNoGrove               ExternalEntity grove not built
entityNotParsed             External entity is not parsed
entityDeclNotFound          Entity declaration not found
entityGeneralRef            Only general entity referenes may be inserted
entityRedefine              ConvertToEntity: cannot redefine entity: it has references in the document
entityCannotConvert         Cannot convert to entity
entityPos                   You must be inside entity reference to perform convert-from-entity operation
entityDeclNotExist          Specified entity declaration doesn't exist
entityDeclRemove            Cannot remove declaration of referenced entity
entityRecursive             Attempt to insert recursive entity reference
cantRemoveXincludeSect      Cannot remove Xinclude reference: it is used elsewhere
#
# Out messages
#
errorEmpty                  ''
errorOpSelection            Attempted operation on empty selection
errorAttrSelection          Can't perform selection operations on attributes
errorCrossEntity            Can't perform any operations on cross-section selection
errorDel                    Del: Nothing to delete
errorBackspace              Backspace: Nothing to delete
errorNoName                 Can't insert element with empty name
errorAttrReplace            Cannot replicate attribute in all entity refs
errorSelReplace             Cannot replicate selection positions in all entity references
errorPaste                  Some entities from copy buffer are already declared - cannot paste
errorPaste2                 Some entities from copy buffer are undeclared - cannot paste
#
#
xincludeHrefMissing         XInclude: 'href' attribute is missing
xincludeWrongParse          XInclude: wrong value of 'parse' attribute
xincludeCannotReadUrl       XInclude: cannot read URL
xincludeCannotParseDocument XInclude: unable to parse the document
xincludeNoDocElement        XInclude: document element is missing
xincludeBadTextGrove        XInclude: bad text grove
xincludeTargetNotFound      XInclude: xpointer target not found
xincludeRecursion           XInclude: recursive xinclude
xincludeMustSpecifyProps    XInclude: must specify properties
xincludedAsExternalEntity   XInclude: Can`t xinclude document which is already used as External Entity
xincludedAsXml              XInclude: Can`t xinclude document as TEXT which is already included as XML
xincludedAsText             XInclude: Can`t xinclude document as XML which is already included as TEXT
xincludeAlreadyExists       XInclude: document with same URL already exists

