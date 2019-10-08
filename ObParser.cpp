

// This file was automatically generated by Coco/R; don't modify it.
#include "ObParser.h"
#include "ObErrors.h"
#include <QtDebug>
#include <QFileInfo>

namespace Ob {


static QString coco_string_create( const wchar_t* str )
{
    return QString::fromStdWString(str);
}

int Parser::peek( quint8 la )
{
	if( la == 0 )
		return d_cur.d_type;
	else if( la == 1 )
		return d_next.d_type;
	else
		return scanner->peekToken( la - 1 ).d_type;
}


void Parser::SynErr(int n, const char* ctx) {
    if (errDist >= minErrDist)
    {
       SynErr(d_next.d_sourcePath,d_next.d_lineNr, d_next.d_colNr, n, errors, ctx);
    }
	errDist = 0;
}

void Parser::SemErr(const char* msg) {
	if (errDist >= minErrDist) errors->error(Ob::Errors::Semantics,d_cur.d_sourcePath,d_cur.d_lineNr, d_cur.d_colNr, msg);
	errDist = 0;
}

void Parser::Get() {
	for (;;) {
		d_cur = d_next;
		d_next = scanner->nextToken();
        bool deliverToParser = false;
        switch( d_next.d_type )
        {
        case Ob::Tok_Invalid:
        	if( !d_next.d_val.isEmpty() )
            	SynErr( d_next.d_type, d_next.d_val );
            // else errors already handeled in lexer
            break;
        case Ob::Tok_Comment:
            d_comments.append(d_next);
            break;
        default:
            deliverToParser = true;
            break;
        }

        if( deliverToParser )
        {
            if( d_next.d_type == Ob::Tok_Eof )
                d_next.d_type = _EOF;

            la->kind = d_next.d_type;
            if (la->kind <= maxT)
            {
                ++errDist;
                break;
            }
        }

		d_next = d_cur;
	}
}

void Parser::Expect(int n, const char* ctx ) {
	if (la->kind==n) Get(); else { SynErr(n, ctx); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::Oberon() {
		d_stack.push(&d_root); 
		if (la->kind == _T_MODULE) {
			module();
		} else if (la->kind == _T_DEFINITION) {
			definition();
		} else SynErr(77,__FUNCTION__);
		d_stack.pop(); 
}

void Parser::module() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_module, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_MODULE,__FUNCTION__);
		addTerminal(); 
		Expect(_T_ident,__FUNCTION__);
		addTerminal(); 
		Expect(_T_Semi,__FUNCTION__);
		addTerminal(); 
		if (la->kind == _T_IMPORT) {
			ImportList();
		}
		DeclarationSequence();
		if (la->kind == _T_BEGIN) {
			Get();
			addTerminal(); 
			StatementSequence();
		}
		Expect(_T_END,__FUNCTION__);
		addTerminal(); 
		Expect(_T_ident,__FUNCTION__);
		addTerminal(); 
		Expect(_T_Dot,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::definition() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_definition, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_DEFINITION,__FUNCTION__);
		addTerminal(); 
		Expect(_T_ident,__FUNCTION__);
		addTerminal(); 
		Expect(_T_Semi,__FUNCTION__);
		addTerminal(); 
		if (la->kind == _T_IMPORT) {
			ImportList();
		}
		DeclarationSequence2();
		Expect(_T_END,__FUNCTION__);
		addTerminal(); 
		Expect(_T_ident,__FUNCTION__);
		addTerminal(); 
		Expect(_T_Dot,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::number() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_number, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (la->kind == _T_integer) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_real) {
			Get();
			addTerminal(); 
		} else SynErr(78,__FUNCTION__);
		d_stack.pop(); 
}

void Parser::qualident() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_qualident, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (peek(1) == _T_ident && peek(2) == _T_Dot ) {
			Expect(_T_ident,__FUNCTION__);
			addTerminal(); 
			Expect(_T_Dot,__FUNCTION__);
			addTerminal(); 
		}
		Expect(_T_ident,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::identdef() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_identdef, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_ident,__FUNCTION__);
		addTerminal(); 
		if (la->kind == _T_Star) {
			Get();
			addTerminal(); 
		}
		d_stack.pop(); 
}

void Parser::ConstDeclaration() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_ConstDeclaration, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		identdef();
		Expect(_T_Eq,__FUNCTION__);
		addTerminal(); 
		ConstExpression();
		d_stack.pop(); 
}

