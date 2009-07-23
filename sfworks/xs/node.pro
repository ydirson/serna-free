TEMPLATE=antlr.t

PARSER=GroveAstParser.cpp
PARSER_H=GroveAstParser.hpp
ANTLR_FLAGS= 
#-trace
ANTLR_INPUT=parser/XmlSchema.g

OBJECTS_DIR=.
CONFIG=x11
TMAKE_CC = egcc
TMAKE_CFLAGS_RELEASE =
TMAKE_CXXFLAGS = -O0 -DGROVE_DEBUG -DQT_GROVE_BUILDER -DQT_THREAD_SUPPORT -DNO_XPATH
TMAKE_CXX  = eg++ -g
TMAKE_LINK = eg++ -pthread -g -L../../../build/sfworks/lib -L../../../3rd/lib  -Wl,-rpath,../../../build/sfworks/lib -Wl,-rpath,../../../3rd/lib
#TMAKE_LIBS = -L../../3rd/antlr -lantlr -L/home/apg/se/3rd/jade/libs -lsp -L/st/qt-2.2.3/lib -lqt -lqt-mt
TMAKE_LIBS = -L../../../3rd/lib -lantlr -lpython -lcommon -lgrove -luri -lutil
#TMAKE_LIBS_X11 =
#TMAKE_LIBS_QT  = -lqt
TMAKE_LIBS_QT_OPENGL =
TMAKE_LIBS_OPENGL =
CLEAN_FILES = *.bak
INCLUDEPATH+=../../sfworks;../../../build/sfworks;../../devel;../../../3rd/antlr/lib/cpp;../../../3rd/qt/include;../../../3rd/jade;../../../3rd/rx;../../../3rd/python/2.1/Python-2.1;../../../3rd/python/2.1/Python-2.1/Include
DEPENDPATH=$$INCLUDEPATH

SOURCES = t.cxx \
      Schema.cxx \
      XsData.cxx \
      XsDataImpl.cxx \
      Component.cxx \
      OriginAndAppinfo.cxx \
      Piece.cxx \
      SchemaResource.cxx \
      SchemaResourceBuilder.cxx \
      ComponentRef.cxx \
      ComponentSpace.cxx \
      ImportMap.cxx \
      NcnCred.cxx \
      ExtEnumCache.cxx \  
      ExtPythonScript.cxx \
      XsMessages.cpp \
\
      parser/GroveAst.cxx \
      parser/SchemaParser.cxx \
      parser/TokenTable.cxx \
      parser/AttributeParser.cxx \
\
      complex/XsComplexTypeImpl.cxx \
      complex/XsContent.cxx \
      complex/SimpleContent.cxx \
      complex/ComplexContent.cxx \
      complex/Connector.cxx \
      complex/AllConnector.cxx \
      complex/ChoiceConnector.cxx \
      complex/SequenceConnector.cxx \
      complex/GroupConnector.cxx \
      complex/ContentToken.cxx \
      complex/Particle.cxx \
      complex/InputTokenSequence.cxx \ 
      complex/WildcardParticle.cxx \
      complex/Wildcard.cxx \ 
      complex/ElementParticle.cxx \
      complex/FsmBuild.cxx \
      complex/FsmMatch.cxx \
