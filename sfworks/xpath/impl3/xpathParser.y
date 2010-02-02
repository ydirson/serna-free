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
#include "xpath/xpath_defs.h"
#include "common/String.h"
#include "common/PathName.h"
#include "grove/XmlName.h"
#include "grove/NodeWithNamespace.h"
#include "grove/NodeExt.h"

#include "xpath/Value.h" 
#include "xpath/Engine.h"
#include "xpath/Exception.h"
#include "xpath/impl3/StringValue.h" 
#include "xpath/impl3/NumericValue.h" 
#include "xpath/impl3/expressions/BinaryExpr.h"
#include "xpath/impl3/expressions/UnaryExpr.h"
#include "xpath/impl3/expressions/ConstExpr.h"
#include "xpath/impl3/expressions/VarExpr.h"
#include "xpath/impl3/expressions/FunctionExpr.h"
#include "xpath/impl3/expressions/AxisExpr.h"
#include "xpath/impl3/expressions/SlashExpr.h"
#include "xpath/impl3/expressions/PredicateExpr.h"
#include "xpath/impl3/expressions/NodeTestExpr.h"
#include "xpath/impl3/Tokenizer.h"
#include "xpath/XpathFunctionFactory.h"
#include "xpath/impl3/debug.h"

#include <stdlib.h>
#include <stdio.h>

USING_COMMON_NS;

namespace Xpath {

// Argument which are passed to the parser. 
struct ParserArgs {
    XpathTokenizer& input;
    ExprImplPtr            exprRoot;
    const FunctionFactory* funcFactory;
    const Engine::NsNode*  nsResolver;
    
    // This is used for lifetime tracking, because parser itself cannot
    // delete nonterminals in case of error
    Vector<RefCntPtr<ExprImpl> > exprHolderVec;
    Vector<RefCntPtr<PQname> > qnameHolderVec;
    
    ParserArgs(XpathTokenizer& t)
        : input(t)
    {
        exprHolderVec.reserve(64);
        qnameHolderVec.reserve(64);
    }
};

static ExprImpl* make2SlashExpr(ExprImpl* left, ExprImpl* right);

// parameters for bison skeleton parser
#define YYSTYPE       TokenValue
#define YYLEX_PARAM   ((ParserArgs*)pargs)->input
#define YYPARSE_PARAM pargs
#define YYERROR_VERBOSE
#define YYPRINT(file, type, value) yyprint(file, type, value)
#define NEW_EXPR(e) \
    ((ParserArgs*)pargs)->exprHolderVec.push_back(e)->pointer()
#define NEW_QNAME(n) \
    ((ParserArgs*)pargs)->qnameHolderVec.push_back(n)->pointer()
#define FUNC_FACTORY  ((ParserArgs*)pargs)->funcFactory
%}

%pure_parser            // make parser reentrant
%name_prefix="xpp_"     // prefix for generated parser funcitons
%debug                  // include debug information
%output="xpathParser.cpp" // output file name
%defines                // generate also xpathParser.hxx

%token  FUNCTIONNAME
%token  LITERAL
%token  TNUMBER
%token  NCNAME
%token  AXISNAME

// Operators are here because tokenizer must know about them.
%token OPAND
%token OPOR
%token OPMOD
%token OPDIV
%token OPMUL
%token OPUNION
%token OPSLASH
%token OP2SLASH
%token OPPLUS
%token OPMINUS
%token OPEQ
%token OPNEQ
%token OPLT
%token OPLTEQ
%token OPGT
%token OPGTEQ
%token OPDCOLON
%token T2DOT

// Node tests
%token NTEST_COMMENT
%token NTEST_NODE
%token NTEST_TEXT
%token NTEST_PI

%{
void xpp_error(const char* s);
static int  xpp_lex(YYSTYPE *lvalp, XpathTokenizer& lexerInput);
static void yyprint (FILE *file, int type, TokenValue value);
%}

%%

xpathExpression: expr
                {
                    ((ParserArgs*)pargs)->exprRoot = $<expr>1;
                }
                ;

locationPath:   relativeLocationPath     
                | absoluteLocationPath 
                ;

absoluteLocationPath: OPSLASH               
                    {
                        $<expr>$ = NEW_EXPR(new AxisRootExpr); 
                    }
                | OPSLASH relativeLocationPath
                    {
                        $<expr>$ = NEW_EXPR(new SlashExpr
                            (new AxisRootExpr, $<expr>2));
                    }
                | abbreviatedAbsoluteLocationPath
                ;

