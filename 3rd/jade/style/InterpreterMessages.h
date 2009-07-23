// This file was automatically generated from style/InterpreterMessages.msg by msggen.pl.
#ifndef InterpreterMessages_INCLUDED
#define InterpreterMessages_INCLUDED 1

#ifdef __GNUG__
#pragma interface
#endif
#include "Message.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

struct InterpreterMessages {
  // 0
  static const MessageType0 unexpectedEof;
  // 1
  static const MessageType0 invalidChar;
  // 2
  static const MessageType0 unknownHash;
  // 3
  static const MessageType1 unknownNamedConstant;
  // 4
  static const MessageType1 unexpectedToken;
  // 5
  static const MessageType0 unterminatedString;
  // 6
  static const MessageType0 missingCloseParen;
  // 7
  static const MessageType1 invalidNumber;
  // 8
  static const MessageType1 invalidAfiiGlyphId;
  // 9
  static const MessageType1 callNonFunction;
  // 10
  static const MessageType0 tooManyArgs;
  // 11
  static const MessageType0 oddKeyArgs;
  // 12
  static const MessageType0 missingArg;
  // 13
  static const MessageType1 syntacticKeywordAsVariable;
  // 14
  static const MessageType1 undefinedVariableReference;
  // 15
  static const MessageType0 badCharName;
  // 16
  static const MessageType1 keywordNotAllowedCall;
  // 17
  static const MessageType1 symbolRequired;
  // 18
  static const MessageType1 unknownTopLevelForm;
  // 19
  static const MessageType1 badModeForm;
  // 20
  static const MessageType1L duplicateDefinition;
  // 22
  static const MessageType1 identifierLoop;
  // 23
  static const MessageType0 outOfRange;
  // 24
  static const MessageType1 unitLoop;
  // 25
  static const MessageType1 badUnitDefinition;
  // 26
  static const MessageType1L duplicateUnitDefinition;
  // 28
  static const MessageType1 undefinedQuantity;
  // 29
  static const MessageType0 incompatibleDimensions;
  // 30
  static const MessageType3 notABoolean;
  // 31
  static const MessageType3 notAPair;
  // 32
  static const MessageType3 notAList;
  // 33
  static const MessageType3 notASymbol;
  // 34
  static const MessageType3 notAString;
  // 35
  static const MessageType3 notAStringOrSymbol;
  // 36
  static const MessageType3 notASymbolOrString;
  // 37
  static const MessageType3 notAChar;
  // 38
  static const MessageType3 notAStyle;
  // 39
  static const MessageType3 notAnExactInteger;
  // 40
  static const MessageType3 notAQuantity;
  // 41
  static const MessageType3 notAColorSpace;
  // 42
  static const MessageType3 notANumber;
  // 43
  static const MessageType3 notASosofo;
  // 44
  static const MessageType3 notAnOptSingletonNode;
  // 45
  static const MessageType3 notASingletonNode;
  // 46
  static const MessageType3 notANodeList;
  // 47
  static const MessageType3 notANamedNodeList;
  // 48
  static const MessageType3 notALengthSpec;
  // 49
  static const MessageType3 notAQuantityOrLengthSpec;
  // 50
  static const MessageType3 notAPriority;
  // 51
  static const MessageType3 notAPattern;
  // 52
  static const MessageType3 notAnAddress;
  // 53
  static const MessageType3 notAGlyphId;
  // 54
  static const MessageType3 notAGlyphSubstTable;
  // 55
  static const MessageType3 notAGlyphIdPairList;
  // 56
  static const MessageType3 notAProcedure;
  // 57
  static const MessageType3 notAVector;
  // 58
  static const MessageType0L duplicateRootRule;
  // 60
  static const MessageType1L duplicateInitialValue;
  // 62
  static const MessageType1 invalidStyleKeyword;
  // 63
  static const MessageType2 invalidMakeKeyword;
  // 64
  static const MessageType1 unknownFlowObjectClass;
  // 65
  static const MessageType1 atomicContent;
  // 66
  static const MessageType0 labelNotASymbol;
  // 67
  static const MessageType1 badConnection;
  // 68
  static const MessageType0 badContentMap;
  // 69
  static const MessageType1 contentMapBadPort;
  // 70
  static const MessageType1 invalidCharacteristicValue;
  // 71
  static const MessageType0 condFail;
  // 72
  static const MessageType1 caseFail;
  // 73
  static const MessageType1 caseElse;
  // 74
  static const MessageType0 caseUnresolvedQuantities;
  // 75
  static const MessageType1 errorProc;
  // 76
  static const MessageType0 divideBy0;
  // 77
  static const MessageType1 invalidKeyArg;
  // 78
  static const MessageType0 keyArgsNotKey;
  // 79
  static const MessageType0 oneBody;
  // 80
  static const MessageType0 specNotArc;
  // 81
  static const MessageType0 noStyleSpec;
  // 82
  static const MessageType1 unknownCharName;
  // 83
  static const MessageType0 noCurrentNode;
  // 84
  static const MessageType0 noCurrentProcessingMode;
  // 85
  static const MessageType0 invalidRadix;
  // 86
  static const MessageType0 sosofoContext;
  // 87
  static const MessageType0 styleContext;
  // 88
  static const MessageType0 notInCharacteristicValue;
  // 89
  static const MessageType1 expressionNotImplemented;
  // 90
  static const MessageType0 RGBColorArgCount;
  // 91
  static const MessageType0 RGBColorArgType;
  // 92
  static const MessageType0 RGBColorArgRange;
  // 93
  static const MessageType1 unknownColorSpaceFamily;
  // 94
  static const MessageType0 deviceRGBColorSpaceNoArgs;
  // 95
  static const MessageType1 notABuiltinInheritedC;
  // 96
  static const MessageType1 invalidNumberFormat;
  // 97
  static const MessageType1 invalidPublicIdChar;
  // 98
  static const MessageType1 debug;
  // 99
  static const MessageType0 useLoop;
  // 100
  static const MessageType1 missingPart;
  // 101
  static const MessageType0 noParts;
  // 102
  static const MessageType0 tableCellOutsideTable;
  // 103
  static const MessageType0 tableRowOutsideTable;
  // 104
  static const MessageType1 noNodePropertyValue;
  // 105
  static const MessageType0 returnNotNodeList;
  // 106
  static const MessageType0 stackTrace;
  // 107
  static const MessageType1 stackTraceEllipsis;
  // 108
  static const MessageType0 processNodeLoop;
  // 109
  static const MessageType0 spliceNotList;
  // 110
  static const MessageType0 readOnly;
  // 111
  static const MessageType1 topLevelAssignment;
  // 112
  static const MessageType0 continuationDead;
  // 113
  static const MessageType0 patternEmptyGi;
  // 114
  static const MessageType0 patternNotList;
  // 115
  static const MessageType1 patternBadGi;
  // 116
  static const MessageType1 patternBadMember;
  // 117
  static const MessageType0 patternMissingQualifierValue;
  // 118
  static const MessageType1 patternUnknownQualifier;
  // 119
  static const MessageType2 patternBadQualifierValue;
  // 120
  static const MessageType0 patternChildRepeat;
  // 121
  static const MessageType0 patternBadAttributeQualifier;
  // 122
  static const MessageType1L ambiguousStyle;
  // 124
  static const MessageType0 ambiguousMatch;
  // 125
  static const MessageType1 uninitializedVariableReference;
  // 126
  static const MessageType1 actualLoop;
  // 127
  static const MessageType1L duplicateCharacteristic;
  // 129
  static const MessageType1L duplicateFlowObjectClass;
  // 131
  static const MessageType1 undefinedMode;
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not InterpreterMessages_INCLUDED */