\
      components/XsSchema.cxx \
      components/XsType.cxx \
      components/XsElement.cxx \
      components/XsElementImpl.cxx \
      components/XsAttribute.cxx \
      components/XsAttributeImpl.cxx \
      components/XsAttributeGroup.cxx \
      components/XsAttributeGroupImpl.cxx \
      components/XsGroup.cxx \
      components/XsGroupImpl.cxx \
      components/XsNotation.cxx \
      components/XsNotationImpl.cxx \
      components/XsAnyAttribute.cxx \
      components/AttributeSet.cxx \
      components/XsIdentityConstraint.cxx \
      components/XsKeyImpl.cxx \
      components/XsKeyRefImpl.cxx \
      components/XsUniqueImpl.cxx \
      \
      datatypes/AnyType.cxx \
      datatypes/AtomicType.cxx \
      datatypes/XsSimpleTypeImpl.cxx \
      datatypes/SimpleDerivedType.cxx \
      datatypes/Restriction.cxx \
      datatypes/List.cxx \
      datatypes/Union.cxx \
      datatypes/XsFacet.cxx \
      datatypes/LengthFacet.cxx \
      datatypes/MinLengthFacet.cxx \
      datatypes/MaxLengthFacet.cxx \
      datatypes/PatternFacet.cxx \
      datatypes/EnumerationFacet.cxx \
      datatypes/MaxInclusiveFacet.cxx \
      datatypes/MinInclusiveFacet.cxx \
      datatypes/MaxExclusiveFacet.cxx \
      datatypes/MinExclusiveFacet.cxx \
      datatypes/TotalDigitsFacet.cxx \
      datatypes/FractionDigitsFacet.cxx \
      datatypes/DurationFacet.cxx \
      datatypes/PeriodFacet.cxx \
      datatypes/WhiteSpaceFacet.cxx \
      datatypes/EncodingFacet.cxx \
      datatypes/StringType.cxx \
      datatypes/AnyUriType.cxx \
      datatypes/BooleanType.cxx \
      datatypes/NormalizedStringType.cxx \
      datatypes/TokenType.cxx \
      datatypes/LanguageType.cxx \
      datatypes/QNameType.cxx \
      datatypes/NmtokenType.cxx \
      datatypes/NmtokensType.cxx \
      datatypes/NameType.cxx \
      datatypes/NCNameType.cxx \
      datatypes/IDType.cxx \
      datatypes/NumberType.cxx \
      datatypes/IntegerType.cxx \
      datatypes/NonPositiveIntegerType.cxx \
      datatypes/NegativeIntegerType.cxx \
      datatypes/LongType.cxx \
      datatypes/IntType.cxx  \
      datatypes/ShortType.cxx \
      datatypes/ByteType.cxx \
      datatypes/NonNegativeIntegerType.cxx \
      datatypes/UnsignedLongType.cxx \
      datatypes/UnsignedIntType.cxx \
      datatypes/UnsignedShortType.cxx \
      datatypes/UnsignedByteType.cxx \
      datatypes/PositiveIntegerType.cxx \
      datatypes/DoubleType.cxx \
      datatypes/FloatType.cxx \
      datatypes/SchemaDateTime.cxx \
      datatypes/RecurringType.cxx \
      datatypes/DurationType.cxx \
      datatypes/DateTimeType.cxx \
      datatypes/TimeType.cxx \
      datatypes/TimePeriodType.cxx \
      datatypes/DateType.cxx \
      datatypes/YearMonthType.cxx \
      datatypes/YearType.cxx \
      datatypes/MonthDayType.cxx \
      datatypes/DayType.cxx \
      datatypes/InitAtomicTypes.cxx \
\
      ../../../3rd/rx/alpha3.8.g1/rxspencer-alpha3.8.g1/regcomp.c \
      ../../../3rd/rx/alpha3.8.g1/rxspencer-alpha3.8.g1/regexec.c \
      ../../../3rd/rx/alpha3.8.g1/rxspencer-alpha3.8.g1/regfree.c 

HEADERS = GroveAst.h \
      GroveAstParserTokenTypes.hpp \
      GroveAstParser.hpp \
      SchemaParser.h \ 
      ../../sfworks/grove/Decls.h \
      ../../sfworks/grove/NodePtr.h \
      ../../sfworks/grove/Node.h \
      ../../sfworks/grove/Nodes.h \
      ../../sfworks/grove/GroveVisitor.h \
      ../../sfworks/grove/Grove.h \
      ../../sfworks/grove/TreelocRep.h \
      ../../sfworks/grove/GroveBuilder.h \
      ../../sfworks/grove/DebugGroveVisitor.h \
      ../../sfworks/grove/XmlNs.h \
      ../../sfworks/grove/ExternalId.h \
      ../../sfworks/grove/ConstrainedEntityIter.h \
      ../../sfworks/grove/EntityDecl.h \ \
      ../../sfworks/grove/EntityReferenceTable.h \
      ../../sfworks/grove/GroveSection.h \
      ../../sfworks/grove/EntityDeclSet.h \ \
      ../../sfworks/grove/GroveSectionRoot.h \
      ../../sfworks/grove/GroveSaver.h \
      ../../sfworks/grove/GroveVisitorMux.h \ \
      ../../sfworks/grove/EltContentIter.h \
      ../../sfworks/grove/Invalidity.h \
      ../../sfworks/grove/NodeInvalidity.h \
      ../../sfworks/grove/ElementIdTable.h \ \
      ../../sfworks/grove/PrologNodes.h

TARGET = t
