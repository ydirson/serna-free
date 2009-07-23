// This file was automatically generated from style/InterpreterMessages.msg by msggen.pl.

#ifdef __GNUG__
#pragma implementation
#endif

#include "config.h"
#include "InterpreterMessages.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

const MessageType0 InterpreterMessages::unexpectedEof(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
0
#ifndef SP_NO_MESSAGE_TEXT
,"unexpected end of file"
#endif
);
const MessageType0 InterpreterMessages::invalidChar(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
1
#ifndef SP_NO_MESSAGE_TEXT
,"invalid character"
#endif
);
const MessageType0 InterpreterMessages::unknownHash(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
2
#ifndef SP_NO_MESSAGE_TEXT
,"invalid character after '#'"
#endif
);
const MessageType1 InterpreterMessages::unknownNamedConstant(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
3
#ifndef SP_NO_MESSAGE_TEXT
,"unknown #! named constant %1"
#endif
);
const MessageType1 InterpreterMessages::unexpectedToken(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
4
#ifndef SP_NO_MESSAGE_TEXT
,"unexpected token %1"
#endif
);
const MessageType0 InterpreterMessages::unterminatedString(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
5
#ifndef SP_NO_MESSAGE_TEXT
,"string with no closing quote"
#endif
);
const MessageType0 InterpreterMessages::missingCloseParen(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
6
#ifndef SP_NO_MESSAGE_TEXT
,"missing closing parenthesis"
#endif
);
const MessageType1 InterpreterMessages::invalidNumber(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
7
#ifndef SP_NO_MESSAGE_TEXT
,"invalid number %1"
#endif
);
const MessageType1 InterpreterMessages::invalidAfiiGlyphId(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
8
#ifndef SP_NO_MESSAGE_TEXT
,"invalid AFII glyph identifier %1"
#endif
);
const MessageType1 InterpreterMessages::callNonFunction(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
9
#ifndef SP_NO_MESSAGE_TEXT
,"call of non-function object %1"
#endif
);
const MessageType0 InterpreterMessages::tooManyArgs(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
10
#ifndef SP_NO_MESSAGE_TEXT
,"too many arguments for function"
#endif
);
const MessageType0 InterpreterMessages::oddKeyArgs(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
11
#ifndef SP_NO_MESSAGE_TEXT
,"odd number of keyword/value arguments"
#endif
);
const MessageType0 InterpreterMessages::missingArg(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
12
#ifndef SP_NO_MESSAGE_TEXT
,"missing argument for function call"
#endif
);
const MessageType1 InterpreterMessages::syntacticKeywordAsVariable(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
13
#ifndef SP_NO_MESSAGE_TEXT
,"syntactic keyword %1 used as variable"
#endif
);
const MessageType1 InterpreterMessages::undefinedVariableReference(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
14
#ifndef SP_NO_MESSAGE_TEXT
,"reference to undefined variable %1"
#endif
);
const MessageType0 InterpreterMessages::badCharName(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
15
#ifndef SP_NO_MESSAGE_TEXT
,"no character with that name"
#endif
);
const MessageType1 InterpreterMessages::keywordNotAllowedCall(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
16
#ifndef SP_NO_MESSAGE_TEXT
,"syntactic keyword %1 not valid in call position"
#endif
);
const MessageType1 InterpreterMessages::symbolRequired(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
17
#ifndef SP_NO_MESSAGE_TEXT
,"symbol required (got %1)"
#endif
);
const MessageType1 InterpreterMessages::unknownTopLevelForm(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
18
#ifndef SP_NO_MESSAGE_TEXT
,"unknown top level form %1"
#endif
);
const MessageType1 InterpreterMessages::badModeForm(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
19
#ifndef SP_NO_MESSAGE_TEXT
,"bad form %1 in mode group"
#endif
);
const MessageType1L InterpreterMessages::duplicateDefinition(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
20
#ifndef SP_NO_MESSAGE_TEXT
,"identifier %1 already defined in same part"
,"first definition was here"
#endif
);
const MessageType1 InterpreterMessages::identifierLoop(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
22
#ifndef SP_NO_MESSAGE_TEXT
,"loop in specification of value of %1"
#endif
);
const MessageType0 InterpreterMessages::outOfRange(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
23
#ifndef SP_NO_MESSAGE_TEXT
,"argument out of range"
#endif
);
const MessageType1 InterpreterMessages::unitLoop(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
24
#ifndef SP_NO_MESSAGE_TEXT
,"loop in specification of value of unit %1"
#endif
);
const MessageType1 InterpreterMessages::badUnitDefinition(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
25
#ifndef SP_NO_MESSAGE_TEXT
,"bad value specified for unit %1"
#endif
);
const MessageType1L InterpreterMessages::duplicateUnitDefinition(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
26
#ifndef SP_NO_MESSAGE_TEXT
,"unit %1 already defined in same part"
,"first definition was here"
#endif
);
const MessageType1 InterpreterMessages::undefinedQuantity(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
28
#ifndef SP_NO_MESSAGE_TEXT
,"quantity %1 undefined"
#endif
);
const MessageType0 InterpreterMessages::incompatibleDimensions(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
29
#ifndef SP_NO_MESSAGE_TEXT
,"incompatible dimensions"
#endif
);
const MessageType3 InterpreterMessages::notABoolean(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
30
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a boolean"
#endif
);
const MessageType3 InterpreterMessages::notAPair(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
31
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a pair"
#endif
);
const MessageType3 InterpreterMessages::notAList(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
32
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a list"
#endif
);
const MessageType3 InterpreterMessages::notASymbol(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
33
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a symbol"
#endif
);
const MessageType3 InterpreterMessages::notAString(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
34
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a string"
#endif
);
const MessageType3 InterpreterMessages::notAStringOrSymbol(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
35
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a string or symbol"
#endif
);
const MessageType3 InterpreterMessages::notASymbolOrString(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
36
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 neither a string not a symbol"
#endif
);
const MessageType3 InterpreterMessages::notAChar(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
37
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a char"
#endif
);
const MessageType3 InterpreterMessages::notAStyle(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
38
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a style"
#endif
);
const MessageType3 InterpreterMessages::notAnExactInteger(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
39
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not an exact integer"
#endif
);
const MessageType3 InterpreterMessages::notAQuantity(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
40
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a quantity"
#endif
);
const MessageType3 InterpreterMessages::notAColorSpace(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
41
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a color-space"
#endif
);
const MessageType3 InterpreterMessages::notANumber(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
42
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a number"
#endif
);
const MessageType3 InterpreterMessages::notASosofo(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
43
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a sosofo"
#endif
);
const MessageType3 InterpreterMessages::notAnOptSingletonNode(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
44
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not an optional singleton node list"
#endif
);
const MessageType3 InterpreterMessages::notASingletonNode(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
45
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a singleton node list"
#endif
);
const MessageType3 InterpreterMessages::notANodeList(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
46
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a node list"
#endif
);
const MessageType3 InterpreterMessages::notANamedNodeList(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
47
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a named node list"
#endif
);
const MessageType3 InterpreterMessages::notALengthSpec(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
48
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a length or length-spec"
#endif
);
const MessageType3 InterpreterMessages::notAQuantityOrLengthSpec(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
49
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a quantity or length-spec"
#endif
);
const MessageType3 InterpreterMessages::notAPriority(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
50
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not an integer or the symbol \"force\""
#endif
);
const MessageType3 InterpreterMessages::notAPattern(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
51
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a valid element matching pattern"
#endif
);
const MessageType3 InterpreterMessages::notAnAddress(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
52
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not an address"
#endif
);
const MessageType3 InterpreterMessages::notAGlyphId(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
53
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a glyph-id"
#endif
);
const MessageType3 InterpreterMessages::notAGlyphSubstTable(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
54
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a glyph-subst-table"
#endif
);
const MessageType3 InterpreterMessages::notAGlyphIdPairList(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
55
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a list of pairs of glyph-ids"
#endif
);
const MessageType3 InterpreterMessages::notAProcedure(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
56
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a procedure"
#endif
);
const MessageType3 InterpreterMessages::notAVector(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
57
#ifndef SP_NO_MESSAGE_TEXT
,"%2 argument for primitive %1 of wrong type: %3 not a vector"
#endif
);
const MessageType0L InterpreterMessages::duplicateRootRule(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
58
#ifndef SP_NO_MESSAGE_TEXT
,"root rule already defined in same part with same importance"
,"first definition was here"
#endif
);
const MessageType1L InterpreterMessages::duplicateInitialValue(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
60
#ifndef SP_NO_MESSAGE_TEXT
,"initial value already declared for characteristic %1 in same part"
,"first declaration was here"
#endif
);
const MessageType1 InterpreterMessages::invalidStyleKeyword(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
62
#ifndef SP_NO_MESSAGE_TEXT
,"%1 is not a valid keyword in a style expression"
#endif
);
const MessageType2 InterpreterMessages::invalidMakeKeyword(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
63
#ifndef SP_NO_MESSAGE_TEXT
,"%1 is not a valid keyword in a make expression for flow object class %2"
#endif
);
const MessageType1 InterpreterMessages::unknownFlowObjectClass(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
64
#ifndef SP_NO_MESSAGE_TEXT
,"%1 is not the name of any flow object class"
#endif
);
const MessageType1 InterpreterMessages::atomicContent(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
65
#ifndef SP_NO_MESSAGE_TEXT
,"content expression cannot be specified in make expression for atomic flow object class %1"
#endif
);
const MessageType0 InterpreterMessages::labelNotASymbol(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
66
#ifndef SP_NO_MESSAGE_TEXT
,"value for \"label:\" not a symbol"
#endif
);
const MessageType1 InterpreterMessages::badConnection(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
67
#ifndef SP_NO_MESSAGE_TEXT
,"no port for label %1"
#endif
);
const MessageType0 InterpreterMessages::badContentMap(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
68
#ifndef SP_NO_MESSAGE_TEXT
,"invalid content map"
#endif
);
const MessageType1 InterpreterMessages::contentMapBadPort(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
69
#ifndef SP_NO_MESSAGE_TEXT
,"content map references non-existent port %1"
#endif
);
const MessageType1 InterpreterMessages::invalidCharacteristicValue(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
70
#ifndef SP_NO_MESSAGE_TEXT
,"invalid value for %1 characteristic"
#endif
);
const MessageType0 InterpreterMessages::condFail(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
71
#ifndef SP_NO_MESSAGE_TEXT
,"no clause in cond expression matched"
#endif
);
const MessageType1 InterpreterMessages::caseFail(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
72
#ifndef SP_NO_MESSAGE_TEXT
,"no clause in case expression matched %1"
#endif
);
const MessageType1 InterpreterMessages::caseElse(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
73
#ifndef SP_NO_MESSAGE_TEXT
,"expected \"else\" not %1"
#endif
);
const MessageType0 InterpreterMessages::caseUnresolvedQuantities(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
74
#ifndef SP_NO_MESSAGE_TEXT
,"sorry, cannot handle unresolvable quantities in datums in case expression"
#endif
);
const MessageType1 InterpreterMessages::errorProc(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
75
#ifndef SP_NO_MESSAGE_TEXT
,"%1"
#endif
);
const MessageType0 InterpreterMessages::divideBy0(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
76
#ifndef SP_NO_MESSAGE_TEXT
,"division by zero"
#endif
);
const MessageType1 InterpreterMessages::invalidKeyArg(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
77
#ifndef SP_NO_MESSAGE_TEXT
,"procedure does not have %1 keyword argument"
#endif
);
const MessageType0 InterpreterMessages::keyArgsNotKey(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
78
#ifndef SP_NO_MESSAGE_TEXT
,"argument not a keyword"
#endif
);
const MessageType0 InterpreterMessages::oneBody(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
79
#ifndef SP_NO_MESSAGE_TEXT
,"support for more than one style-specification-body not yet implemented"
#endif
);
const MessageType0 InterpreterMessages::specNotArc(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
80
#ifndef SP_NO_MESSAGE_TEXT
,"specification document does not have the DSSSL architecture as a base architecture"
#endif
);
const MessageType0 InterpreterMessages::noStyleSpec(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
81
#ifndef SP_NO_MESSAGE_TEXT
,"specification document did not contain a style-specification-body element"
#endif
);
const MessageType1 InterpreterMessages::unknownCharName(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
82
#ifndef SP_NO_MESSAGE_TEXT
,"unknown character name %1"
#endif
);
const MessageType0 InterpreterMessages::noCurrentNode(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
83
#ifndef SP_NO_MESSAGE_TEXT
,"attempt to use current node when there is none"
#endif
);
const MessageType0 InterpreterMessages::noCurrentProcessingMode(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
84
#ifndef SP_NO_MESSAGE_TEXT
,"attempt to process node in illegal context"
#endif
);
const MessageType0 InterpreterMessages::invalidRadix(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
85
#ifndef SP_NO_MESSAGE_TEXT
,"radix must be 2, 8, 10 or 16"
#endif
);
const MessageType0 InterpreterMessages::sosofoContext(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
86
#ifndef SP_NO_MESSAGE_TEXT
,"this context requires a sosofo"
#endif
);
const MessageType0 InterpreterMessages::styleContext(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
87
#ifndef SP_NO_MESSAGE_TEXT
,"this context requires a style object"
#endif
);
const MessageType0 InterpreterMessages::notInCharacteristicValue(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
88
#ifndef SP_NO_MESSAGE_TEXT
,"procedure can only be used in evaluation of characteristic value"
#endif
);
const MessageType1 InterpreterMessages::expressionNotImplemented(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
89
#ifndef SP_NO_MESSAGE_TEXT
,"sorry, %1 expression not implemented"
#endif
);
const MessageType0 InterpreterMessages::RGBColorArgCount(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
90
#ifndef SP_NO_MESSAGE_TEXT
,"Device RGB color requires three arguments"
#endif
);
const MessageType0 InterpreterMessages::RGBColorArgType(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
91
#ifndef SP_NO_MESSAGE_TEXT
,"arguments for Device RGB color must be numbers"
#endif
);
const MessageType0 InterpreterMessages::RGBColorArgRange(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
92
#ifndef SP_NO_MESSAGE_TEXT
,"arguments for Device RGB color must be in the range 0 to 1"
#endif
);
const MessageType1 InterpreterMessages::unknownColorSpaceFamily(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
93
#ifndef SP_NO_MESSAGE_TEXT
,"unknown color-space family %1"
#endif
);
const MessageType0 InterpreterMessages::deviceRGBColorSpaceNoArgs(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
94
#ifndef SP_NO_MESSAGE_TEXT
,"Device RGB Color Space Family does not take any arguments"
#endif
);
const MessageType1 InterpreterMessages::notABuiltinInheritedC(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
95
#ifndef SP_NO_MESSAGE_TEXT
,"%1 is not a pre-defined inherited characteristic"
#endif
);
const MessageType1 InterpreterMessages::invalidNumberFormat(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
96
#ifndef SP_NO_MESSAGE_TEXT
,"invalid number format %1"
#endif
);
const MessageType1 InterpreterMessages::invalidPublicIdChar(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
97
#ifndef SP_NO_MESSAGE_TEXT
,"invalid character %1 in public identifier"
#endif
);
const MessageType1 InterpreterMessages::debug(
MessageType::info,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
98
#ifndef SP_NO_MESSAGE_TEXT
,"debug %1"
#endif
);
const MessageType0 InterpreterMessages::useLoop(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
99
#ifndef SP_NO_MESSAGE_TEXT
,"circular use of specification parts"
#endif
);
const MessageType1 InterpreterMessages::missingPart(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
100
#ifndef SP_NO_MESSAGE_TEXT
,"no style-specification or external-specification with ID %1"
#endif
);
const MessageType0 InterpreterMessages::noParts(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
101
#ifndef SP_NO_MESSAGE_TEXT
,"document did not contain any style-specifications or external-specifications"
#endif
);
const MessageType0 InterpreterMessages::tableCellOutsideTable(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
102
#ifndef SP_NO_MESSAGE_TEXT
,"table-cell flow object not inside a table"
#endif
);
const MessageType0 InterpreterMessages::tableRowOutsideTable(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
103
#ifndef SP_NO_MESSAGE_TEXT
,"table-row flow object not inside a table"
#endif
);
const MessageType1 InterpreterMessages::noNodePropertyValue(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
104
#ifndef SP_NO_MESSAGE_TEXT
,"no value for node property %1"
#endif
);
const MessageType0 InterpreterMessages::returnNotNodeList(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
105
#ifndef SP_NO_MESSAGE_TEXT
,"value returned by procedure was not a node-list"
#endif
);
const MessageType0 InterpreterMessages::stackTrace(
MessageType::info,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
106
#ifndef SP_NO_MESSAGE_TEXT
,"called from here"
#endif
);
const MessageType1 InterpreterMessages::stackTraceEllipsis(
MessageType::info,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
107
#ifndef SP_NO_MESSAGE_TEXT
,"called from here...(%1 calls omitted)"
#endif
);
const MessageType0 InterpreterMessages::processNodeLoop(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
108
#ifndef SP_NO_MESSAGE_TEXT
,"node processing loop detected"
#endif
);
const MessageType0 InterpreterMessages::spliceNotList(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
109
#ifndef SP_NO_MESSAGE_TEXT
,"unquote-splicing expression does not evaluate to a list"
#endif
);
const MessageType0 InterpreterMessages::readOnly(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
110
#ifndef SP_NO_MESSAGE_TEXT
,"object is read-only"
#endif
);
const MessageType1 InterpreterMessages::topLevelAssignment(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
111
#ifndef SP_NO_MESSAGE_TEXT
,"assignment to top-level variable %1"
#endif
);
const MessageType0 InterpreterMessages::continuationDead(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
112
#ifndef SP_NO_MESSAGE_TEXT
,"invalid call to continuation"
#endif
);
const MessageType0 InterpreterMessages::patternEmptyGi(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
113
#ifndef SP_NO_MESSAGE_TEXT
,"empty generic identifier in pattern"
#endif
);
const MessageType0 InterpreterMessages::patternNotList(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
114
#ifndef SP_NO_MESSAGE_TEXT
,"pattern is not a list"
#endif
);
const MessageType1 InterpreterMessages::patternBadGi(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
115
#ifndef SP_NO_MESSAGE_TEXT
,"%1 cannot be used as a generic identifier in a pattern"
#endif
);
const MessageType1 InterpreterMessages::patternBadMember(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
116
#ifndef SP_NO_MESSAGE_TEXT
,"%1 cannot occur in a pattern"
#endif
);
const MessageType0 InterpreterMessages::patternMissingQualifierValue(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
117
#ifndef SP_NO_MESSAGE_TEXT
,"value missing for qualifier in pattern"
#endif
);
const MessageType1 InterpreterMessages::patternUnknownQualifier(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
118
#ifndef SP_NO_MESSAGE_TEXT
,"unknown pattern qualifier %1"
#endif
);
const MessageType2 InterpreterMessages::patternBadQualifierValue(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
119
#ifndef SP_NO_MESSAGE_TEXT
,"bad value %1 for %2 qualifier in pattern"
#endif
);
const MessageType0 InterpreterMessages::patternChildRepeat(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
120
#ifndef SP_NO_MESSAGE_TEXT
,"repeat qualifier not allowed inside children qualifier"
#endif
);
const MessageType0 InterpreterMessages::patternBadAttributeQualifier(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
121
#ifndef SP_NO_MESSAGE_TEXT
,"bad value for attributes qualifier in pattern"
#endif
);
const MessageType1L InterpreterMessages::ambiguousStyle(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
122
#ifndef SP_NO_MESSAGE_TEXT
,"characteristic %1 applied in style rule with same specificity"
,"other style rule is here"
#endif
);
const MessageType0 InterpreterMessages::ambiguousMatch(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
124
#ifndef SP_NO_MESSAGE_TEXT
,"node matches more than one pattern with the same specificity"
#endif
);
const MessageType1 InterpreterMessages::uninitializedVariableReference(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
125
#ifndef SP_NO_MESSAGE_TEXT
,"reference to uninitialized variable %1"
#endif
);
const MessageType1 InterpreterMessages::actualLoop(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
126
#ifndef SP_NO_MESSAGE_TEXT
,"circular use of actual value of characteristic %1"
#endif
);
const MessageType1L InterpreterMessages::duplicateCharacteristic(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
127
#ifndef SP_NO_MESSAGE_TEXT
,"characteristic %1 already defined in same part"
,"first definition was here"
#endif
);
const MessageType1L InterpreterMessages::duplicateFlowObjectClass(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
129
#ifndef SP_NO_MESSAGE_TEXT
,"flow object class %1 already defined in same part"
,"first definition was here"
#endif
);
const MessageType1 InterpreterMessages::undefinedMode(
MessageType::error,
#ifdef BUILD_LIBSP
MessageFragment::libModule,
#else
MessageFragment::appModule,
#endif
131
#ifndef SP_NO_MESSAGE_TEXT
,"mode %1 not defined"
#endif
);
#ifdef SP_NAMESPACE
}
#endif
