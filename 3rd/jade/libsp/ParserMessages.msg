# Copyright (c) 1994, 1995, 1996 James Clark
# See the file COPYING for copying permission.
# Next line tells msggen to generate a .cxx file.
!cxx
# Format is+
# TN+tag+relevant clauses+message
# where T is a letter indicating the type, and N is the number of arguments.
Q1+nameLength+9.3.1+length of name must not exceed NAMELEN (%1)
Q1+parameterEntityNameLength+10.5.1.1+length of parameter entity name must not exceed NAMELEN less the length of the PERO delimiter (%1)
Q1+numberLength+9.3.1+length of number must not exceed NAMELEN (%1)
Q1+attributeValueLength+7.9.4.5+length of attribute value must not exceed LITLEN less NORMSEP (%1)
E0+peroGrpoProlog+9.4.4p3+a name group is not allowed in a parameter entity reference in the prolog
E0+groupLevel+10.1.3p8+an entity end in a token separator must terminate an entity referenced in the same group
E2+groupCharacter++character %1 invalid: only %2 and token separators allowed
E0+psRequired+10.1.1p4+a parameter separator is required after a number that is followed by a name start character
E2+markupDeclarationCharacter++character %1 invalid: only %2 and parameter separators allowed
E0+declarationLevel+10.1.1p3+an entity end in a parameter separator must terminate an entity referenced in the same declaration
E0+groupEntityEnd+10.1.3p8+an entity end is not allowed in a token separator that does not follow a token
E1+invalidToken++%1 is not a valid token here
E0+groupEntityReference+10.1.3p7+a parameter entity reference can only occur in a group where a token could occur
E1+duplicateGroupToken+10.1.3p6+token %1 has already occurred in this group
Q1+groupCount+10.1.3.1+the number of tokens in a group must not exceed GRPCNT (%1)
E0+literalLevel+10.1.2p6 9.1.1p3+an entity end in a literal must terminate an entity referenced in the same literal
E1+literalMinimumData+10.1.7p3+character %1 invalid: only minimum data characters allowed
#018E0+literalNonSgml+9.2p2+non-SGML character number %1 not allowed in literal
E0+dataTagPatternNonSgml+11.2.4.4p9+a parameter literal in a data tag pattern must not contain a numeric character reference to a non-SGML character
E0+dataTagPatternFunction+11.2.4.4p9+a parameter literal in a data tag pattern must not contain a numeric character reference to a function character
E0+eroGrpoStartTag+9.4.4p3+a name group is not allowed in a general entity reference in a start tag
E0+eroGrpoProlog+9.4.4p3+a name group is not allowed in a general entity reference in the prolog
E1+functionName+9.5p4+%1 is not a function name
E1+characterNumber+4.36+%1 is not a character number in the document character set
E1+parameterEntityUndefined+9.4.4.1p2+parameter entity %1 not defined
E1+entityUndefined+9.4.4.1p2+general entity %1 not defined and no default entity
E0+rniNameStart++RNI delimiter must be followed by name start character
#028E0+commentNonSgml+10.3p2+non-SGML character not allowed in comment
E0+commentEntityEnd+10.3p2+unterminated comment: found end of entity inside comment+comment started here
W0+mixedConnectors+10.1.3p4+only one type of connector should be used in a single group
E1+noSuchReservedName++%1 is not a reserved name
E1+invalidReservedName++%1 is not allowed as a reserved name here
Q1+minimumLiteralLength+10.1.7.1+length of interpreted minimum literal must not exceed reference LITLEN (%1)
Q1+tokenizedAttributeValueLength+7.9.4.5 7.9.3p5+length of tokenized attribute value must not exceed LITLEN less NORMSEP (%1)
Q1+systemIdentifierLength+10.1.6.1+length of system identifier must not exceed LITLEN (%1)
Q1+parameterLiteralLength+10.1.2.1+length of interpreted parameter literal must not exceed LITLEN (%1)
Q1+dataTagPatternLiteralLength+11.2.4.5p3+length of interpreted parameter literal in data tag pattern must not exceed DTEMPLEN
E0+literalClosingDelimiter++literal is missing closing delimiter
E2+paramInvalidToken++%1 invalid: only %2 and parameter separators are allowed
E2+groupTokenInvalidToken++%1 invalid: only %2 and token separators are allowed
E2+connectorInvalidToken++%1 invalid: only %2 and token separators are allowed
E1+noSuchDeclarationType++unknown declaration type %1
E1+dtdSubsetDeclaration++%1 declaration not allowed in DTD subset
#
E1+declSubsetCharacter++character %1 not allowed in declaration subset
E0+documentEndDtdSubset++end of document in DTD subset
E1+prologCharacter++character %1 not allowed in prolog
E0+documentEndProlog++end of document in prolog
E1+prologDeclaration++%1 declaration not allowed in prolog
E1+rankStemGenericIdentifier+11.2.1p2+%1 used both a rank stem and generic identifier
E0+missingTagMinimization+11.2p3+omitted tag minimization parameter can be omitted only if "OMITTAG NO" is specified on the SGML declaration
E1+duplicateElementDefinition+11.2p2+element type %1 already defined
E0+entityApplicableDtd++entity reference with no applicable DTD
E1+commentDeclInvalidToken+10.3p1+invalid comment declaration: found %1 outside comment but inside comment declaration+comment declaration started here
E1+instanceDeclaration++%1 declaration not allowed in instance
E0+contentNonSgml++non-SGML character not allowed in content
E1+noCurrentRank++no current rank for rank stem %1
E1+duplicateAttlistNotation+11.4.1.1p4+duplicate attribute definition list for notation %1
E1+duplicateAttlistElement+11.3p4+duplicate attribute definition list for element %1
E0+endTagEntityEnd++entity end not allowed in end tag
E1+endTagCharacter++character %1 not allowed in end tag
E1+endTagInvalidToken++%1 invalid: only s and tagc allowed here
E0+pcdataNotAllowed++character data is not allowed here
E1+elementNotAllowed++document type does not allow element %1 here
E2+missingElementMultiple++document type does not allow element %1 here; missing one of %2 start-tag
E2+missingElementInferred++document type does not allow element %1 here; assuming missing %2 start-tag
E1+startTagEmptyElement+7.3.1.1p3+no start tag specified for implied empty element %1
E1+omitEndTagDeclare++end tag for %1 omitted, but its declaration does not permit this+start tag was here
E1+omitEndTagOmittag+7.3.1p1+end tag for %1 omitted, but OMITTAG NO was specified+start tag was here
E1+omitStartTagDeclaredContent+7.3.1.1p2+start tag omitted for element %1 with declared content
E1+elementEndTagNotFinished++end tag for %1 which is not finished
E1+omitStartTagDeclare++start tag for %1 omitted, but its declaration does not permit this
Q1+taglvlOpenElements++number of open elements exceeds TAGLVL (%1)
E1+undefinedElement++element %1 undefined
E0+emptyEndTagNoOpenElements++empty end tag but no open elements
E1+elementNotFinished++%1 not finished but containing element ended
E1+elementNotOpen++end tag for element %1 which is not open
E1+internalParameterDataEntity++internal parameter entity %1 cannot be CDATA or SDATA
E1+attributeSpecCharacter++character %1 not allowed in attribute specification list
E0+unquotedAttributeValue++an attribute value must be a literal unless it contains only name characters
E0+attributeSpecEntityEnd++entity end not allowed in attribute specification list except in attribute value literal
E1+externalParameterDataSubdocEntity++external parameter entity %1 cannot be CDATA, SDATA, NDATA or SUBDOC
W1+duplicateEntityDeclaration++duplicate declaration of entity %1
W1+duplicateParameterEntityDeclaration++duplicate declaration of parameter entity %1
E0+piEntityReference+10.5.3p9+a reference to a PI entity is allowed only in a context where a processing instruction could occur
E0+internalDataEntityReference+10.5.3p9+a reference to a CDATA or SDATA entity is allowed only in a context where a data character could occur
E0+externalNonTextEntityReference+9.4p0+a reference to a subdocument entity or external data entity is allowed only in a context where a data character could occur
E0+externalNonTextEntityRcdata+9.4p0+a reference to a subdocument entity or external data entity is not allowed in replaceable character data
E0+entlvl+9.4.1+the number of open entities cannot exceed ENTLVL
E0+piEntityRcdata+10.5.3p9+a reference to a PI entity is not allowed in replaceable character data
E1+recursiveEntityReference+9.4p4+entity %1 is already open
E1+undefinedShortrefMapInstance+11.6.2p2+short reference map %1 not defined
E0+usemapAssociatedElementTypeDtd+11.6.1p1+short reference map in DTD must specify associated element type
E0+usemapAssociatedElementTypeInstance+11.6.2p1+short reference map in document instance cannot specify associated element type
#092
E2+undefinedShortrefMapDtd+11.6.1p2+short reference map %1 for element %2 not defined in DTD
E1+unknownShortrefDelim+11.5p4+%1 is not a short reference delimiter
E1+delimDuplicateMap+11.5p6+short reference delimiter %1 already mapped in this declaration
E0+noDocumentElement++no document element
#097E0+
E0+processingInstructionEntityEnd++entity end not allowed in processing instruction
Q1+processingInstructionLength+8.1p1+length of processing instruction must not exceed PILEN (%1)
E0+processingInstructionClose++missing pic delimiter
E0+attributeSpecNameTokenExpected+7.9p2 7.9.1.2p1+an attribute specification must start with a name or name token
E1+noSuchAttributeToken+7.9.1.2p1+%1 is not a member of a group specified for any attribute
E0+attributeNameShorttag+7.9.1.2+the name and vi delimiter can be omitted from an attribute specification only if SHORTTAG YES is specified
E1+noSuchAttribute++there is no attribute %1
E0+attributeValueExpected+7.9.3p1+an attribute value specification must start with a literal or a name character
Q1+nameTokenLength+9.3.1+length of name token must not exceed NAMELEN (%1)
E0+attributeSpecLiteral+7.9.3p1 7.9.1.2p1+an attribute value literal can occur in an attribute specification list only after a vi delimiter
E1+duplicateAttributeSpec+7.9p5+duplicate specification of attribute %1
E1+duplicateAttributeDef+11.3.2p2+duplicate definition of attribute %1
E0+emptyDataAttributeSpec+11.4.1.2p3+data attribute specification must be omitted if attribute specification list is empty
E0+markedSectionEnd+10.4p6+marked section end not in marked section declaration
E1+markedSectionLevel+10.4.1p1+number of open marked sections must not exceed TAGLVL (%1)
E0+unclosedMarkedSection+10.4p1+missing marked section end+marked section started here
E0+specialParseEntityEnd++entity end in character data, replaceable character data or ignored marked section
Q2+normalizedAttributeValueLength+7.9.4.5+normalized length of attribute value literal must not exceed LITLEN (%1); length was %2
E0+attributeValueSyntax+7.9.4.1p1+syntax of attribute value does not conform to declared value
E2+attributeValueChar+7.9.4.1p1+character %1 is not allowed in the value of attribute %2
E1+attributeValueMultiple+7.9.4.1p1+value of attribute %1 must be a single token
E2+attributeValueNumberToken+7.9.4p1+value of attribute %2 invalid: %1 cannot start a number token
E2+attributeValueName+7.9.4p1+value of attribute %2 invalid: %1 cannot start a name
E1+attributeMissing+7.9p4+non-impliable attribute %1 not specified but OMITTAG NO and SHORTTAG NO
E1+requiredAttributeMissing+7.9p4 7.9.1.1p2+required attribute %1 not specified
E1+currentAttributeMissing+7.9.1.1p2+first occurrence of current attribute %1 not specified
E1+invalidNotationAttribute+7.9.4.4p1+%1 is not a notation name
E1+invalidEntityAttribute+7.9.4.3p1+%1 is not a general entity name
E3+attributeValueNotInGroup+7.9.4.1p2+value of attribute %2 cannot be %1; must be one of %3
E1+notDataOrSubdocEntity+7.9.4.3p1+%1 is not a data or subdocument entity
E3+ambiguousModelInitial++content model is ambiguous: when no tokens have been matched, both the %2 and %3 occurrences of %1 are possible
E5+ambiguousModel++content model is ambiguous: when the current token is the %2 occurrence of %1, both the %4 and %5 occurrences of %3 are possible
E5+ambiguousModelSingleAnd++content model is ambiguous: when the current token is the %2 occurrence of %1 and the innermost containing and group has been matched, both the %4 and %5 occurrences of %3 are possible
E6+ambiguousModelMultipleAnd++content model is ambiguous: when the current token is the %2 occurrence of %1 and the innermost %3 containing and groups have been matched, both the %5 and %6 occurrences of %4 are possible
E1+commentDeclarationCharacter++invalid comment declaration: found character %1 outside comment but inside comment declaration+comment declaration started here
E1+nonSgmlCharacter++non SGML character number %1
E0+dataMarkedSectionDeclSubset++data or replaceable character data in declaration subset
E1+duplicateId+4.153+ID %1 already defined+ID %1 first defined here
E1+notFixedValue+4.136+value of fixed attribute %1 not equal to default
E1+sdCommentSignificant++character %1 is not significant in the reference concrete syntax and so cannot occur in a comment in the SGML declaration
E1+standardVersion+13p2+minimum data of first minimum literal in SGML declaration must be "ISO 8879:1986" or "ISO 8879:1986 (ENR)" or "ISO 8879:1986 (WWW)" not %1
E1+namingBeforeLcnmstrt+13.4.5p1+parameter before "LCNMSTRT" must be "NAMING" not %1
E1+sdEntityEnd+13p1+unexpected entity end in SGML declaration: only %1, S separators and comments allowed
E2+sdInvalidNameToken++%1 invalid: only %2 and parameter separators allowed
E1+numberTooBig++magnitude of %1 too big (length exceeds NAMELEN)
E1+sdLiteralSignificant++character %1 is not significant in the reference concrete syntax and so cannot occur in a literal in the SGML declaration except as the replacement of a character reference
E1+syntaxCharacterNumber+4.36+%1 is not a valid syntax reference character number
E0+sdParameterEntity+451p7+a parameter entity reference cannot occur in an SGML declaration
E2+sdParamInvalidToken++%1 invalid: only %2 and parameter separators are allowed
E0+giveUp++cannot continue because of previous errors
E1+sdMissingCharacters++SGML declaration cannot be parsed because the character set does not contain characters having the following numbers in ISO 646: %1
E1+missingMinimumChars++the specified character set is invalid because it does not contain the minimum data characters having the following numbers in ISO 646: %1
E1+duplicateCharNumbers++character numbers declared more than once: %1
E1+codeSetHoles++character numbers should have been declared UNUSED: %1
W1+basesetCharsMissing++character numbers missing in base set: %1
W1+documentCharMax++characters in the document character set with numbers exceeding %1 not supported
E1+fpiMissingField++invalid formal public identifier %1: missing //
E1+fpiMissingTextClassSpace++invalid formal public identifier %1: no SPACE after public text class
E1+fpiInvalidTextClass++invalid formal public identifier %1: invalid public text class
E1+fpiInvalidLanguage++invalid formal public identifier %1: public text language must be a name containing only upper case letters
E1+fpiIllegalDisplayVersion++invalid formal public identifer %1: public text display version not permitted with this text class
E1+fpiExtraField++invalid formal public identifier %1: extra field
E0+notationIdentifierTextClass++public text class of public identifier in notation identifier must be NOTATION
W1+unknownBaseset++base character set %1 is unknown
E2+lexicalAmbiguity+13.4.6p2+delimiter set is ambiguous: %1 and %2 can be recognized in the same mode
E1+missingSignificant++characters with the following numbers in the syntax reference character set are significant in the concrete syntax but are not in the document character set: %1
E1+translateSyntaxCharDoc++there is no unique character in the document character set corresponding to character number %1 in the syntax reference character set
E1+translateSyntaxCharInternal++there is no unique character in the internal character set corresponding to character number %1 in the syntax reference character set
E1+missingSyntaxChar+13.4.3p2+the character with number %1 in ISO 646 is significant but has no representation in the syntax reference character set
E1+unknownCapacitySet++capacity set %1 is unknown
W1+duplicateCapacity++capacity %1 already specified
E1+capacityExceedsTotalcap++value of capacity %1 exceeds value of TOTALCAP
E1+unknownPublicSyntax++syntax %1 is unknown
E0+nmstrtLength+13.4.5p13+UCNMSTRT must have the same number of characters as LCNMSTRT
E0+nmcharLength+13.4.5p13+UCNMCHAR must have the same number of characters as LCNMCHAR
# This isn't formally a quantity error, but we'll treat it as one.
Q1+subdocLevel+9.4.2p1+number of open subdocuments exceeds quantity specified for SUBDOC parameter in SGML declaration (%1)
E1+subdocEntity+10.5.5p12+entity %1 declared SUBDOC, but SUBDOC NO specified in SGML declaration
E0+parameterEntityNotEnded+10.1.1p2+a parameter entity referenced in a parameter separator must end in the same declaration
X1+missingId++reference to non-existent ID %1
W1+dtdUndefinedElement++generic identifier %1 used in DTD but not defined
E1+elementNotFinishedDocumentEnd++%1 not finished but document ended
E0+subdocGiveUp++cannot continue with subdocument because of previous errors
E0+noDtd++no document type declaration; will parse without validation
E0+noDtdSubset++no internal or external document type declaration subset; will parse without validation
E0+notSgml++this is not an SGML document
Q1+taglen+7.4.2p1+length of start-tag before interpretation of literals must not exceed TAGLEN (%1)
E0+groupParameterEntityNotEnded+10.1.3p7+a parameter entity referenced in a token separator must end in the same group
E1+invalidSgmlChar+13.1.2p3+the following character numbers are shunned characters that are not significant and so should have been declared UNUSED: %1
E1+translateDocChar++there is no unique character in the specified document character set corresponding to character number %1 in ISO 646
Q1+attributeValueLengthNeg+7.9.4.5+length of attribute value must not exceed LITLEN less NORMSEP (-%1)
Q1+tokenizedAttributeValueLengthNeg+7.9.4.5 7.9.3p5+length of tokenized attribute value must not exceed LITLEN less NORMSEP (-%1)
E1+scopeInstanceQuantity+13.3p9+concrete syntax scope is INSTANCE but value of %1 quantity is less than value in reference quantity set
E1+basesetTextClass+13.1.1.1p4+public text class of formal public identifier of base character set must be CHARSET
E1+capacityTextClass+13.2p7+public text class of formal public identifier of capacity set must be CAPACITY
E1+syntaxTextClass+13.4.1p6+public text class of formal public identifier of concrete syntax must be SYNTAX
E0+msocharRequiresMsichar+13.4.4p13+when there is an MSOCHAR there must also be an MSICHAR
E1+switchNotMarkup+13.4.1p4+character number %1 in the syntax reference character set was specified as a character to be switched but is not a markup character
E1+switchNotInCharset+13.4.1p4+character number %1 was specified as a character to be switched but is not in the syntax reference character set
W1+ambiguousDocCharacter++character numbers %1 in the document character set have been assigned the same meaning, but this is the meaning of a significant character
E1+oneFunction+13.4.4p11+character number %1 assigned to more than one function
E1+duplicateFunctionName+13.4.4p12+%1 is already a function name
E1+missingSignificant646++characters with the following numbers in ISO 646 are significant in the concrete syntax but are not in the document character set: %1
E1+generalDelimAllFunction+13.4.6.1p5+general delimiter %1 consists solely of function characters
E1+nmcharLetter+13.4.5p11+letters assigned to LCNMCHAR, UCNMCHAR, LCNMSTRT or UCNMSTRT: %1
E1+nmcharDigit+13.4.5p11+digits assigned to LCNMCHAR, UCNMCHAR, LCNMSTRT or UCNMSTRT: %1
E1+nmcharRe+13.4.5p11+character number %1 cannot be assigned to LCNMCHAR, UCNMCHAR, LCNMSTRT or UCNMSTRT because it is RE
E1+nmcharRs+13.4.5p11+character number %1 cannot be assigned to LCNMCHAR, UCNMCHAR, LCNMSTRT or UCNMSTRT because it is RS
E1+nmcharSpace+13.4.5p11+character number %1 cannot be assigned to LCNMCHAR, UCNMCHAR, LCNMSTRT or UCNMSTRT because it is SPACE
E1+nmcharSepchar+13.4.5p11+separator characters assigned to LCNMCHAR, UCNMCHAR, LCNMSTRT or UCNMSTRT: %1
E1+switchLetterDigit+13.4.1p4+character number %1 cannot be switched because it is a Digit, LC Letter or UC Letter
W0+zeroNumberOfCharacters++pointless for number of characters to be 0
E1+nameReferenceReservedName+13.4.7p6+%1 cannot be the replacement for a reference reserved name because it is another reference reserved name
E1+ambiguousReservedName+13.4.7p6+%1 cannot be the replacement for a reference reserved name because it is the replacement of another reference reserved name
E1+duplicateReservedName+13.4.7p2+replacement for reserved name %1 already specified
W1+reservedNameSyntax++%1 is not a valid name in the declared concrete syntax
E1+multipleBSequence+13.4.6.2p6+%1 is not a valid short reference delimiter because it has more than one B sequence
E1+blankAdjacentBSequence+13.4.6.2p6+%1 is not a valid short reference delimiter because it is adjacent to a character that can occur in a blank sequence
E2+delimiterLength+13.4.6p5+length of delimiter %1 exceeds NAMELEN (%2)
W2+reservedNameLength++length of reserved name %1 exceeds NAMELEN (%2)
E1+nmcharNmstrt+13.4.5p12+character numbers assigned to both LCNMCHAR or UCNMCHAR and LCNMSTRT or UCNMSTRT: %1
E0+scopeInstanceSyntaxCharset+13.3p7+when the concrete syntax scope is INSTANCE the syntax reference character set of the declared syntax must be the same as that of the reference concrete syntax
W0+emptyOmitEndTag+11.2.2p8+end-tag minimization should be "O" for element with declared content of EMPTY
W1+conrefOmitEndTag+11.2.2p8+end-tag minimization should be "O" for element %1 because it has CONREF attribute
E1+conrefEmpty+11.3.4p12+element %1 has a declared content of EMPTY and a CONREF attribute
E1+notationEmpty+11.3.3p21+element %1 has a declared content of EMPTY and a NOTATION attribute
E0+dataAttributeDeclaredValue+11.4.1p2+declared value of data attribute cannot be ENTITY, ENTITIES, ID, IDREF, IDREFS or NOTATION
E0+dataAttributeDefaultValue+11.4.1p3+default value of data attribute cannot be CONREF or CURRENT
Q2+attcnt+11.3.1+number of attribute names and name tokens (%1) exceeds ATTCNT (%2)
E0+idDeclaredValue+11.3.4p11+if the declared value is ID the default value must be IMPLIED or REQUIRED
E1+multipleIdAttributes+11.3.3p19+the attribute definition list already declared attribute %1 as the ID attribute
E1+multipleNotationAttributes+11.3.3p19+the attribute definition list already declared attribute %1 as the NOTATION attribute
E1+duplicateAttributeToken+11.3.3p20+token %1 occurs more than once in attribute definition list
E1+notationNoAttributes+11.4.1.2p2+no attributes defined for notation %1
E2+entityNotationUndefined+10.5.5p10+notation %1 for entity %2 undefined
E2+mapEntityUndefined++entity %1 undefined in short reference map %2
E1+attlistNotationUndefined+11.4.1.1p4+notation %1 is undefined but had attribute definition
Q1+bracketedLitlen+10.5.4.1+length of interpreted parameter literal in bracketed text plus the length of the bracketing delimiters must not exceed LITLEN (%1)
Q1+genericIdentifierLength+11.2.1.2+length of rank stem plus length of rank suffix must not exceed NAMELEN (%1)
E0+instanceStartOmittag+7.2p1+document instance must start with document element
Q1+grplvl+11.2.4.5p1+content model nesting level exceeds GRPLVL (%1)
Q1+grpgtcnt+11.2.4.5p2+grand total of content tokens exceeds GRPGTCNT (%1)
E0+unclosedStartTagShorttag+7.4.1p2+unclosed start-tag requires SHORTTAG YES
E0+netEnablingStartTagShorttag+7.4.1p2+net-enabling start-tag requires SHORTTAG YES
E0+unclosedEndTagShorttag+7.5.1p2+unclosed end-tag requires SHORTTAG YES
E0+multipleDtds+7.1p6+DTDs other than base allowed only if CONCUR YES or EXPLICIT YES
E0+afterDocumentElementEntityEnd+7.2p1+end of entity other than document entity after document element
E1+declarationAfterDocumentElement+7.2p1+%1 declaration illegal after document element
E0+characterReferenceAfterDocumentElement+7.2p1+character reference illegal after document element
E0+entityReferenceAfterDocumentElement+7.2p1+entity reference illegal after document element
E0+markedSectionAfterDocumentElement+7.2p1+marked section illegal after document element
E3+requiredElementExcluded+11.2.5.2p5+the %1 occurrence of %2 in the content model for %3 cannot be excluded at this point because it is contextually required
E3+invalidExclusion+11.2.5.2p4+the %1 occurrence of %2 in the content model for %3 cannot be excluded because it is neither inherently optional nor a member of an or group
E0+attributeValueShorttag+7.9.3.1+an attribute value specification must be an attribute value literal unless SHORTTAG YES is specified
E0+conrefNotation+7.9.4.4p2+value cannot be specified both for notation attribute and content reference attribute
E1+duplicateNotationDeclaration+11.4p3+notation %1 already defined
E1+duplicateShortrefDeclaration+11.5p3+short reference map %1 already defined+first defined here
E1+duplicateDelimGeneral+13.4.6.1+general delimiter role %1 already defined
Q1+idrefGrpcnt+7.9.4.5p5+number of id references in start-tag must not exceed GRPCNT (%1)
Q1+entityNameGrpcnt+7.9.4.5p6+number of entity names in attribute specification list must not exceed GRPCNT (%1)
Q2+attsplen+7.9.2+normalized length of attribute specification list must not exceed ATTSPLEN (%1); length was %2
E1+duplicateDelimShortref+13.4.6p2+short reference delimiter %1 already specified
E1+duplicateDelimShortrefSet+13.4.6.2+single character short references were already specified for character numbers: %1
W1+defaultEntityInAttribute++default entity used in entity attribute %1
W1+defaultEntityReference++reference to entity %1 uses default entity 
W2+mapDefaultEntity++entity %1 in short reference map %2 uses default entity
E1+noSuchDtd+12.1.3p6+no DTD %1 declared
W1+noLpdSubset++LPD %1 has neither internal nor external subset
E0+assocElementDifferentAtts+12.2.1p11+element types have different link attribute definitions
E1+duplicateLinkSet+12.2p6+link set %1 already defined
E0+emptyResultAttributeSpec+12.2.2p8+empty result attribute specification
E1+noSuchSourceElement++no source element type %1
E1+noSuchResultElement++no result element type %1
E0+documentEndLpdSubset++end of document in LPD subset
E1+lpdSubsetDeclaration++%1 declaration not allowed in LPD subset
E0+idlinkDeclSimple+12.1.4.3+ID link set declaration not allowed in simple link declaration subset
E0+linkDeclSimple+12.1.4.3+link set declaration not allowed in simple link declaration subset
E1+simpleLinkAttlistElement+12.1.4.3+attributes can only be defined for base document element (not %1) in simple link declaration subset
E0+shortrefOnlyInBaseDtd+11.1p11+a short reference mapping declaration is allowed only in the base DTD
E0+usemapOnlyInBaseDtd+11.1p11+a short reference use declaration is allowed only in the base DTD
E0+linkAttributeDefaultValue+12.1.4.2p3+default value of link attribute cannot be CURRENT or CONREF
E0+linkAttributeDeclaredValue+12.1.4.2p2+declared value of link attribute cannot be ID, IDREF, IDREFS or NOTATION
E0+simpleLinkFixedAttribute+12.1.4.3+only fixed attributes can be defined in simple LPD
E0+duplicateIdLinkSet+12.1.4p1+only one ID link set declaration allowed in an LPD subset
E1+noInitialLinkSet+12.2p7+no initial link set defined for LPD %1
E1+notationUndefinedSourceDtd++notation %1 not defined in source DTD
E0+simpleLinkResultNotImplied+12.1.1p1+result document type in simple link specification must be implied
E0+simpleLinkFeature+12.1.1p5+simple link requires SIMPLE YES
E0+implicitLinkFeature+12.1.2p4+implicit link requires IMPLICIT YES
E0+explicitLinkFeature+12.1.3p4+explicit link requires EXPLICIT YES
E0+lpdBeforeBaseDtd+7.1p1+LPD not allowed before first DTD
E0+dtdAfterLpd+7.1p1+DTD not allowed after an LPD
E1+unstableLpdGeneralEntity++definition of general entity %1 is unstable
E1+unstableLpdParameterEntity++definition of parameter entity %1 is unstable
E1+multipleIdLinkRuleAttribute+12.2.3p3+multiple link rules for ID %1 but not all have link attribute specifications
E1+multipleLinkRuleAttribute+12.2.1p9+multiple link rules for element type %1 but not all have link attribute specifications
E2+uselinkBadLinkSet+12.3p10+link type %1 does not have a link set %2
E1+uselinkSimpleLpd++link set use declaration for simple link process
E1+uselinkBadLinkType+12.3p10+no link type %1
E1+duplicateDtdLpd+12.1p3+both document type and link type %1
E1+duplicateLpd+12.1p3+link type %1 already defined
E1+duplicateDtd+11.1p7+document type %1 already defined
E1+undefinedLinkSet+12.2.2p8+link set %1 used in LPD but not defined
E1+duplicateImpliedResult+12.2.2p6+#IMPLIED already linked to result element type %1
E1+simpleLinkCount+12.1.1.1p1+number of active simple link processes exceeds quantity specified for SIMPLE parameter in SGML declaration (%1)
E0+duplicateExplicitChain+7.1p11+only one chain of explicit link processes can be active
E1+explicit1RequiresSourceTypeBase+12.1.2p5 12.1.3p5 12.1.3.1+source document type name for link type %1 must be base document type since EXPLICIT YES 1
E0+oneImplicitLink+7.1p10+one one implicit link process can be active
W1+sorryLink++sorry, link type %1 not activated: only one implicit or explicit link process can be active (with base document type as source document type)
E0+entityReferenceMissingName+9.4.4p1 9.4.4p2+name missing after name group in entity reference
E1+explicitNoRequiresSourceTypeBase+12.1.2p5 12.1.3p5 12.1.3.1+source document type name for link type %1 must be base document type since EXPLICIT NO
W0+linkActivateTooLate++link process must be activated before base DTD
E0+pass2Ee++unexpected entity end while starting second pass
E2+idlinkElementType+12.2.3p2+type %1 of element with ID %2 not associated element type for applicable link rule in ID link set
E0+datatagNotImplemented++DATATAG feature not implemented
E0+startTagMissingName+7.4p1+generic identifier specification missing after document type specification in start-tag
E0+endTagMissingName+7.5p1+generic identifier specification missing after document type specification in end-tag
E0+startTagGroupNet+7.4.1.3p1+a net-enabling start-tag cannot include a document type specification
E0+documentElementUndefined+11.1p10+DTD did not contain element declaration for document type name
E0+badDefaultSgmlDecl++invalid default SGML declaration
E1+nonExistentEntityRef++reference to entity %1 for which no system identifier could be generated+entity was defined here
W0+pcdataUnreachable+11.2.4p11+content model is mixed but does not allow #PCDATA everywhere
E0+sdRangeNotSingleChar++start or end of range must specify a single character
E0+sdInvalidRange++number of first character in range must not exceed number of second character in range
E0+sdEmptyDelimiter++delimiter cannot be an empty string
W0+tooManyCharsMinimumLiteral++too many characters assigned same meaning with minimum literal
W1+defaultedEntityDefined++earlier reference to entity %1 used default entity
W0+emptyStartTag++empty start-tag
W0+emptyEndTag++empty end-tag
W1+unusedMap++unused short reference map %1
W1+unusedParamEntity++unused parameter entity %1
W1+cannotGenerateSystemIdPublic++cannot generate system identifier for public text %1
W1+cannotGenerateSystemIdGeneral++cannot generate system identifier for general entity %1
W1+cannotGenerateSystemIdParameter++cannot generate system identifier for parameter entity %1
W1+cannotGenerateSystemIdDoctype++cannot generate system identifier for document type %1
W1+cannotGenerateSystemIdLinktype++cannot generate system identifier for link type %1
W1+cannotGenerateSystemIdNotation++cannot generate system identifier for notation %1
W1+excludeIncludeSame++element type %1 both included and excluded
E1+implyingDtd++no document type declaration; implying %1
E1+afdrVersion++minimum data of AFDR declaration must be "ISO/IEC 10744:1997" not %1
E0+missingAfdrDecl++AFDR declaration required before use of AFDR extensions
E0+enrRequired++ENR extensions were used but minimum literal was not "ISO 8879:1986 (ENR)" or "ISO 8879:1986 (WWW)"
E1+numericCharRefLiteralNonSgml++illegal numeric character reference to non-SGML character %1 in literal
E2+numericCharRefUnknownDesc++cannot convert character reference to number %1 because description %2 unrecognized
E3+numericCharRefUnknownBase++cannot convert character reference to number %1 because character %2 from baseset %3 unknown
E1+numericCharRefBadInternal++character reference to number %1 cannot be converted because of problem with internal character set
E1+numericCharRefNoInternal++cannot convert character reference to number %1 because character not in internal character set
E0+wwwRequired++Web SGML adaptations were used but minimum literal was not "ISO 8879:1986 (WWW)"
E1+attributeTokenNotUnique++token %1 can be value for more multiple attributes so attribute name required
Q1+hexNumberLength++length of hex number must not exceed NAMELEN (%1)
W1+entityNameSyntax++%1 is not a valid name in the declared concrete syntax