void Parser::ConstExpression() {
		expression();
}

void Parser::expression() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_expression, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		SimpleExpression();
		if (StartOf(1)) {
			relation();
			SimpleExpression();
		}
		d_stack.pop(); 
}

void Parser::TypeDeclaration() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_TypeDeclaration, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		identdef();
		Expect(_T_Eq,__FUNCTION__);
		addTerminal(); 
		type();
		d_stack.pop(); 
}

void Parser::type() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_type, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (la->kind == _T_ident) {
			qualident();
		} else if (la->kind == _T_ARRAY) {
			ArrayType();
		} else if (la->kind == _T_RECORD) {
			RecordType();
		} else if (la->kind == _T_POINTER) {
			PointerType();
		} else if (la->kind == _T_PROCEDURE) {
			ProcedureType();
		} else SynErr(79,__FUNCTION__);
		d_stack.pop(); 
}

void Parser::ArrayType() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_ArrayType, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_ARRAY,__FUNCTION__);
		addTerminal(); 
		LengthList();
		Expect(_T_OF,__FUNCTION__);
		addTerminal(); 
		type();
		d_stack.pop(); 
}

void Parser::RecordType() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_RecordType, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_RECORD,__FUNCTION__);
		addTerminal(); 
		if (la->kind == _T_Lpar) {
			Get();
			addTerminal(); 
			BaseType();
			Expect(_T_Rpar,__FUNCTION__);
			addTerminal(); 
		}
		if (la->kind == _T_ident) {
			FieldListSequence();
		}
		Expect(_T_END,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::PointerType() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_PointerType, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_POINTER,__FUNCTION__);
		addTerminal(); 
		Expect(_T_TO,__FUNCTION__);
		addTerminal(); 
		type();
		d_stack.pop(); 
}

void Parser::ProcedureType() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_ProcedureType, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_PROCEDURE,__FUNCTION__);
		addTerminal(); 
		if (la->kind == _T_Lpar) {
			FormalParameters();
		}
		d_stack.pop(); 
}

void Parser::LengthList() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_LengthList, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		length();
		while (la->kind == _T_Comma) {
			Get();
			addTerminal(); 
			length();
		}
		d_stack.pop(); 
}

void Parser::length() {
		ConstExpression();
}

void Parser::BaseType() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_BaseType, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		qualident();
		d_stack.pop(); 
}

void Parser::FieldListSequence() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_FieldListSequence, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		FieldList();
		while (la->kind == _T_Semi) {
			Get();
			addTerminal(); 
			if (la->kind == _T_ident) {
				FieldList();
			}
		}
		d_stack.pop(); 
}

void Parser::FieldList() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_FieldList, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		IdentList();
		Expect(_T_Colon,__FUNCTION__);
		addTerminal(); 
		type();
		d_stack.pop(); 
}

void Parser::IdentList() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_IdentList, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		identdef();
		while (la->kind == _T_Comma) {
			Get();
			addTerminal(); 
			identdef();
		}
		d_stack.pop(); 
}

void Parser::FormalParameters() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_FormalParameters, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_Lpar,__FUNCTION__);
		addTerminal(); 
		if (la->kind == _T_VAR || la->kind == _T_ident) {
			FPSection();
			while (la->kind == _T_Semi) {
				Get();
				addTerminal(); 
				FPSection();
			}
		}
		Expect(_T_Rpar,__FUNCTION__);
		addTerminal(); 
		if (la->kind == _T_Colon) {
			Get();
			addTerminal(); 
			qualident();
		}
		d_stack.pop(); 
}

void Parser::VariableDeclaration() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_VariableDeclaration, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		IdentList();
		Expect(_T_Colon,__FUNCTION__);
		addTerminal(); 
		type();
		d_stack.pop(); 
}

void Parser::designator() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_designator, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		qualident();
		while (StartOf(2)) {
			selector();
		}
		d_stack.pop(); 
}

void Parser::selector() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_selector, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (la->kind == _T_Dot) {
			Get();
			addTerminal(); 
			Expect(_T_ident,__FUNCTION__);
			addTerminal(); 
		} else if (la->kind == _T_Lbrack) {
			Get();
			addTerminal(); 
			ExpList();
			Expect(_T_Rbrack,__FUNCTION__);
			addTerminal(); 
		} else if (la->kind == _T_Hat) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_Lpar) {
			Get();
			addTerminal(); 
			if (StartOf(3)) {
				ExpList();
			}
			Expect(_T_Rpar,__FUNCTION__);
			addTerminal(); 
		} else SynErr(80,__FUNCTION__);
		d_stack.pop(); 
}