relativeLocationPath: step
                | relativeLocationPath OPSLASH step
                    {
                        $<expr>$ = NEW_EXPR(new SlashExpr($<expr>1, $<expr>3));
                    }
                | abbreviatedRelativeLocationPath
                ;
                
step:           axisExpr
                | axisPredList
                | abbreviatedStep
                ;

axisPredPair:   axisExpr predicate
                    {
                        $<expr>$ = 
                            NEW_EXPR(new PredicateExpr($<expr>1, $<expr>2));    
                    }
                ;
                
axisPredList:   axisPredPair
                | axisPredList predicate
                    {
                        $<expr>$ = 
                            NEW_EXPR(new PredicateExpr($<expr>1, $<expr>2));    
                    }
                ;

abbreviatedAbsoluteLocationPath: OP2SLASH relativeLocationPath
                    {
                        $<expr>$ = NEW_EXPR(make2SlashExpr
                            (new AxisRootExpr, $<expr>2));
                    }
                ;

abbreviatedRelativeLocationPath: relativeLocationPath OP2SLASH step
                    {
                        $<expr>$ = NEW_EXPR(make2SlashExpr($<expr>1, $<expr>3));
                    }
                ;

abbreviatedStep: '.'
                    {
                        $<expr>$ = NEW_EXPR(new AxisSelfExpr);
                    }
                | T2DOT
                    {
                        $<expr>$ = NEW_EXPR(new AxisParentExpr);
                    }
                ;

axisExpr:       AXISNAME OPDCOLON nodeTest   
                    {
                        ExprImplPtr ep = NEW_EXPR(AxisExpr::make($<asString()>1,
                            static_cast<Xpath::NodeTestExpr*>($<expr>3)));
                        if (ep.isNull()) {
                            throw Xpath::Exception(XpathMessages::parserBadAxis);
                            YYABORT;
                        }
                        $<expr>$ = ep.pointer();
                    }
                | '@' nodeTest
                    {
                        $<expr>$ = NEW_EXPR(new AxisAttributeExpr
                            (static_cast<Xpath::NodeTestExpr*>($<expr>2)));
                    }
                | nodeTest
                    {
                        $<expr>$ = NEW_EXPR(new AxisChildExpr
                            (static_cast<Xpath::NodeTestExpr*>($<expr>1)));
                    }
                ;

nodeTest:       nameTest 
                    {
                        $<expr>$ = NEW_EXPR(new NameNodeTestExpr(*$<qname>1,
                            ((ParserArgs*)pargs)->nsResolver));
                    }
                | NTEST_NODE '(' ')'  
                    {
                        $<expr>$ = NEW_EXPR(new NodeNodeTestExpr);
                    }
                | NTEST_COMMENT '(' ')'   
                    {
                        $<expr>$ = NEW_EXPR(new CommentNodeTestExpr);
                    }
                | NTEST_TEXT '(' ')' 
                    {
                        $<expr>$ = NEW_EXPR(new TextNodeTestExpr);
                    }
                | NTEST_PI   '(' LITERAL ')'
                    {
                        $<expr>$ = 
                            NEW_EXPR(PiNodeTestExpr::make($<asString()>3));
                    }
                | NTEST_PI '(' ')' 
                    {
                        $<expr>$ = NEW_EXPR(new PiNodeTestExpr(String::null()));
                    }
                ;

nameTest:       '*'               
                    { 
                        $<qname>$ = NEW_QNAME(new PQname("*")); 
                    }
                | NCNAME ':' '*'  
                    { 
                        $<qname>$ = NEW_QNAME(new PQname("*", $<asString()>1));
                    }
                | qname            
                ;    

qname:          NCNAME  
                    {
                        $<qname>$ = NEW_QNAME(new PQname($<asString()>1));
                    }
                | NCNAME ':' NCNAME  
                    {
                        $<qname>$ = 
                            NEW_QNAME(new PQname($<asString()>3, $<asString()>1));
                    }
                ;

predicate:      '[' expr ']'
                    {
                        $<expr>$ = NEW_EXPR($<expr>2);
                    }
                ;

expr:           orExpr
                ;