W0+cdataContent++CDATA declared content
W0+rcdataContent++RCDATA declared content
W0+inclusion++inclusion
W0+exclusion++exclusion
W0+numberDeclaredValue++NUMBER or NUMBERS declared value
W0+nameDeclaredValue++NAME or NAMES declared value
W0+nutokenDeclaredValue++NUTOKEN or NUTOKENS declared value
W0+conrefAttribute++conref attribute
W0+currentAttribute++current attribute
W0+tempMarkedSection++TEMP marked section
W0+instanceIncludeMarkedSection++included marked section in the instance
W0+instanceIgnoreMarkedSection++ignored marked section in the instance
W0+rcdataMarkedSection++RCDATA marked section
W0+piEntity++processing instruction entity
W0+bracketEntity++bracketed text entity
W0+internalCdataEntity++internal CDATA entity
W0+internalSdataEntity++internal SDATA entity
W0+externalCdataEntity++external CDATA entity
W0+externalSdataEntity++external SDATA entity
W0+dataAttributes++attribute definition list declaration for notation
W0+rank++rank stem
W0+missingSystemId++no system id specified
W0+psComment++comment in parameter separator
W0+namedCharRef++named character reference
W0+andGroup++and group
W0+attributeValueNotLiteral++attribute value not a literal
W0+missingAttributeName++attribute name missing
W0+elementGroupDecl++element declaration for group of element types
W0+attlistGroupDecl++attribute definition list declaration for group of element types
W0+emptyCommentDecl++empty comment declaration
W0+commentDeclS++s separator in comment declaration
W0+commentDeclMultiple++multiple comments in comment declaration
W0+missingStatusKeyword++no status keyword
W0+multipleStatusKeyword++multiple status keywords
W0+instanceParamEntityRef++parameter entity reference in document instance
W0+current++current attribute
W0+minimizationParam++element type minimization parameter
W0+refc++reference not terminated by refc delimiter
W0+pcdataNotFirstInGroup++#PCDATA not first in model group
W0+pcdataInSeqGroup++#PCDATA in seq group
W0+pcdataInNestedModelGroup++#PCDATA in nested model group
W0+pcdataGroupNotRep++#PCDATA in model group that does not have rep occurrence indicator
W0+nameGroupNotOr++name group or name token group used connector other than OR
W0+piMissingName++processing instruction does not start with name
W0+instanceStatusKeywordSpecS++s separator in status keyword specification in document instance
W0+externalDataEntityRef++reference to external data entity
W0+attributeValueExternalEntityRef++reference to external entity in attribute value
W1+dataCharDelim++character %1 is the first character of a delimiter but occurred as data
W0+explicitSgmlDecl++SGML declaration was not implied
W0+internalSubsetMarkedSection++marked section in internal DTD subset
E0+nestcWithoutNet++net-enabling start-tag not immediately followed by null end-tag
E0+contentAsyncEntityRef++entity end in different element from entity reference
E0+immednetRequiresEmptynrm++NETENABL IMMEDNET requires EMPTYNRM YES
W0+nonSgmlCharRef++reference to non-SGML character
W0+defaultEntityDecl++declaration of default entity
W0+internalSubsetPsParamEntityRef++reference to parameter entity in parameter separator in internal subset
W0+internalSubsetTsParamEntityRef++reference to parameter entity in token separator in internal subset
W0+internalSubsetLiteralParamEntityRef++reference to parameter entity in parameter literal in internal subset
E0+cannotGenerateSystemIdSgml++cannot generate system identifier for SGML declaration reference
E1+sdTextClass++public text class of formal public identifier of SGML declaration must be SD
E0+sgmlDeclRefRequiresWww++SGML declaration reference was used but minimum literal was not "ISO 8879:1986 (WWW)"
W0+pcdataGroupMemberOccurrenceIndicator++member of model group containing #PCDATA has occurrence indicator
W0+pcdataGroupMemberModelGroup++member of model group containing #PCDATA is a model group
# message fragments
=1000
+delimStart++delimiter 
+delimEnd++
+digit++digit
+nameStartCharacter++name start character
+sepchar++sepchar
+separator++separator
+nameCharacter++name character
+dataCharacter++data character
+minimumDataCharacter++minimum data character
+significantCharacter++significant character
+recordEnd++record end character
+recordStart++record start character
+space++space character
+listSep++, 
+rangeSep++-
+parameterLiteral++parameter literal
+dataTagGroup++data tag group
+modelGroup++model group
+dataTagTemplateGroup++data tag template group
+name++name
+nameToken++name token
+elementToken++element token
+inclusions++inclusions
+exclusions++exclusions
+minimumLiteral++minimum literal
+attributeValueLiteral++attribute value literal
+systemIdentifier++system identifier
+number++number
+attributeValue++attribute value
+capacityName++name of capacity
+generalDelimiteRoleName++name of general delimiter role
+referenceReservedName++reference reserved name
+quantityName++name of quantity
+entityEnd++entity end
+shortrefDelim++short reference delimiter
