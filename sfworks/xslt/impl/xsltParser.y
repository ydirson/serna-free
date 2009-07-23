// (c) 2003 by Syntext, Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.
//
// 
//
// This is Bison parser for XPath. This grammar requires Bison 1.35 or later.
//
%{
#include "xslt/xslt_defs.h"
#include "xslt/Exception.h"
#include "xpath/Engine.h"
#include "xpath/XpathFunctionFactory.h"
#include "xslt/impl/xpath_values.h"
#include "xslt/impl/xpath_exprs.h"
#include "xslt/impl/patterns.h"
#include "xslt/impl/Tokenizer.h"
#include "xslt/impl/patterns.h"
#include "xslt/impl/XsltFunctionFactory.h"
#include "xslt/impl/debug.h"

#include <stdlib.h>
#include <stdio.h>

USING_COMMON_NS;

namespace Xslt {

using Xpath::FunctionFactory;         
using Xpath::ExprImpl;

// Argument which are passed to the parser. 
struct ParserArgs {
    Xpath::XpathTokenizer xp_input;
    XsltTokenizer  input;
    RefCntPtr<AltPatternExpr> exprRoot;
    const FunctionFactory* funcFactory;
    const GroveLib::NodeWithNamespace* nsResolver;
    Xpath::AxisExpr* paxis;

    // This is used for lifetime tracking, because parser itself cannot
    // delete nonterminals in case of error
    Vector<RefCntPtr<ExprImpl> > exprHolderVec;
    Vector<RefCntPtr<PQname> > qnameHolderVec;
    
    ParserArgs(const String& istr, 
               const GroveLib::NodeWithNamespace* resolver,
               const Xpath::FunctionFactory* xf)
        : xp_input(istr), input(xp_input, resolver, xf),
          funcFactory(xf),
          nsResolver(resolver), paxis(0)
    {
        exprHolderVec.reserve(64);
        qnameHolderVec.reserve(64);
    }
};

static Xpath::ExprImplPtr self_axis(Xpath::ExprImplPtr expr)
{
    if (expr->type() != Xpath::Expr::AXIS)
        return expr;
    Xpath::AxisExpr* ae = static_cast<Xpath::AxisExpr*>(expr.pointer());
    if (ae->name() != Xpath::AxisExpr::CHILD)
        return expr;
    return new Xpath::AxisSelfExpr(ae->nodeTestExpr());
}

// parameters for bison skeleton parser
#define PARGS(x)        ((ParserArgs*)pargs)->x
#define YYSTYPE         TokenValue
#define YYLEX_PARAM     PARGS(input)
#define YYPARSE_PARAM   pargs
#define YYERROR_VERBOSE
#define YYPRINT(file, type, value) yyprint(file, type, value)
#define NEW_EXPR(e) \
    PARGS(exprHolderVec).push_back(e)->pointer()
#define NEW_PEXPR(e) static_cast<PatternExprImpl*>(NEW_EXPR(e))
#define NEW_QNAME(n) \
    PARGS(qnameHolderVec).push_back(n)->pointer()
%}
%pure_parser            // make parser reentrant
%name_prefix="xpp_"     // prefix for generated parser funcitons
%debug                  // include debug information
%output="xsltParser.cpp" // output file name
%defines                // generate also xpathParser.hxx

%token XT_OPSLASH
%token XT_OP2SLASH
%token XT_IDFUNC
%token XT_KEYFUNC
%token XT_LITERAL
%token XT_XPATHEXPR
%token XT_AXISNAME
%token XT_OPDCOLON
%token XT_NTEST_NODE
%token XT_NTEST_COMMENT
%token XT_NTEST_TEXT
%token XT_NTEST_PI
%token XT_NCNAME
%token XT_BAR

%{
void xpp_error(const char* s);
static int  xpp_lex(YYSTYPE *lvalp, XsltTokenizer& lexerInput);
static void yyprint (FILE *file, int type, TokenValue value);
%}

%%

pattern:       locationPathPattern
                   {
                       PARGS(exprRoot)->addAlternative($<pexpr>1);
                   }
               | pattern XT_BAR locationPathPattern 
                   {
                       PARGS(exprRoot)->addAlternative($<pexpr>3);   
                   }
               ;

locationPathPattern: XT_OPSLASH
                   {
                       $<pexpr>$ = NEW_PEXPR(new RootPatternExpr);
                   }
               | XT_OPSLASH relativePathPattern
                   {
                       $<pexpr>$ = NEW_PEXPR(new SlashPatternExpr
                           (new RootPatternExpr, $<pexpr>2, true));
                   }
               | idKeyFunc 
                   {
                       $<pexpr>$ = 
                           NEW_PEXPR(new StepPatternExpr($<expr>1, 0.5));
                   }
               | idKeyFunc XT_OPSLASH relativePathPattern
                   {
                       $<pexpr>$ = NEW_PEXPR(new SlashPatternExpr
                           (new StepPatternExpr($<expr>1, 0.5),
                            $<pexpr>3, true));
                   }
               | idKeyFunc XT_OP2SLASH relativePathPattern
                   {
                       $<pexpr>$ = NEW_PEXPR(new SlashPatternExpr
                           (new StepPatternExpr($<expr>1, 0.5), 
                            $<pexpr>3, false));
                   }
               | XT_OP2SLASH relativePathPattern
                   {
                       $<pexpr>$ = NEW_PEXPR(new SlashPatternExpr
                           (new RootPatternExpr, $<pexpr>2, false));
                   }   
               | relativePathPattern
               ;

idKeyFunc:     XT_IDFUNC '(' XT_LITERAL ')'
                   {
                       Xpath::ExprImplPtr e = new Xpath::ConstExpr
                           (new Xpath::StringValue($3.asString()));
                       $<expr>$ = NEW_EXPR(PARGS(funcFactory)->makeFunction
                            ("id", (new Xpath::FunctionArgExpr(e, 0)), 
                            PARGS(nsResolver)));
                   }
               | XT_KEYFUNC '(' XT_LITERAL ',' XT_LITERAL ')'
                   {
                       Xpath::ExprImplPtr e = new Xpath::ConstExpr
                           (new Xpath::StringValue($3.asString()));
                       Xpath::ExprImplPtr e1 = new Xpath::ConstExpr
                           (new Xpath::StringValue($5.asString()));
                       $<expr>$ = NEW_EXPR(PARGS(funcFactory)->
                            makeFunction("key", new Xpath::FunctionArgExpr
                                (e, (new Xpath::FunctionArgExpr(e1, 0))), 
                                PARGS(nsResolver)));
                   }
               ;

relativePathPattern: stepPattern
               | relativePathPattern XT_OPSLASH  stepPattern
                   {
                       $<pexpr>$ = NEW_PEXPR(new SlashPatternExpr
                           ($<pexpr>1, $<pexpr>3, true));
                   }
               | relativePathPattern XT_OP2SLASH stepPattern
                   {
                       $<pexpr>$ = NEW_PEXPR(new SlashPatternExpr
                           ($<pexpr>1, $<pexpr>3, false));
                   }
               ;

stepPattern:    axisPredList
                    {
                        const Xpath::AxisExpr* ae = PARGS(paxis);
                        $<pexpr>$ = NEW_PEXPR(new StepPatternExpr(
                            $<expr>1, 0.5, ae->nodeTestExpr(), ae->name()));
                        PARGS(paxis) = 0;
                    }
                | axisExpr
                    {
                        RefCntPtr<Xpath::AxisExpr> ae = static_cast
                            <Xpath::AxisExpr*>(&*self_axis($<expr>1));
                        $<pexpr>$ = NEW_PEXPR(new StepPatternExpr(&*ae,
                            ae->priority(),
                            ae->nodeTestExpr(), ae->name()));
                    }
                ;

axisPredList:   axisPredPair
                | axisPredList predicate 
                    {
                        $<expr>$ = NEW_EXPR(new Xpath::PredicateExpr
                            ($<expr>1, $<expr>2));
                    }
                ;

axisPredPair:   axisExpr predicate
                    {
                        using namespace Xpath;
                        bool can_optimize = !$<expr>1->contextDependency() && 
                            !$<expr>1->isNumeric();
                        ExprImplPtr axis = can_optimize ?
                            $<expr>1 : self_axis($<expr>1);
                        RefCntPtr<PredicateExpr> pe = 
                            new PredicateExpr(axis, $<expr>2);
                        ExprImplPtr opt_expr = pe->optimize(0xFF);
                        if (opt_expr->type() == Expr::AXIS) {
                            opt_expr = self_axis(opt_expr);
                            axis = opt_expr;
                        } 
                        PARGS(paxis) = static_cast<AxisExpr*>(&*axis);
                        $<expr>$ = NEW_EXPR(opt_expr);
                    }
                ;
               
axisExpr:      XT_AXISNAME XT_OPDCOLON nodeTest
                   {    
                        Xpath::ExprImplPtr ae = NEW_EXPR 
                            (Xpath::AxisExpr::make($1.asString(), 
                                static_cast<Xpath::NodeTestExpr*>($<expr>3)));
                        if (ae.isNull()) {
                            xpp_error("Invalid axis name");
                            YYABORT;
                        }    
                        Xpath::AxisExpr::Name axisid =
                            static_cast<Xpath::AxisExpr*>(ae.pointer())->name();
                        if (axisid != Xpath::AxisExpr::CHILD &&
                            axisid != Xpath::AxisExpr::ATTRIBUTE) {
                                xpp_error("only 'child' and 'attribute' axes"
                                    " are allowed in patterns");
                                YYABORT;
                        }
                        $<expr>$ = ae.pointer();
                   }
                | '@' nodeTest 
                    {
                        $<expr>$ = 
                            NEW_EXPR(new Xpath::AxisAttributeExpr
                                (static_cast<Xpath::NodeTestExpr*>($<expr>2)));
                    }
                | nodeTest
                    {
                        $<expr>$ = 
                            NEW_EXPR(new Xpath::AxisChildExpr
                                (static_cast<Xpath::NodeTestExpr*>($<expr>1)));
                    }
                ;

predicate:     XT_XPATHEXPR 
                   {
                       $<expr>1->decRefCnt(); // special: see Tokenizer.cxx
                       $<expr>$ = NEW_EXPR($<expr>1);
                   }
               ;

nodeTest:       nameTest 
                    {
                        $<expr>$ =  NEW_EXPR(new Xpath::NameNodeTestExpr
                            (*$<qname>1, PARGS(nsResolver)));
                    }
                | XT_NTEST_NODE '(' ')'  
                    {
                        $<expr>$ = NEW_EXPR(new Xpath::NodeNodeTestExpr);
                    }
                | XT_NTEST_COMMENT '(' ')'   
                    {
                        $<expr>$ = NEW_EXPR(new Xpath::CommentNodeTestExpr);
                    }
                | XT_NTEST_TEXT '(' ')' 
                    {
                        $<expr>$ = NEW_EXPR(new Xpath::TextNodeTestExpr);
                    }
                | XT_NTEST_PI   '(' XT_LITERAL ')'
                    {
                        $<expr>$ = 
                          NEW_EXPR(Xpath::PiNodeTestExpr::make($3.asString()));
                    }
                | XT_NTEST_PI   '(' ')'
                    {
                        $<expr>$ = 
                            NEW_EXPR(new Xpath::PiNodeTestExpr(String::null()));
                    }
                ;

nameTest:       '*'               
                    { 
                        $<qname>$ = NEW_QNAME(new PQname("*")); 
                    }
                | XT_NCNAME ':' '*'  
                    { 
                        $<qname>$ = NEW_QNAME(new PQname("*", $1.asString()));
                    }
                | qname            
                ;    

qname:          XT_NCNAME  
                    {
                        $<qname>$ = NEW_QNAME(new PQname($1.asString()));
                    }
                | XT_NCNAME ':' XT_NCNAME  
                    {
                        $<qname>$ = 
                            NEW_QNAME(new PQname($3.asString(), $1.asString()));
                    }
                ;
%%

/////////////////////////////////////////////////////////////////////

PatternExprPtr 
PatternExpr::makePattern(const COMMON_NS::String& expression,
                         const GroveLib::NodeWithNamespace* resolver,
                         const Xpath::FunctionFactory* fact)
{
    return PatternExprImpl::makePattern(expression, resolver, fact).pointer();
}

PatternExprPtr
PatternExpr::makeSimplePattern(const Common::String& expression,
                               const GroveLib::NodeWithNamespace* resolver,
                               const Xpath::FunctionFactory* fact)
{
    Xpath::XpathFunctionFactory xpathFact;
    return PatternExprImpl::makePattern(expression, resolver,
        fact ? fact : &xpathFact).pointer();    
}

PatternExprImplPtr 
PatternExprImpl::makePattern(const COMMON_NS::String& expression,
                             const GroveLib::NodeWithNamespace* resolver,
                             const Xpath::FunctionFactory* fact)
{
    ParserArgs pa(expression, resolver, fact);
    pa.exprRoot = new AltPatternExpr;
    yyparse(&pa);
    if (pa.exprRoot->alternatives().size() == 1)
        return pa.exprRoot->alternatives()[0].pointer();
    return pa.exprRoot.pointer();
}

Xpath::ExprPtr 
PatternExpr::makeExpr(const COMMON_NS::String& expression,
                      const GroveLib::NodeWithNamespace* resolver,
                      const Xpath::FunctionFactory* fact)
{
    XsltFunctionFactory xff;
    return Xpath::Engine::makeExpr(expression, resolver, fact ? fact : &xff);
}

Xpath::ExprImplPtr 
PatternExprImpl::makeExpr(const COMMON_NS::String& expression,
                          const GroveLib::NodeWithNamespace* resolver,
                          const Xpath::FunctionFactory* fact)
{
    XsltFunctionFactory xff;
    Xpath::XpathTokenizer xpt(expression);
    return xpt.makeExpr(resolver, fact ? fact : &xff);
}

static int xpp_lex(YYSTYPE *lvalp, XsltTokenizer& lexerInput)
{
    return lexerInput.getToken(lvalp);    
}

static void yyprint(FILE* file, int type, TokenValue value)
{
    switch(type) {
        case XT_LITERAL:
        case XT_NCNAME:
        case XT_AXISNAME:
            fprintf(file, "#%s<%s> ", yytname[YYTRANSLATE(type)], 
                value.asString().utf8().c_str());
            break;
        default:
            fprintf(file, "%s ", yytname[YYTRANSLATE(type)]);
    }
}

void xpp_error(const char* s)
{
    throw Xslt::Exception(XsltMessages::parserError, s);
}

} // namespace Xslt