primaryExpr:    variableReference 
                | '(' expr ')'
                    {
                        $<expr>$ = $<expr>2;
                    }
                | LITERAL
                    {
                        $<expr>$ = NEW_EXPR(new ConstExpr
                            (new StringValue($<asString()>1))); 
                    }
                | TNUMBER  
                    {
                        $<expr>$ = NEW_EXPR(new ConstExpr
                            (new NumericValue($<nval>1)));
                    }
                | functionCall
                ;

variableReference: '$' qname  
                {
                    $<expr>$ = NEW_EXPR(new VarExpr($<qname>2->asString())); 
                }
                ;

functionCall:   FUNCTIONNAME '(' functionArgumentList ')' 
                    {
                        ExprImplPtr av = $<expr>3;
                        FunctionExpr* res = FUNC_FACTORY->makeFunction
                            ($<asString()>1, av.isNull() ? 0 :
                                static_cast<FunctionArgExpr*>(av.pointer()),
                                ((ParserArgs*)pargs)->nsResolver);
                        if (0 == res) {
                           throw Xpath::Exception(XpathMessages::parserUknFunc, $<asString()>1);
                        }
                        $<expr>$ = NEW_EXPR(res);
                    }
                ;

functionArgumentList: expr 
                    {
                        $<expr>$ = NEW_EXPR(new FunctionArgExpr($<expr>1, 0));
                    }
                | expr ',' functionArgumentList
                    {
                        $<expr>$ = NEW_EXPR(new FunctionArgExpr($<expr>1, 
                            static_cast<FunctionArgExpr*>($<expr>3)));
                    }
                |   // if nothing
                    {
                        $<expr>$ = 0;
                    }
                ;

unionExpr:      pathExpr 
                | unionExpr OPUNION pathExpr
                    {
                        $<expr>$ = NEW_EXPR(new BinaryBarExpr($<expr>1, 
                            $<expr>3));
                    }
                ;

pathExpr:       locationPath
                | filterExpr
                | filterExpr OP2SLASH relativeLocationPath
                    {
                        $<expr>$ = NEW_EXPR(make2SlashExpr($<expr>1, $<expr>3));
                    }
                | filterExpr OPSLASH relativeLocationPath
                    {
                        $<expr>$ = NEW_EXPR(new SlashExpr($<expr>1, $<expr>3));
                    }
                ;

filterExpr:     primaryExpr
                | filterExpr predicate
                    {
                        $<expr>$ = 
                            NEW_EXPR(new PredicateExpr($<expr>1, $<expr>2));
                    }
                ;

orExpr:         andExpr
                | orExpr OPOR andExpr 
                    {
                        $<expr>$ = NEW_EXPR(new BinaryOrExpr($<expr>1,
                            $<expr>3));
                    }
                ;

andExpr:        equalityExpr
                | andExpr OPAND equalityExpr
                    {
                        $<expr>$ = NEW_EXPR(new BinaryAndExpr($<expr>1, 
                            $<expr>3));
                    }
                ;

equalityExpr:   relationalExpr
                | equalityExpr OPEQ  relationalExpr
                    {
                        $<expr>$ = NEW_EXPR(new BinaryEqExpr($<expr>1, 
                            $<expr>3));
                    }
                | equalityExpr OPNEQ relationalExpr 
                    {
                        $<expr>$ = NEW_EXPR(new BinaryNotEqExpr($<expr>1,
                            $<expr>3));
                    }
                ;

relationalExpr: additiveExpr
                | relationalExpr OPLT   additiveExpr 
                    {
                        $<expr>$ = NEW_EXPR(new BinaryLtExpr($<expr>1,
                            $<expr>3));
                    }
                | relationalExpr OPGT   additiveExpr 
                    {
                        $<expr>$ = NEW_EXPR(new BinaryGtExpr($<expr>1,
                            $<expr>3));
                    }
                | relationalExpr OPLTEQ additiveExpr 
                    {
                        $<expr>$ = NEW_EXPR(new BinaryLtEqExpr($<expr>1,
                            $<expr>3));
                    }
                | relationalExpr OPGTEQ additiveExpr 
                    {
                        $<expr>$ = NEW_EXPR(new BinaryGtEqExpr($<expr>1, 
                            $<expr>3));
                    }
                ;

additiveExpr:   multiplicativeExpr
                | additiveExpr OPPLUS  multiplicativeExpr 
                    {
                        $<expr>$ = NEW_EXPR(new BinaryAddExpr($<expr>1,
                            $<expr>3));
                    }
                | additiveExpr OPMINUS multiplicativeExpr 
                    {
                        $<expr>$ = NEW_EXPR(new BinarySubExpr($<expr>1,
                            $<expr>3));
                    }
                ;

