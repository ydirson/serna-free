#
# 
#
# XML Validator messages
#
# Please note that the ORDER of these messages is important because
# all messages are split into two major sections: Schema processor
# error messages and validation errors.
6
#
##################################################################
#
# Schema parser-related messages
#
noSchema                 No XML Schema to parse
unknownParserException   Internal error: unknown ANTLR exception in Schema parser!
oldW3Cns                 Warning: Schema document does not have namespace %0
noW3Cns                  Schema document does not have correct W3C namespace (should be %0)
schemaAlreadyParsed      This XML Schema is already parsed
notSchemaNsElement       Unexpected element '%0' of non-Schema namespace
unexpectedSchemaConstruct Unexpected XML Schema construct
unknownSchemaElement     Unknown XML Schema element: %0
undeclaredSchemaPrefix   Namespace prefix '%0' undeclared in XML Schema document
parserMessage            Parse error: %0
refElementHasContent     Element with attribute 'ref' should not declare content 
#
# Schema composition
#
dontKnowHowToAccessUri   Don't know how to access URI '%0' - it is not mapped
namespaceNotImported     Namespace with URI '%0' is not imported
importLocationNotMapped  Import location URI '%0' is not mapped in Schema document
unableToIncludeSchema    Unable to include schema from location '%0'
targetNsNotMapped        Target namespace with URI '%0' is not mapped!
targetNsNotMappedImp     Target namespace '%0' is not mapped in imported schema
noPrefixForUri           No prefix is mapped for URI '%0'
cannotObtainCredFromImp  Cannot obtain credential {%0}:%1 from imported schema
redefineNotImplemented   Error: 'redefine' features is not yet implemented
nsOrLocationRequired     'namespace' or 'schemaLocation' is required for import
referenceToUndefined     Reference to undefined component %0
schemaLocationRequired   Non-empty 'schemaLocation' attribute is required
unableToLoadSchema       Unable to load Schema with URI %0, which is mapped to URL %1
includedNsMustBeEqual    TargetNs of included schema (%0) must be empty or equal to targetNs of local schema
importedSchemaHasNoDocElem Imported schema has no document element (?!)
#
# Schema components parser
#
nameRequired             Attribute 'name' is required for top-level schema constructs %0
refRequired              Redefinition of element %0
nameOrRefRequired        'name' or 'ref' attributes are required 
isNotAnExtAttr           Attribute '%0' doesn't belong to the Schema Extensions namespace
isNotAnExtElem           Element '%0' doesn't belong to the Schema Extensions namespace
elemsInScript            Element children prohibited in script content
nameProhibited           'name' attribute is prohibited in this context
refProhibited            'ref' attribute is prohibited in this context
noOccursOnTopLevel       'minOccur' and 'maxOccur' is prohibited for top-level definitions
noFormOnTopLevel         'form' attribute is prohibited for top-level definitions
fixDefConflict           'default' and 'fixed' must not both be present
noValueToUse             'use' attribute is not allowed for top-level without 'default' attribute
noUseDefault             If default and use are both present, use must be 'optional'
noValidBlock             '%0' is an invalid value for the 'block' attribute
noValidFinal             '%0' is an invalid value for the 'final' attribute
noValidForm              '%0' is an invalid value for the 'form' attribute
noValidUse               '%0' is an invalid value for the 'use' attribute
finalOnLocal             'final' attribute is prohibited for local element definitions
substitutionGroupOnLocal 'substititionGroup' is prohibited for local element definitions
attributeRedefinition    Re-definition of top-level attribute %0
elementRedefinition      Re-definition of top-level element %0
simpleTypeRedefinition   Re-definition of a simple type %0
noValidBool              Invalid boolean value
noValidInt               Invalid integer value
notUsedAttr              Extra attribute '%0'
notRightFacet            Facet is not applicable
valueNotDefined          Value for facet '%0' is not defined: default value is used.
fixedFacet               Value for facet '%0' is fixed, setting this facet is not allowed.
facetsConflict           Facet '%0' is in conflict with other facets: '%1'.
minOccursOut             Invalid value of minOccurs attribute
maxOccursOut             Invalid value of maxOccurs attribute
defaultAndMinOccurs      'minOccurs' must be 0, if 'default' is present
duplicateAllToken        Duplicate element %0 in <all> compositor
allInConnector           Using <all> within other compositors prohibited
tokensOverlap            Ambiguous content model: token '%0' overlaps with '%1'
trimOrResultAbsent       One of attributes: trim or trimResult is absent
idConflict               Id conflict: might already be in use
redefinitionOfUndefined  Attempt to redefine undefined component '%0'
#
VALIDATION_ERRORS_FOLLOW -
#
lengthFacetConstrain      Value is constrained by length of '%0'
minLengthFacetConstrain   Value is constrained by min length of '%0'
maxLengthFacetConstrain   Value is constrained by max length of '%0'
patternFacetConstrain     Value does not satisfy any pattern
enumerationFacetConstrain Value does not satisfy any enumeration facet
scaleFacetConstrain       Value is to have %0 digits after decimal point
precisionFacetConstrain   Value is to have %0 digits
notBool                   Value does not represent boolean value: '%0'
notDouble                 Value is not convertable to target numeric type: '%0'
notFloat                  Value is not convertable to float: '%0'
maxInclFacetConstrain     Value is constrained by max inclusive value, it should be <= '%0'
maxExclFacetConstrain     Value is constrained by max exclusive value, it should be < '%0'
minInclFacetConstrain     Value is constrained by min inclusive value, it should be >= '%0'
minExclFacetConstrain     Value is constrained by min exclusive value, it should be > '%0'
notAllowedDigit           Not allowed digit in '%0'
nonPositiveFacetConstrain Value is constrained by nonPositiveInteger,  it should be <= 0
negativeFacetConstrain    Value is constrained by negativeInteger, it should be <= -1
nonNegativeFacetConstrain Value is constrained by nonNegativeInteger, it should be > 0
positiveFacetConstrain    Value is constrained by positiveInteger, it should be > 1
longFacetConstrain        Value is constrained by long
intFacetConstrain         Value is constrained by int
shortFacetConstrain       Value is constrained by short
byteFacetConstrain        Value is constrained by byte
unsignedLongConstrain     Value is constrained by unsignedLong
unsignedIntConstrain      Value is constrained by unsignedInt
unsignedShortConstrain    Value is constrained by unsignedShort
unsignedByteConstrain     Value is constrained by unsignedByte
notDateTime               Value is not convertable to time duration PnYnMnDTnHnMnS
notRecurringDateTime      Value is not convertable to recurring duration type CCYY-MM-DDThh:mm:ss.sss
recurringPeriodInvalid    Value is in conflict with previosly defined recurring values: '%0' 
notCDATAValid             Value of CDATA simpleType contains #xD(car-ret) or #xA(lfeed) or #x9(tab): '%0'
notToken                  Value of TokenType contains #xD(car-ret) or #xA(lfeed) or #x9(tab) or unnecesary Spaces: '%0'
notLanguage               Value of LanguageType is not valid: '%0'
notNCNamePrefix           Value has empty prefix: '%0'
notNCNameLocalName        Value has empty local name: '%0'
valueIsEmpty              Value is empty
notNmtoken                Value is not NMTOKEN: '%0'
notName                   Value is not Name type: '%0'
notNCName                 Value is not NCName type: '%0'
notId                     Required ID '%0' not found
duplicateId               Duplicate ID '%0'
invalidUnion              Value does not satisfy any of union datatypes
nonEmptyTextChunk         Text is not allowed in element-only content
tooBigDfa                 Size of constructed DFA exceeds compiled-in limit (%0 transitions): simplify your content model
unexpectedToken           Unexpected token: '%0'
missingMandatoryElem      Missing mandatory element '%0' - you may have removed it
prematureEndOfSeq         Premature end of input sequence
multipleElemsInAll        Multiple definitions of element '%0' in <all> connector
missedAllElem             Missed required element '%0' in <all> connector 
onlyElemsInAll            Only elements allowed in <all> connector
partialOnlyInEditMode     Partial validation works only with 'editMode' validation flag set
mustValidateParent        Attempt to partially validate element which itself is not locally valid
possibleTokensAre         Possible tokens are: %0
elementTypeNotFound       Element type for '{%0}%1' not found
simpleTypeNotFound        Simple type for '{%0}%1' not found
cannotValidateAbstract    Cannot validate against an abstract type
cannotDataValidateMixed   Cannot data-validate mixed content
elemsInSimpleContent      Element children prohibited in simple content
cannotDataValidateElt     Cannot data-validate element-only or mixed content
invalidBaseType           Invalid base type or it does not exist
attrInvalidStart          Attribute name must not match 'xmlns'
attrProhibited            Attribute '%0' is prohibited
attrRequired              Attribute '%0' is required
attrFixed                 Overriding fixed value '%0' for attribute '%1'
attrNotValid              Attribute '%0' is not valid
attrComplexBaseType       Attribute '%0' is not valid: complextype is not allowed
attrUnknownBaseType       Attribute '%0' is not valid: unknown basetype class
attrExtra                 Element '%0' has undefined extra attribute '%1'
noSubstDerivation         Element is prohibited to be the head of Substitution group
noSubstGroup              Element is abstract and there are no elements of substitution group
contentShouldBeEmpty      Element '%0' has invalid content
simpleContentEmpty        Simple content should be empty
defaultMixedContent       Element content keeps not only text for default or fixed state
fixedNotEqContent         Fixed attribute does not match element's content
regexError                There is trimming error: '%0'
notNodeset                XPath expression is wrong or not right content: (%0) is not a nodeset 
emptyNodeset              XPath expression is wrong or not right content: (%0) nodeset is empty
notElement                XPath expression is wrong or not right content: nodeset keeps not only elements                
keyViolation              Key violation of %0 identity-constraint
noCorrespondedKey         Keyref has not found corresponded key from referenced identity-constraint
fetchScriptError          Fetching python script error: %0
parseScriptError          Error parsing python script: %0
noDvOrigin                Broken DvOrigin argument in function: pyValidateComplex(...)
parseXpathExprError       Error parsing xpath expression
evalXpathExprError        Error evaluating (calculation) of xpath expression
getResultScriptError      Error in result value of python script: %0
preScriptReturn           Python script of pre-validation process returned negative result
postScriptReturn          Python script of post-validation process returned negative result
scriptReturn              Python script of complex type validation process returned negative result
userDefined               CUSTOM:
diffMixed                 Mismatched mixed property of base type content
mismatchedConnectors      Mismatched content of base complex type
noDerivationExt           Base type prohibits derivation by extension
noDerivationRest          Base type prohibits derivation by restriction
baseHasSimpleCont         Base type has simple content
baseHasNoContent          Base type has no valid content
baseHasNoConector         Base type has no valid connector of content     
baseIsNotComplex          Base type is not complextype
noBaseType                Base type is null
diffContent               Contents for both types are different
occursWider               Occurence range is extended, not restricted
elemNotValid              Element has undefined reference
diffElemNames             Element names  for both types are different
missDefault               Mismatched default or fixed in type definition
noElemType                Base type for element is not specified
diffBaseTypes             Elements base types are different 
diffConnectors            Different connectors   
mismatchedStructure       Connector does not match base structure
noAttrInBase              Extra %0, not specified in base type  
noAttrInCur               Not specified %0, which is in base type
cannotDefaultComplex      Cannot use 'default' attribute value for a complex content
elementGenerated          Element '{%0}%1' was automatically generated by fixup mode
