#
# 
#
# XML Formatter processor messages
#
3
#
creationError               Formatting: error creating <%0>: %1
foNotSupported              Formatting info: fo <%0> is not supported, ignored
foNotAllowed                Formatting info: fo <%0> is not allowed in <%1>
foConvertedToInline         Formatting info: fo <%0> in context of <%1> will be shown as inline
pageMasterDuplicated        Formatting info: <%0> with name [%1] is already specified, this one will be ignored
#
# Creation messages
#
rootNotFound                fo:root not found
foOrder                     fo:layout-master-set must precede any fo:page-sequence
noLayoutMasterSet           no fo:layout-master-set found
noPageSequence              No fo:page-sequence found
noMasterReference           PageSequence: no master-reference specified
noPageMaster                PageSequence: no page master found
noFlowName                  FlowFo: no flow-name specified
noRegionSpecs               FlowFo: no region specs found
foRegionDup                 fo:region <%0> was already declared
noPageMasters               layout-master-set has no page masters
noMasterName                SimplePageMaster: no master-name specified
noMasterReferenceSpec       PageMasterReference: no master-reference specified
noPageMasterDeclared        No page master was declared as <%0>
noMasterNameSeq             SequencePageMaster: no master-name specified
noMasterRefSeq              page-sequence-master has no master references
#
# FoController messages
#
foControllerFin             Formatter error (fo controller): Finalizing area which has been detached from its chain
foControllerGen             Formatter error (fo controller): Generated page is corrupted
#
# Parser messages
#
parseToken                  Failed to resolve token
parseExplicitToken          Cannot resolve token %0 
parseColorNumber            Color number expected after '#'
parseChar                   Unexpected character
parseExpr                   Premature end of expr
parseTerm                   term expected
parseNaN                    Division by zero
parseBrace                  Missing ')'
parseFunc                   Expression expected as function arg
parseCommaBrace             Expected ',' or ')'
parseBraceBal               Unbalanced '()'
parseConversion             Invalid conversion requested
parseTokenToNum             Token must resolve to numeric value
parseInvFunc                Invalid function name: %0
parseFuncArg                Bad argument type for function
parseFuncNumArg             Wrong number of function arguments