void Parser::ExpList() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_ExpList, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		expression();
		while (la->kind == _T_Comma) {
			Get();
			addTerminal(); 
			expression();
		}
		d_stack.pop(); 
}

void Parser::SimpleExpression() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_SimpleExpression, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (la->kind == _T_Plus || la->kind == _T_Minus) {
			if (la->kind == _T_Plus) {
				Get();
				addTerminal(); 
			} else {
				Get();
				addTerminal(); 
			}
		}
		term();
		while (la->kind == _T_Plus || la->kind == _T_Minus || la->kind == _T_OR) {
			AddOperator();
			term();
		}
		d_stack.pop(); 
}

void Parser::relation() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_relation, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		switch (la->kind) {
		case _T_Eq: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_Hash: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_Lt: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_Leq: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_Gt: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_Geq: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_IN: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_IS: {
			Get();
			addTerminal(); 
			break;
		}
		default: SynErr(81,__FUNCTION__); break;
		}
		d_stack.pop(); 
}

void Parser::term() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_term, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		factor();
		while (StartOf(4)) {
			MulOperator();
			factor();
		}
		d_stack.pop(); 
}

void Parser::AddOperator() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_AddOperator, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (la->kind == _T_Plus) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_Minus) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_OR) {
			Get();
			addTerminal(); 
		} else SynErr(82,__FUNCTION__);
		d_stack.pop(); 
}

void Parser::factor() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_factor, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		switch (la->kind) {
		case _T_integer: case _T_real: {
			number();
			break;
		}
		case _T_string: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_hexstring: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_hexchar: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_NIL: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_TRUE: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_FALSE: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_Lbrace: {
			set();
			break;
		}
		case _T_ident: {
			variableOrFunctionCall();
			break;
		}
		case _T_Lpar: {
			Get();
			addTerminal(); 
			expression();
			Expect(_T_Rpar,__FUNCTION__);
			addTerminal(); 
			break;
		}
		case _T_Tilde: {
			Get();
			addTerminal(); 
			factor();
			break;
		}
		default: SynErr(83,__FUNCTION__); break;
		}
		d_stack.pop(); 
}

void Parser::MulOperator() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_MulOperator, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (la->kind == _T_Star) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_Slash) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_DIV) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_MOD) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_Amp) {
			Get();
			addTerminal(); 
		} else SynErr(84,__FUNCTION__);
		d_stack.pop(); 
}

void Parser::set() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_set, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_Lbrace,__FUNCTION__);
		addTerminal(); 
		if (StartOf(3)) {
			element();
			while (la->kind == _T_Comma) {
				Get();
				addTerminal(); 
				element();
			}
		}
		Expect(_T_Rbrace,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::variableOrFunctionCall() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_variableOrFunctionCall, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		designator();
		d_stack.pop(); 
}

void Parser::element() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_element, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		expression();
		if (la->kind == _T_2Dot) {
			Get();
			addTerminal(); 
			expression();
		}
		d_stack.pop(); 
}

void Parser::statement() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_statement, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (StartOf(5)) {
			switch (la->kind) {
			case _T_ident: {
				assignmentOrProcedureCall();
				break;
			}
			case _T_IF: {
				IfStatement();
				break;
			}
			case _T_CASE: {
				CaseStatement();
				break;
			}
			case _T_WHILE: {
				WhileStatement();
				break;
			}
			case _T_REPEAT: {
				RepeatStatement();
				break;
			}
			case _T_FOR: {
				ForStatement();
				break;
			}
			}
		}
		d_stack.pop(); 
}

void Parser::assignmentOrProcedureCall() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_assignmentOrProcedureCall, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		designator();
		if (la->kind == _T_ColonEq) {
			Get();
			addTerminal(); 
			expression();
		}
		d_stack.pop(); 
}

