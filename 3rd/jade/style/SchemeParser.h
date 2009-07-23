// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef SchemeParser_INCLUDED
#define SchemeParser_INCLUDED 1

#include "Interpreter.h"
#include "Expression.h"
#include "Owner.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class SchemeParser : public Messenger {
public:
  SchemeParser(Interpreter &, Owner<InputSource> &);
  void parse();
  bool parseExpression(Owner<Expression> &);
private:
  SchemeParser(const SchemeParser &); // undefined
  void operator=(const SchemeParser &); // undefined

  enum {
    allowEndOfEntity = 01,
    allowFalse = 02,
    allowKeyword = 04,
    allowOpenParen = 010,
    allowCloseParen = 020,
    allowIdentifier = 040,
    allowPeriod = 0100,
    allowOtherExpr = 0200,  // number, character, glyph-id, quote, backquote
    allowExpressionKey = 0400,
    allowKeyDefine = 01000,
    allowKeyElse = 02000,
    allowKeyArrow = 04000, // =>
    allowString = 010000,
    allowHashOptional = 020000,
    allowHashKey = 040000,
    allowHashRest = 0100000,
    allowUnquote = 0200000,
    allowUnquoteSplicing = 0400000,
    allowQuasiquoteKey = 01000000,
    allowVector = 02000000,
    allowHashContents = 04000000,
    allowExpr = (allowFalse|allowKeyword|allowOpenParen|allowIdentifier
		  |allowString|allowHashOptional|allowHashKey|allowHashRest
		  |allowOtherExpr)
  };
  enum Token {
    tokenEndOfEntity,
    tokenTrue,
    tokenFalse,
    tokenString,
    tokenIdentifier,
    tokenKeyword,
    tokenChar,
    tokenNumber,
    tokenGlyphId,
    tokenOpenParen,
    tokenCloseParen,
    tokenPeriod,
    tokenVector,
    tokenQuote,
    tokenQuasiquote,
    tokenUnquote,
    tokenUnquoteSplicing,
    tokenHashRest,
    tokenHashOptional,
    tokenHashKey,
    tokenHashContents,
    tokenVoid
  };
  bool doDefine();
  bool doDefineUnit();
  bool doElement();
  bool doOrElement();
  bool doDefault();
  bool doId();
  bool doRoot();
  bool doMode();
  bool doDeclareInitialValue();
  bool doDeclareCharacteristic();
  bool doDeclareFlowObjectClass();
  bool doDeclareClassAttribute();
  bool doDeclareIdAttribute();
  bool doDeclareFlowObjectMacro();
  bool skipForm();
  bool parseExpression(unsigned allowed, Owner<Expression> &,
		       Identifier::SyntacticKey &, Token &);
  bool parseBegin(Owner<Expression> &expr);
  bool parseSet(Owner<Expression> &expr);
  bool parseLambda(Owner<Expression> &);
  bool parseLet(Owner<Expression> &);
  bool parseLetStar(Owner<Expression> &);
  bool parseLetrec(Owner<Expression> &);
  bool parseBindingsAndBody(Vector<const Identifier *> &vars,
			    NCVector<Owner<Expression> > &inits,
			    Owner<Expression> &body);
  bool parseBindingsAndBody1(Vector<const Identifier *> &vars,
			     NCVector<Owner<Expression> > &inits,
			     Owner<Expression> &body);
  bool parseQuote(Owner<Expression> &);
  bool parseIf(Owner<Expression> &);
  bool parseCond(Owner<Expression> &, bool opt = 0);
  bool parseCase(Owner<Expression> &);
  bool parseOr(Owner<Expression> &);
  bool parseAnd(Owner<Expression> &, bool opt = 0);
  bool parseMake(Owner<Expression> &);
  bool parseStyle(Owner<Expression> &);
  bool parseWithMode(Owner<Expression> &);
  bool parseFormals(Vector<const Identifier *> &,
		    NCVector<Owner<Expression> > &,
		    int &, bool &, int &);
  bool parseDatum(unsigned otherAllowed, ELObj *&, Location &, Token &);
  bool parseSelfEvaluating(unsigned otherAllowed, ELObj *&, Token &);
  bool parseAbbreviation(const char *, ELObj *&);
  bool parseQuasiquote(Owner<Expression> &);
  bool parseQuasiquoteTemplate(unsigned level,
			       unsigned allowed,
			       Owner<Expression> &,
			       Identifier::SyntacticKey &,
			       Token &,
			       bool &spliced);
  void createQuasiquoteAbbreviation(const char *, Owner<Expression> &);
  bool parseRuleBody(Owner<Expression> &, ProcessingMode::RuleType &);
  bool getToken(unsigned, Token &);
  bool isDelimiter(Xchar);
  void extendToken();
  bool scanString();
  void skipComment();
  bool tokenRecover(unsigned, Token &);
  bool tokenIsNumber();
  bool scanString(unsigned, Token &);
  ELObj *convertAfiiGlyphId(const StringC &);
  Identifier *lookup(const StringC &str);
  ProcessingMode *lookupProcessingMode(const StringC &);
  void dispatchMessage(Message &);
  void dispatchMessage(const Message &);
  void initMessage(Message &msg);
  bool dsssl2() const;
  
  Interpreter *interp_;
  Owner<InputSource> in_;
  StringC currentToken_;
  ProcessingMode *defMode_;
  const char *afiiPublicId_;
  bool dsssl2_;
};

inline
Identifier *SchemeParser::lookup(const StringC &str)
{
  return interp_->lookup(str);
}

inline
ProcessingMode *SchemeParser::lookupProcessingMode(const StringC &name)
{
  return interp_->lookupProcessingMode(name);
}

inline
bool SchemeParser::dsssl2() const
{
  return dsssl2_;
}

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not SchemeParser_INCLUDED */
