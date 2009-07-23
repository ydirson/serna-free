#
# 
#
# XML Xslt processor messages
#
5
#
message                     XSLT: %0
#
# Factory messages
#
factoryBadNodetype          Unrecognized nodetype
factoryBadNode              Improper node for style instruction
factoryErrInstr             Error creating instruction <%0:%1>
factoryInvInstr             Invalid instruction type <%0:%1>
#
# Instance messages
#
instanceInit                Error instantiating match pattern
instanceAttr                attribute can only be applied to element node %0
instanceCals                calsProcess - context must be element node %0
instanceCalsInstr           %0 - cannot find parent CALS instruction (wrong use of CALS table extension in the stylesheet?) %1
instanceCalsCell            calsCell - executed in non-element context %0
instanceCalsGroup           calsGroup - executed in non-element context %0
instanceVar                 error building variable instance: %0 %1
instanceMsg                 Explicitly terminated by xsl:message %0
#
# Instruction messages
#
instrXpath                  %0 %1
instrAttrSet                XSLT instruction:Attempt to use undefined attribute-set: %0 %1
instrContext                %0 \n Instruction context:\n file:%1 (line: %2, column: %3)\n element name: %4
instrApplSections           wrong value for 'sections' attribute:\n may be omit, balance, preserve
instrApplFoldTmpl           wrong  value for 'apply-serna-fold-template' attribute;\n possible values are 'true', 'false'
instrAttrSetCircular        circular use-attribute-sets %0
instrAttrUndef              Attempt to inherit undefined attribute-set: %0 %1
instrCallTmpl               CallTemplate: no such template registered <%0> %1
instrChooseLast             Choose: xsl:otherwise must be the last one %0
instrChoosePer              Choose: only one xsl:otherwise may present %0 
instrForEach                ForEach: sort instructions must precede any other
instrImportCycle            Import/include: %0 directly or indirectly includes or imports itself %1
instrImportNoDoc            Import/include: style document unavailable <%0> %1
instrImportParse            Import/include: cannot parse style document <%0> %1
instrImport                 Import: attempt to import incorrect stylesheet %0
instrVar                    Variable: 'select' attribute is present - content must be empty %0
instrParam                  WithParam: attempt to pass the same parameter twice %0
instrNumberLevel            Bad xsl:number level specification %0
instrNumberLetter           Bad xsl:letter-value specification %0
instrNumberCombine          xsl:number: cannot combine 'value' and 'count' attributes %0
instrCalsTable              CalsTable: no cals-table-empty-cell template defined %0
instrInclude                First child of INCLUDE is not a <stylesheet> %0
instrImportChild            First child of IMPORT is not a <stylesheet> %0
instrNullVar                Null variable name %0
instrDupVar                 Duplicate variable on the same import level:%0 %1
instrAttrInst               Cannot instantiate unprocessed attr %0
instrNumAny                 xsl:number: level=ANY not supported %0
instrNoSrc                  No source document provided %0
instrNotAllowed             Instruction <%0> not allowed in <%1> %2
instrInvNodeType            Invalid node type %0 %1 
instrAttrName               QNAME value required for attribute %0 %1
instrAttrReq                Attribute %0 is required %1
instrAttrExtra              Unknown extra attribute: %0 %1
instrNoInst                 Instruction: no make function for instantiation %0
#
# Attribute Value Template messages
#
attrValTmplBrace            Single closing curly brace in AttributeValueTemplate
attrValTmpl                 AttributeValueTemplate: %0
#
# Engine messages
#
engineNoStyle               Style document not found, uri is empty
engineNoStyleUri            Can not get stylesheet by uri: %0
engineNoSrc                 No source document
engineNoStyleDoc            No stylesheet
engineNoStyleInst           No stylesheet instantiated
engineNoResult              No result grove made
#
# Xpath Evaluation messages
#
xpathEvalArgs               Wrong number of arguments for generate-id() function: must be < 2 %0
xpathEvalDocArgs            document() function must have one or two arguments %0
xpathEvalDocParam           Second argument to document() function must be a node-set %0
#
# Xpath Instantiation messages
#
xpathInstCalsAttrs          Arg 0 of cals-inherited-attribute must be a nodeset %0
xpathInstCalsContext        Parent node of context node of cals-inherited-attribute must be of element type %0
xpathInstDocbookArgs        Wrong number of arguments for docbook-orderedlist-itemnumber function %0
xpathInstCalsTableAttr      CalsAttribute: parent cals-table-cell not found %0
xpathInstCalsTableArgs      Wrong number of arguments for cals-attribute() %0
xpathInstCalsTableNode      Context node of cals-attribute must be of ELEMENT type %0
#
# Pattern Parser messages
#
parserError                 Pattern parse error: %0