void Parser::IfStatement() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_IfStatement, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_IF,__FUNCTION__);
		addTerminal(); 
		expression();
		Expect(_T_THEN,__FUNCTION__);
		addTerminal(); 
		StatementSequence();
		while (la->kind == _T_ELSIF) {
			ElsifStatement();
		}
		if (la->kind == _T_ELSE) {
			ElseStatement();
		}
		Expect(_T_END,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::CaseStatement() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_CaseStatement, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_CASE,__FUNCTION__);
		addTerminal(); 
		expression();
		Expect(_T_OF,__FUNCTION__);
		addTerminal(); 
		Case();
		while (la->kind == _T_Bar) {
			Get();
			addTerminal(); 
			Case();
		}
		Expect(_T_END,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::WhileStatement() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_WhileStatement, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_WHILE,__FUNCTION__);
		addTerminal(); 
		expression();
		Expect(_T_DO,__FUNCTION__);
		addTerminal(); 
		StatementSequence();
		while (la->kind == _T_ELSIF) {
			ElsifStatement2();
		}
		Expect(_T_END,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::RepeatStatement() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_RepeatStatement, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_REPEAT,__FUNCTION__);
		addTerminal(); 
		StatementSequence();
		Expect(_T_UNTIL,__FUNCTION__);
		addTerminal(); 
		expression();
		d_stack.pop(); 
}

void Parser::ForStatement() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_ForStatement, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_FOR,__FUNCTION__);
		addTerminal(); 
		Expect(_T_ident,__FUNCTION__);
		addTerminal(); 
		Expect(_T_ColonEq,__FUNCTION__);
		addTerminal(); 
		expression();
		Expect(_T_TO,__FUNCTION__);
		addTerminal(); 
		expression();
		if (la->kind == _T_BY) {
			Get();
			addTerminal(); 
			ConstExpression();
		}
		Expect(_T_DO,__FUNCTION__);
		addTerminal(); 
		StatementSequence();
		Expect(_T_END,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::StatementSequence() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_StatementSequence, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		statement();
		while (la->kind == _T_Semi) {
			Get();
			addTerminal(); 
			statement();
		}
		d_stack.pop(); 
}

void Parser::ElsifStatement() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_ElsifStatement, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_ELSIF,__FUNCTION__);
		addTerminal(); 
		expression();
		Expect(_T_THEN,__FUNCTION__);
		addTerminal(); 
		StatementSequence();
		d_stack.pop(); 
}

void Parser::ElseStatement() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_ElseStatement, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_ELSE,__FUNCTION__);
		addTerminal(); 
		StatementSequence();
		d_stack.pop(); 
}

void Parser::Case() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_Case, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (StartOf(6)) {
			CaseLabelList();
			Expect(_T_Colon,__FUNCTION__);
			addTerminal(); 
			StatementSequence();
		}
		d_stack.pop(); 
}

void Parser::CaseLabelList() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_CaseLabelList, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		LabelRange();
		while (la->kind == _T_Comma) {
			Get();
			addTerminal(); 
			LabelRange();
		}
		d_stack.pop(); 
}

void Parser::LabelRange() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_LabelRange, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		label();
		if (la->kind == _T_2Dot) {
			Get();
			addTerminal(); 
			label();
		}
		d_stack.pop(); 
}

void Parser::label() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_label, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (la->kind == _T_integer) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_string) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_hexchar) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_hexstring) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_ident) {
			qualident();
		} else SynErr(85,__FUNCTION__);
		d_stack.pop(); 
}

void Parser::ElsifStatement2() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_ElsifStatement2, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_ELSIF,__FUNCTION__);
		addTerminal(); 
		expression();
		Expect(_T_DO,__FUNCTION__);
		addTerminal(); 
		StatementSequence();
		d_stack.pop(); 
}

void Parser::ProcedureDeclaration() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_ProcedureDeclaration, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		ProcedureHeading();
		Expect(_T_Semi,__FUNCTION__);
		addTerminal(); 
		ProcedureBody();
		Expect(_T_ident,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::ProcedureHeading() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_ProcedureHeading, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_PROCEDURE,__FUNCTION__);
		addTerminal(); 
		identdef();
		if (la->kind == _T_Lpar) {
			FormalParameters();
		}
		d_stack.pop(); 
}

