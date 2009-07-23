#
# 
#
# XML Xpath processor messages
#
4
#
#
# Retrieval messages
#
retrievalStrFromBool        Attempt to get string from bool
retrievalDblFromBool        Attempt to get double from bool
retrievalSetFromBool        Attempt to get nodeset from bool
retrievalBoolFromNum        Attempt to get bool from numeric
retrievalStrFromNum         Attempt to get string from numeric
retrievalSetFromNum         Attempt to get nodeset from numeric
retrievalBoolFromStr        Attempt to get bool from string
retrievalDblFromStr         Attempt to get double from string
retrievalSetFromStr         Attempt to get nodeset from string
retrievalBoolFromSet        Attempt to get bool from nodeset
retrievalStrFromSet         Attempt to get string from nodeset
retrievalDblFromSet         Attempt to get double from nodeset
retrievalBoolUndef          Attempt to get bool from undefined
retrievalStrUndef           Attempt to get string from undefined
retrievalDblUndef           Attempt to get double from undefined
retrievalSetUndef           Attempt to get nodeset from undefined
#
# Conversion messages
#
conversionStrToSet          Cannot convert string to nodeset
conversionNumToSet          Cannot convert numeric to nodeset
#
# Operation messages
#
operationComp               Cannot compare to undefined value
operationConv               Cannot convert undefined value
#
# Invalid Expression messages
#
invExprJoin                 Optimization error: joining different axes
invExprLeftOpBin            No left operand in BinaryExpr
invExprRightOpBin           No right operand in BinaryExpr
invExprLeftOpPred           No left operand in PredicateExpr
invExprRightOpPred          No predicate in PredicateExpr
invExprNs                   Unmapped namespace prefix: %0
invExprLeftOpSlash          No left operand in SlashExpr
invExprRightOpSlash         No right operand in SlashExpr
invExprNoOp                 No operand in UnaryExpr
#
# Parser and tokenizer messages
#
parserUknFunc               Unknown XPath function: %0
parserBadAxis               Bad axis name
parserException             %0: %1 (line: %2, column: %3); element: %4
parserOpExpected            operator expected 
parserBadStr                unterminated string
parserError                 Parse Error: %0
#
# Evaluation Expression messages
#
evalAccess                  ExprInst: do not access value  within notification! %0
evalConcat                  Concatenation error: should be at least 2 arguments %0
evalFuncInst                FunctionInst: argument number is out of range %0
evalFilterInst              FilterInst: improper type of left operand %0
evalUnaryInst               UnaryInst: improper value type of operand %0
evalBinaryInst              BinaryBarInst: operand(s) are not of expected type %0
evalNArgs                   FunctionInst: wrong number of arguments for function %0
evalNoExtFunc               No such function: %0; %1
#
# Instantiation Expression messages
#
instVar                     Variable "%0" undeclared [%1]