multiplicativeExpr: unaryExpr
                | multiplicativeExpr OPMUL unaryExpr 
                    {
                        $<expr>$ = NEW_EXPR(new BinaryMulExpr($<expr>1,
                            $<expr>3));
                    }
                | multiplicativeExpr OPDIV unaryExpr 
                    {
                        $<expr>$ = NEW_EXPR(new BinaryDivExpr($<expr>1,
                            $<expr>3));
                    }
                | multiplicativeExpr OPMOD unaryExpr 
                    {
                        $<expr>$ = NEW_EXPR(new BinaryModExpr($<expr>1, 
                            $<expr>3));
                    }
                ;

unaryExpr:      unionExpr
                | OPMINUS unaryExpr 
                    {
                        $<expr>$ = NEW_EXPR(new UnaryExpr
                            ($<expr>2, UnaryExpr::MINUS));
                    }
                ;
%%

static ExprImpl* make2SlashExpr(ExprImpl* left, ExprImpl* right)
{
    AxisExpr* childaxis = 0;
    if (right->type() == Expr::AXIS) {
        childaxis = static_cast<AxisExpr*>(right);
        if (childaxis->name() != AxisExpr::CHILD)
            childaxis = 0;
    }
    if (childaxis)
        return new SlashExpr(left, 
            (new AxisDosExpr(childaxis->nodeTestExpr())));
    else
        return new SlashExpr(new SlashExpr(left, new AxisDosExpr), right);
}

/////////////////////////////////////////////////////////////////////

static int xpp_lex(YYSTYPE *lvalp, XpathTokenizer& lexerInput)
{
    return lexerInput.getToken(lvalp);    
}

static void yyprint(FILE* file, int type, TokenValue value)
{
    switch(type) {
        case FUNCTIONNAME:
        case LITERAL:
        case NCNAME:
        case AXISNAME:
            fprintf(file, "#%s<%s> ", yytname[YYTRANSLATE(type)], 
                value.asString().utf8().c_str());
            break;
        case TNUMBER:
            fprintf(file, "#%s<%f> ", yytname[YYTRANSLATE(type)], value.nval);
            break;
        default:
            fprintf(file, "%s ", yytname[YYTRANSLATE(type)]);
    }
}

void xpp_error(const char* s)
{
    throw Xpath::Exception(XpathMessages::parserError, s);
}

RefCntPtr<ExprImpl> XpathTokenizer::makeExpr(const Engine::NsNode* nsResolver,
                                             const FunctionFactory* fact)
{
    try {
        ParserArgs pa(*this);
        pa.nsResolver = nsResolver;
        if (!fact) {
            XpathFunctionFactory xff;
            pa.funcFactory = &xff;
            yyparse(&pa);
        } else {
            pa.funcFactory = fact;
            yyparse(&pa);
        }
        RefCntPtr<ExprImpl> expr;
        const int oflags = fact 
            ? fact->optimizationFlags() : FunctionFactory::OPT_NORMAL;
        if (oflags)
            expr = pa.exprRoot->optimize(oflags);
        else
            expr = pa.exprRoot;
        ExprImpl* eimpl = expr.pointer();
        expr->makeNodeTestChain(eimpl);
        return expr.pointer();
    } 
    catch (Xpath::Exception& e) {
        if (!nsResolver || !nsResolver->grove())
            throw;
        String line, column;
        if (nsResolver->nodeExt() &&
            nsResolver->nodeExt()->asLineLocExt()) {
            GroveLib::LineLocExt* ext = nsResolver->nodeExt()->asLineLocExt();
            line = String::number(ext->line() + 1);
            column = String::number(ext->column() + 1);
        }
        throw Xpath::Exception(XpathMessages::parserException, 
            e.whatString(), 
            PathName(nsResolver->grove()->topSysid()).absolute().name(),
            line, column, nsResolver->nodeName()); 
    }
    catch (...) {
        throw;
    }
    return 0;
}

namespace Engine {

ExprPtr makeExpr(const String& expression, 
                 const NsNode* nsResolver,
                 const FunctionFactory* fact)
{
    XpathTokenizer xpt(expression);
    return xpt.makeExpr(nsResolver, fact).pointer();
}

} // namespace Engine

} // namespace Xpath