void Parser::ProcedureBody() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_ProcedureBody, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		DeclarationSequence();
		if (la->kind == _T_BEGIN) {
			Get();
			addTerminal(); 
			StatementSequence();
		}
		if (la->kind == _T_RETURN) {
			ReturnStatement();
		}
		Expect(_T_END,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::DeclarationSequence() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_DeclarationSequence, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (la->kind == _T_CONST) {
			Get();
			addTerminal(); 
			while (la->kind == _T_ident) {
				ConstDeclaration();
				Expect(_T_Semi,__FUNCTION__);
				addTerminal(); 
			}
		}
		if (la->kind == _T_TYPE) {
			Get();
			addTerminal(); 
			while (la->kind == _T_ident) {
				TypeDeclaration();
				Expect(_T_Semi,__FUNCTION__);
				addTerminal(); 
			}
		}
		if (la->kind == _T_VAR) {
			Get();
			addTerminal(); 
			while (la->kind == _T_ident) {
				VariableDeclaration();
				Expect(_T_Semi,__FUNCTION__);
				addTerminal(); 
			}
		}
		while (la->kind == _T_PROCEDURE) {
			ProcedureDeclaration();
			Expect(_T_Semi,__FUNCTION__);
			addTerminal(); 
		}
		d_stack.pop(); 
}

void Parser::ReturnStatement() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_ReturnStatement, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_RETURN,__FUNCTION__);
		addTerminal(); 
		expression();
		d_stack.pop(); 
}

void Parser::DeclarationSequence2() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_DeclarationSequence2, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (la->kind == _T_CONST) {
			Get();
			addTerminal(); 
			while (la->kind == _T_ident) {
				ConstDeclaration();
				Expect(_T_Semi,__FUNCTION__);
				addTerminal(); 
			}
		}
		if (la->kind == _T_TYPE) {
			Get();
			addTerminal(); 
			while (la->kind == _T_ident) {
				TypeDeclaration();
				Expect(_T_Semi,__FUNCTION__);
				addTerminal(); 
			}
		}
		if (la->kind == _T_VAR) {
			Get();
			addTerminal(); 
			while (la->kind == _T_ident) {
				VariableDeclaration();
				Expect(_T_Semi,__FUNCTION__);
				addTerminal(); 
			}
		}
		while (la->kind == _T_PROCEDURE) {
			ProcedureHeading();
			Expect(_T_Semi,__FUNCTION__);
			addTerminal(); 
		}
		d_stack.pop(); 
}

void Parser::FPSection() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_FPSection, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (la->kind == _T_VAR) {
			Get();
			addTerminal(); 
		}
		Expect(_T_ident,__FUNCTION__);
		addTerminal(); 
		while (la->kind == _T_Comma) {
			Get();
			addTerminal(); 
			Expect(_T_ident,__FUNCTION__);
			addTerminal(); 
		}
		Expect(_T_Colon,__FUNCTION__);
		addTerminal(); 
		FormalType();
		d_stack.pop(); 
}

void Parser::FormalType() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_FormalType, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		while (la->kind == _T_ARRAY) {
			Get();
			addTerminal(); 
			Expect(_T_OF,__FUNCTION__);
			addTerminal(); 
		}
		qualident();
		d_stack.pop(); 
}

void Parser::ImportList() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_ImportList, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_IMPORT,__FUNCTION__);
		addTerminal(); 
		import();
		while (la->kind == _T_Comma) {
			Get();
			addTerminal(); 
			import();
		}
		Expect(_T_Semi,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::import() {
		Ob::SynTree* n = new Ob::SynTree( Ob::SynTree::R_import, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_ident,__FUNCTION__);
		addTerminal(); 
		if (la->kind == _T_ColonEq) {
			Get();
			addTerminal(); 
			Expect(_T_ident,__FUNCTION__);
			addTerminal(); 
		}
		d_stack.pop(); 
}




// If the user declared a method Init and a mehtod Destroy they should
// be called in the contructur and the destructor respctively.
//
// The following templates are used to recognize if the user declared
// the methods Init and Destroy.

template<typename T>
struct ParserInitExistsRecognizer {
	template<typename U, void (U::*)() = &U::Init>
	struct ExistsIfInitIsDefinedMarker{};

	struct InitIsMissingType {
		char dummy1;
	};
	
	struct InitExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static InitIsMissingType is_here(...);

	// exist only if ExistsIfInitIsDefinedMarker is defined
	template<typename U>
	static InitExistsType is_here(ExistsIfInitIsDefinedMarker<U>*);

	enum { InitExists = (sizeof(is_here<T>(NULL)) == sizeof(InitExistsType)) };
};

template<typename T>
struct ParserDestroyExistsRecognizer {
	template<typename U, void (U::*)() = &U::Destroy>
	struct ExistsIfDestroyIsDefinedMarker{};

	struct DestroyIsMissingType {
		char dummy1;
	};
	
	struct DestroyExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static DestroyIsMissingType is_here(...);

	// exist only if ExistsIfDestroyIsDefinedMarker is defined
	template<typename U>
	static DestroyExistsType is_here(ExistsIfDestroyIsDefinedMarker<U>*);

	enum { DestroyExists = (sizeof(is_here<T>(NULL)) == sizeof(DestroyExistsType)) };
};

// The folloing templates are used to call the Init and Destroy methods if they exist.

// Generic case of the ParserInitCaller, gets used if the Init method is missing
template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
struct ParserInitCaller {
	static void CallInit(T *t) {
		// nothing to do
	}
};

// True case of the ParserInitCaller, gets used if the Init method exists
template<typename T>
struct ParserInitCaller<T, true> {
	static void CallInit(T *t) {
		t->Init();
	}
};

// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
struct ParserDestroyCaller {
	static void CallDestroy(T *t) {
		// nothing to do
	}
};

// True case of the ParserDestroyCaller, gets used if the Destroy method exists
template<typename T>
struct ParserDestroyCaller<T, true> {
	static void CallDestroy(T *t) {
		t->Destroy();
	}
};

void Parser::Parse() {
	d_cur = Ob::Token();
	d_next = Ob::Token();
	Get();
	Oberon();
	Expect(0,__FUNCTION__);
}

Parser::Parser(Ob::Lexer *scanner, Ob::Errors* err) {
	maxT = 76;

	ParserInitCaller<Parser>::CallInit(this);
	la = &d_dummy;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = err;
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[7][78] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, T,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, T,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, T,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,T, T,T,T,T, T,x,x,x, x,x},
		{x,x,x,T, x,x,x,T, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,x,T,T, T,x,x,x, x,x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	ParserDestroyCaller<Parser>::CallDestroy(this);
}

void Parser::SynErr(const QString& sourcePath, int line, int col, int n, Ob::Errors* err, const char* ctx, const QString& str ) {
	QString s;
	QString ctxStr;
	if( ctx )
		ctxStr = QString( " in %1" ).arg(ctx);
    if( n == 0 )
        s = QString("EOF expected%1").arg(ctxStr);
    else if( n < Ob::TT_Specials )
        s = QString("'%2' expected%1").arg(ctxStr).arg(Ob::tokenTypeString(n));
    else if( n <= Ob::TT_Max )
        s = QString("%2 expected%1").arg(ctxStr).arg(Ob::tokenTypeString(n));
    else
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"T_Literals_ expected"); break;
			case 2: s = coco_string_create(L"T_Hash expected"); break;
			case 3: s = coco_string_create(L"T_Amp expected"); break;
			case 4: s = coco_string_create(L"T_Lpar expected"); break;
			case 5: s = coco_string_create(L"T_Latt expected"); break;
			case 6: s = coco_string_create(L"T_Rpar expected"); break;
			case 7: s = coco_string_create(L"T_Star expected"); break;
			case 8: s = coco_string_create(L"T_Ratt expected"); break;
			case 9: s = coco_string_create(L"T_Plus expected"); break;
			case 10: s = coco_string_create(L"T_Comma expected"); break;
			case 11: s = coco_string_create(L"T_Minus expected"); break;
			case 12: s = coco_string_create(L"T_Dot expected"); break;
			case 13: s = coco_string_create(L"T_2Dot expected"); break;
			case 14: s = coco_string_create(L"T_Slash expected"); break;
			case 15: s = coco_string_create(L"T_2Slash expected"); break;
			case 16: s = coco_string_create(L"T_Colon expected"); break;
			case 17: s = coco_string_create(L"T_ColonEq expected"); break;
			case 18: s = coco_string_create(L"T_Semi expected"); break;
			case 19: s = coco_string_create(L"T_Lt expected"); break;
			case 20: s = coco_string_create(L"T_Leq expected"); break;
			case 21: s = coco_string_create(L"T_Eq expected"); break;
			case 22: s = coco_string_create(L"T_Gt expected"); break;
			case 23: s = coco_string_create(L"T_Geq expected"); break;
			case 24: s = coco_string_create(L"T_Lbrack expected"); break;
			case 25: s = coco_string_create(L"T_Rbrack expected"); break;
			case 26: s = coco_string_create(L"T_Hat expected"); break;
			case 27: s = coco_string_create(L"T_Lbrace expected"); break;
			case 28: s = coco_string_create(L"T_Bar expected"); break;
			case 29: s = coco_string_create(L"T_Rbrace expected"); break;
			case 30: s = coco_string_create(L"T_Tilde expected"); break;
			case 31: s = coco_string_create(L"T_Keywords_ expected"); break;
			case 32: s = coco_string_create(L"T_ARRAY expected"); break;
			case 33: s = coco_string_create(L"T_BEGIN expected"); break;
			case 34: s = coco_string_create(L"T_BY expected"); break;
			case 35: s = coco_string_create(L"T_CASE expected"); break;
			case 36: s = coco_string_create(L"T_CONST expected"); break;
			case 37: s = coco_string_create(L"T_DEFINITION expected"); break;
			case 38: s = coco_string_create(L"T_DIV expected"); break;
			case 39: s = coco_string_create(L"T_DO expected"); break;
			case 40: s = coco_string_create(L"T_ELSE expected"); break;
			case 41: s = coco_string_create(L"T_ELSIF expected"); break;
			case 42: s = coco_string_create(L"T_END expected"); break;
			case 43: s = coco_string_create(L"T_FALSE expected"); break;
			case 44: s = coco_string_create(L"T_FOR expected"); break;
			case 45: s = coco_string_create(L"T_IF expected"); break;
			case 46: s = coco_string_create(L"T_IMPORT expected"); break;
			case 47: s = coco_string_create(L"T_IN expected"); break;
			case 48: s = coco_string_create(L"T_IS expected"); break;
			case 49: s = coco_string_create(L"T_MOD expected"); break;
			case 50: s = coco_string_create(L"T_MODULE expected"); break;
			case 51: s = coco_string_create(L"T_NIL expected"); break;
			case 52: s = coco_string_create(L"T_OF expected"); break;
			case 53: s = coco_string_create(L"T_OR expected"); break;
			case 54: s = coco_string_create(L"T_POINTER expected"); break;
			case 55: s = coco_string_create(L"T_PROCEDURE expected"); break;
			case 56: s = coco_string_create(L"T_RECORD expected"); break;
			case 57: s = coco_string_create(L"T_REPEAT expected"); break;
			case 58: s = coco_string_create(L"T_RETURN expected"); break;
			case 59: s = coco_string_create(L"T_THEN expected"); break;
			case 60: s = coco_string_create(L"T_TO expected"); break;
			case 61: s = coco_string_create(L"T_TRUE expected"); break;
			case 62: s = coco_string_create(L"T_TYPE expected"); break;
			case 63: s = coco_string_create(L"T_UNTIL expected"); break;
			case 64: s = coco_string_create(L"T_VAR expected"); break;
			case 65: s = coco_string_create(L"T_WHILE expected"); break;
			case 66: s = coco_string_create(L"T_Specials_ expected"); break;
			case 67: s = coco_string_create(L"T_ident expected"); break;
			case 68: s = coco_string_create(L"T_integer expected"); break;
			case 69: s = coco_string_create(L"T_real expected"); break;
			case 70: s = coco_string_create(L"T_string expected"); break;
			case 71: s = coco_string_create(L"T_hexchar expected"); break;
			case 72: s = coco_string_create(L"T_hexstring expected"); break;
			case 73: s = coco_string_create(L"T_Comment expected"); break;
			case 74: s = coco_string_create(L"T_Eof expected"); break;
			case 75: s = coco_string_create(L"T_MaxToken_ expected"); break;
			case 76: s = coco_string_create(L"??? expected"); break;
			case 77: s = coco_string_create(L"invalid Oberon"); break;
			case 78: s = coco_string_create(L"invalid number"); break;
			case 79: s = coco_string_create(L"invalid type"); break;
			case 80: s = coco_string_create(L"invalid selector"); break;
			case 81: s = coco_string_create(L"invalid relation"); break;
			case 82: s = coco_string_create(L"invalid AddOperator"); break;
			case 83: s = coco_string_create(L"invalid factor"); break;
			case 84: s = coco_string_create(L"invalid MulOperator"); break;
			case 85: s = coco_string_create(L"invalid label"); break;

		default:
		{
			s = QString( "generic error %1").arg(n);
		}
		break;
	}
    if( !str.isEmpty() )
        s = QString("%1 %2").arg(s).arg(str);
	if( err )
		err->error(Ob::Errors::Syntax, sourcePath, line, col, s);
	else
		qCritical() << "Error Parser" << line << col << s;
	//count++;
}

} // namespace

