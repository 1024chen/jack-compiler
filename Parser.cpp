#include "Parser.h"
#include "Scanner.h"
#include <string>
#include <iostream>
#include <deque>

using namespace std;


void Parser::print()
{
	printSyntaxTree(syntaxTree);
}

void Parser::printSyntaxTree(Parser::TreeNode *tree)
{
	static int indentno = 0;
	indentno += 2;
	while (tree != nullptr)
	{
		for (int i = 0; i < indentno; i++)
			cout << " ";
		switch (tree->nodeKind)
		{	
		case Parser::compound_statement_kind:
			cout << "compound_statement" << endl;
			break;
		case Parser::Bool_kind:
			cout << tree->token.lexeme << endl;
			break;
		case Parser::Var_declaration_kind:				
			cout << "var declaration" << endl;
			break;
		case Parser::Var_array_declaration_kind:		
			cout << "array declaratoin" << endl;			
			break;
		case Parser::Function_declaration_kind:			
			cout << "function declaration" << endl;			
			break;
		case Parser::Param_kind:						
			cout << "param" << endl;			
			break;
		case Parser::Param_Array_kind:
			cout << "array declaration" << endl;
			break;
		case Parser::If_kind:							
			cout << "if" << endl;		
			break;
		case Parser::While_kind:						
			cout << "while" << endl;	
			break;
		case Parser::Assign_kind:						
			cout << "==" << endl;
			break;
		case Parser::Break_kind:
			cout << "break" << endl;
			break;
		case Parser::Continue_kind:
			cout << "continue" << endl;
			break;
		case Parser::Call_kind:							
			cout << "function call" << endl;
			break;
		case Parser::Return_kind:						
			cout << "return" << endl;		
			break;
		case Parser::Operation_kind:					
			cout << tree->token.lexeme << endl;
			break;
		case Parser::Const_kind:						
			cout << "const: " << tree->token.lexeme << endl;
			break;
		case Parser::Identifier_kind:					
			cout << "ID: " << tree->token.lexeme << endl;
			break;
		case Parser::Type_kind:							
			cout << "type: " << tree->token.lexeme << endl;
			break;
		default:	
			break;
		}
		printSyntaxTree(tree->child[0]);
		printSyntaxTree(tree->child[1]);
		printSyntaxTree(tree->child[2]);
		printSyntaxTree(tree->child[3]);
		tree = tree->next;
	}
	indentno -= 2;
}

Parser::Parser(string filename) : scanner(filename)
{
	error = false;
	errorNumbers = 1;
}

void Parser::parse()
{
	scanner.scanToken();
	tokens = scanner.getTokens();
	syntaxTree = parse_program();
	cout << "**************************syntaxTree*****************************" << endl;
}

void Parser::syntaxError(string got)
{
	cerr << "\n>>>error (" << Parser::errorNumbers << ") ("
		<< currentToken.currentRow << ", " << currentToken.currentColumn << ") " << endl;
	cerr << "\tunexpected token " << got << endl;
	Parser::errorNumbers++;
	error = true;
}

Parser::TreeNode * Parser::getSyntaxTree()
{
	return syntaxTree;
}

void Parser::readNextToken()
{
	if (!tokens->empty())
	{
		currentToken = tokens->front();
		tokens->pop_front();
	}
	else
		exit(-1);
}

void Parser::match(string expectedString)
{
	if (currentToken.lexeme == expectedString)
	{
		readNextToken();
	}
	else
	{
		syntaxError(expectedString);
		readNextToken();
	}
}

bool Parser::peek(Scanner::TokenType expectedToken)
{
	if (currentToken.kind == expectedToken)
	{
		readNextToken();
		return true;
	}
	else
		return false;
}

Parser::TreeNode * Parser::parse_program()
{
	TreeNode *t = parse_declaration_list();
	return t;
}

/*
�ķ�: declaration_list -> declaration_list declaration
                        | declaration

�﷨��:  declaration -> declaration -> declaration -> ... -> declaration
*/
Parser::TreeNode * Parser::parse_declaration_list()
{
	TreeNode *t = nullptr;
	readNextToken();
	t = parse_declaration();
	TreeNode *p = t;
	while (currentToken.kind != Scanner::ENDOFFILE)
	{
		TreeNode *q = parse_declaration();
		if (q != nullptr)
		{
			if (t == nullptr)
				t = p = q;
			else
			{
				p->next = q;
				p = q;
			}
		}
	}
	return t;
}


/*
�ķ�: declaration -> var_declaration
                   | function_declaration
����ķ��Ľ�������:
  ͨ���۲����ֲ�ͬ��������ʽ,��֪:
    ������������ʽ��: int a; ������int a[];
    ������������ʽ��: int func(){}
  ������Ҫ��ǰ��2��Token��֪�����׸õ�����һ������ʽ.
    �����ǰ���ĵ�2��Token��������,��ô�͵���function_declaration()
    ����͵���var_declaration()
*/
Parser::TreeNode * Parser::parse_declaration()
{
	TreeNode *t = nullptr;
	if (!tokens->empty())
	{
		secondToken = tokens->front();
		tokens->pop_front();
	}
	else
		exit(-1);
	
	if (!tokens->empty())
	{
		thirdToken = tokens->front();
		tokens->pop_front();
	}
	else
		exit(-1);

	if (thirdToken.kind == Scanner::LPARAN)
	{	
		tokens->push_front(thirdToken);
		tokens->push_front(secondToken);
		t = parse_function_declaration();
		return t;
	}
	else
	{
		tokens->push_front(thirdToken);
		tokens->push_front(secondToken);
		t = parse_var_declaration();
		return t;
	}
}

/*
�ķ�: var_declaration -> type ID ;
                       | type ID [ INT ] ;

�﷨��:
        int a;
	Var_declaration_kind
         /       \
 	   int        a

	   int array[3];
	Var_array_declaration_kind
	    /    |    \
	  int  array   3
*/

Parser::TreeNode * Parser::parse_var_declaration()
{
	TreeNode *t = new TreeNode();
	t->nodeKind = Var_declaration_kind;
	t->child[0] = parse_type();

	if (currentToken.kind == Scanner::ID)
	{
		TreeNode *p = new TreeNode();
		p->nodeKind = Identifier_kind;
		p->token = currentToken;
		t->child[1] = p;
		readNextToken();
	}	
	if (peek(Scanner::SEMI))
		return t;
	else if (peek(Scanner::LBRACKET))
	{
		t->nodeKind = Var_array_declaration_kind;
		if (currentToken.kind == Scanner::INT)
		{
			TreeNode *p = new TreeNode();
			p->nodeKind = Const_kind;
			p->token = currentToken;
			t->child[2] = p;
			readNextToken();
		}
		else
		{
			syntaxError(currentToken.lexeme);
		}
		match("]");
		match(";");
		return t;
	}
	else
	{
		syntaxError(currentToken.lexeme);
		return t;
	}
}


/*
�ķ�: type -> int
            | float
            | void
            | string
            | char
*/
Parser::TreeNode * Parser::parse_type()
{
	TreeNode *t = new TreeNode();
	if (currentToken.kind != Scanner::RW_INT && currentToken.kind != Scanner::RW_FLOAT
		&& currentToken.kind != Scanner::RW_VOID && currentToken.kind != Scanner::RW_STRING
		&& currentToken.kind != Scanner::RW_CHAR && currentToken.kind != Scanner::RW_BOOL)
		syntaxError(currentToken.lexeme);
	t->nodeKind = Type_kind;
	t->token = currentToken;
	readNextToken();
	return t;
}

/*
�ķ�: function_declaration -> type ID ( params ) compound_statement 

�﷨��:
    int func(int a, int b)
	{
		......
	}
	     Function_declaration_kind
		    /    |     |      \
		  int  func  params  compound_statement
*/

Parser::TreeNode * Parser::parse_function_declaration()
{
	TreeNode *t = new TreeNode();
	t->nodeKind = Function_declaration_kind;
	t->child[0] = parse_type();
	if (currentToken.kind == Scanner::ID)
	{
		TreeNode *p = new TreeNode();
		p->nodeKind = Identifier_kind;
		p->token = currentToken;
		t->child[1] = p;
		readNextToken();
	}
	match("(");
	t->child[2] = parse_params();
	match(")");
	t->child[3] = parse_compound_statement();
	return t;
}

/*
�ķ�: params -> param_list 
              | void
              | empty

�ķ��Ľ���: �����ǰ��Token�������ǲ���void, �����, ��ôֱ������һ��void���͵ĵ��ڵ�
            �����ǰ��Token�ǲ���һ��������, �����, ��������һ��void���͵ĵ��ڵ�
			�����˵��������һ������, ���Ǿͽ���param_list
*/
Parser::TreeNode * Parser::parse_params()
{
	TreeNode *t = nullptr;
	if (currentToken.kind == Scanner::RW_VOID)
	{
		t = new TreeNode();
		t->nodeKind = Type_kind;
		t->token = currentToken;
		readNextToken();
		return t;
	}
	else if (currentToken.kind == Scanner::RPARAN)
	{
		t = new TreeNode();
		t->nodeKind = Type_kind;
		t->token.lexeme = "void";
		return t;
	}
	else
	{
		t = parse_param_list();
		return t;
	}
}

/*
�ķ�: param_list -> param_list , param
                  | param

�ķ��Ľ���: ����һ����ݹ���ķ�,�ݹ�չ��֮�����ʽΪ: param, param, param, ... , param
            Ҳ����˵������һ��param, �����Ƚ�����һ��param, ��������֮��, �����ǰToken��һ������, 
			��ô��֤�����滹��param, ����ѭ��
�﷨��;
    param -> param -> param -> ... -> param
*/
Parser::TreeNode * Parser::parse_param_list()
{
	TreeNode *t = nullptr;
	t = parse_param();
	TreeNode *p = t;
	while (peek(Scanner::COMMA))
	{
		TreeNode *q = parse_param();
		if (q != nullptr)
		{
			if (t == nullptr)
				t = p = q;
			else
			{
				p->next = q;
				p = q;
			}
		}
	}
	return t;
}

/*
�ķ�: param -> type ID
             | type ID [ ]			 
*/

Parser::TreeNode * Parser::parse_param()
{
	TreeNode *t = new TreeNode();
	t->nodeKind = Param_kind;
	t->child[0] = parse_type();
	if (currentToken.kind == Scanner::ID)
	{
		TreeNode *p = new TreeNode();
		p->nodeKind = Identifier_kind;
		p->token = currentToken;
		t->child[1] = p;
		readNextToken();
	}
	if (peek(Scanner::LBRACKET))
	{
		TreeNode *p = new TreeNode();
		p->nodeKind = Param_Array_kind;
		t->child[2] = p;
		match("]");
		return t;
	}
	return t;
}


/*
�ķ�: compound_statement -> { local_declarations statement_list }   

*/
Parser::TreeNode * Parser::parse_compound_statement()
{
	TreeNode *t = nullptr;
	match("{");
	t = new TreeNode();
	t->nodeKind = compound_statement_kind;
	t->child[0] = parse_local_declarations();
	TreeNode *p = t->child[0];
	if (t->child[0] != nullptr)
	{	
		while (p->next != nullptr)
			p = p->next;
		p->next = parse_statement_list();
	}
	else
	{
		t->child[0] = parse_statement_list();
	}
	match("}");

	return t;
}

/*
�ķ�: local_declarations -> local_declarations var_declaration
                          | empty
						  
�ķ��Ľ���: �ȿ��ǵ�һ������ʽ, ����һ����ݹ��, ������ǰ���param_list, չ��֮��������һ��var_declaration
            ���ǵڶ�������ʽ�п����ǿ�, ���������Ƶ���0������var_declaration
			������var_declaration���ж�, ������ǰ��һ��Token���ƶ��Ƿ�ü���չ��
*/
Parser::TreeNode * Parser::parse_local_declarations()
{
	TreeNode *t = nullptr;
	TreeNode *p = t;
	while (currentToken.kind == Scanner::RW_INT || currentToken.kind == Scanner::RW_FLOAT
		|| currentToken.kind == Scanner::RW_VOID || currentToken.kind == Scanner::RW_STRING
		|| currentToken.kind == Scanner::RW_CHAR || currentToken.kind == Scanner::RW_BOOL)
	{
		TreeNode *q = parse_var_declaration();
		if (t == nullptr)
		{
			t = p = q;
		}
		else
		{
			p->next = q;
			p = q;
		}
	}
	return t;
}

/*
�ķ�: statement_list -> statement_list statement
                      | empty

�ķ��Ľ���: ����ķ��Ľ����Ƚ�����. �ķ��ݹ�չ��֮����0������statement, ��һ��statement�кܶ�������
            ����ǰ���ķ�ʽ�ǲ��е�.
			���ǵ�����Ƶ�: compound_statement -> { local_declarations statement_list }   
			�����ж�: ���һ��statement������, ��ô��������Token�ض��ǽ���һ���һ�����
			�����жϵ�ǰ��Token�ǲ����һ������������Ƿ����չ��statement
*/

Parser::TreeNode * Parser::parse_statement_list()
{
	TreeNode *t = nullptr;
	TreeNode *p = t;

	if (currentToken.kind != Scanner::RBRACE)
	{
		t = parse_statement();
		p = t;
	}
	while (currentToken.kind != Scanner::RBRACE)
	{
		TreeNode *q = parse_statement();
		if (q != nullptr)
		{
			if (t == nullptr)
			{
				t = p = q;
			}
			else
			{
				p->next = q;
				p = q;
			}
		}
	}
	return t;
}

/*
�ķ�: statement -> expression_statement
                 | compound_statement
                 | selection_statement
                 | iteration_statement
                 | return_statement
                 | assign_statement
				 | break_statement
				 | continue_statement
*/

Parser::TreeNode * Parser::parse_statement()
{
	TreeNode *t = nullptr;
	if (currentToken.kind == Scanner::RW_RETURN)
		t = parse_return_statement();
	else if (currentToken.kind == Scanner::RW_WHILE)
		t = parse_iteration_statement();
	else if (currentToken.kind == Scanner::RW_IF)
		t = parse_selection_statement();
	else if (currentToken.kind == Scanner::LBRACE)
		t = parse_compound_statement();
	else if (currentToken.kind == Scanner::RW_BREAK)
	{
		t = new TreeNode();
		t->nodeKind = Break_kind;
		t->token = currentToken;
		readNextToken();
	}
	else if (currentToken.kind == Scanner::RW_CONTINUE)
	{
		t = new TreeNode();
		t->nodeKind = Continue_kind;
		t->token = currentToken;
		readNextToken();
	}
	else
		t = parse_expression_statement();
	return t;
}


/*
�ķ�: expression_statement -> expression ;
                      | empty ;

����: ������statement_list, �ǿ���ǰ��Token�ǲ��Ƿֺ�, �������, ��ô��˵����һ��expression, �����, ��ô����empty
					  */
Parser::TreeNode * Parser::parse_expression_statement()
{
	TreeNode *t = nullptr;
	if (peek(Scanner::SEMI))
		return t;
	else
	{
		t = parse_expression();
		match(";");
		return t;
	}
}

/*
�ķ�: selection_statement -> if ( expression ) statement
                           | if ( expression ) statement else statement

˵��: ��Ϊ�����Ĺ����������Ƶ�, ����ֻ����һ���﷨��, �������������
*/
Parser::TreeNode * Parser::parse_selection_statement()
{
	TreeNode *t = new TreeNode();
	t->nodeKind = If_kind;
	t->token = currentToken;
	match("if");
	match("(");
	t->child[0] = parse_expression();
	match(")");
	t->child[1] = parse_statement();
	if (peek(Scanner::RW_ELSE))
		t->child[2] = parse_statement();
	return t;
}

/*
�ķ�: iteration_statement -> while ( expression ) statement

*/
Parser::TreeNode * Parser::parse_iteration_statement()
{
	TreeNode *t = new TreeNode();
	t->nodeKind = While_kind;
	t->token = currentToken;
	match("while");
	match("(");
	t->child[0] = parse_expression();
	match(")");
	t->child[1] = parse_statement();
	return t;
}

/*
�ķ�: return_statement -> return ;
                        | return expression ;
*/
Parser::TreeNode * Parser::parse_return_statement()
{
	TreeNode *t = new TreeNode();
	t->nodeKind = Return_kind;
	t->token = currentToken;
	match("return");
	if (peek(Scanner::SEMI))
		return t;
	else
	{
		t->child[0] = parse_expression();
		match(";");
	}
	return t;
}

/*
�ķ�: var -> ID
           | ID [ expression ]
*/
Parser::TreeNode * Parser::parse_var()
{
	TreeNode *t = new TreeNode();
	t->nodeKind = Identifier_kind;
	t->token = currentToken;
	readNextToken();
	if (peek(Scanner::LBRACKET))
	{
		t->child[0] = parse_expression();
		match("]");
	}
	return t;
}

/*
�ķ�: expression -> expression && boolexpression
                 | expression || boolexpression
                 | boolexpression
*/
Parser::TreeNode * Parser::parse_expression()
{
	TreeNode *t = nullptr;
	t = parse_boolexpression();
	while (currentToken.lexeme == "&&" || currentToken.lexeme == "||")
	{
		TreeNode *p = new TreeNode();
		p->nodeKind = Bool_kind;
		p->token = currentToken;
		p->child[0] = t;
		t = p;
		readNextToken();
		t->child[1] = parse_boolexpression();
	}
	return t;
}

/*
�ķ�: boolexpression -> additive_expression relational_operator additive_expression
                    | additive_expression
*/
Parser::TreeNode * Parser::parse_boolexpression()
{
	TreeNode *t = nullptr;
	t = parse_additive_expression();
	if (currentToken.lexeme == "=" || currentToken.lexeme == "<" || currentToken.lexeme == ">" ||
		currentToken.lexeme == "==" || currentToken.lexeme == ">=" || currentToken.lexeme == "<=" ||
		currentToken.lexeme == "!=")
	{
		TreeNode *p = new TreeNode();
		if (currentToken.lexeme == "=")
			p->nodeKind = Assign_kind;
		else
			p->nodeKind = Operation_kind;
		p->token = currentToken;
		p->child[0] = t;
		t = p;
		readNextToken();
		t->child[1] = parse_additive_expression();
	}
	return t;
}

/*
�ķ�: additive_expression -> additive_expression + term
                           | additive_expression �C term
                           | term
*/
Parser::TreeNode * Parser::parse_additive_expression()
{
	TreeNode *t = nullptr;
	t = parse_term();
	while (currentToken.lexeme == "+" || currentToken.lexeme == "-")
	{
		TreeNode *p = new TreeNode();
		p->nodeKind = Operation_kind;
		p->token = currentToken;
		p->child[0] = t;
		t = p;
		readNextToken();
		t->child[1] = parse_term();
	}
	return t;
}

/*
�ķ�: term -> term * factor
            | term / factor
            | factor
*/
Parser::TreeNode * Parser::parse_term()
{
	TreeNode *t = nullptr;
	t = parse_factor();
	while (currentToken.lexeme == "*" || currentToken.lexeme == "/")
	{
		TreeNode *p = new TreeNode();
		p->nodeKind = Operation_kind;
		p->token = currentToken;
		p->child[0] = t;
		t = p;
		readNextToken();
		t->child[1] = parse_factor();
	}
	return t;
}

/*
�ķ�: factor -> ( expression )
              | var
              | call
              | INT
              | FLOAT
              | CHAR
              | STRING
*/
Parser::TreeNode * Parser::parse_factor()
{
	TreeNode *t = nullptr;
	if (currentToken.kind == Scanner::INT || currentToken.kind == Scanner::CHAR 
	 || currentToken.kind == Scanner::FLOAT || currentToken.kind == Scanner::STRING
	 || currentToken.kind == Scanner::BOOL)
	{
		t = new TreeNode();
		t->nodeKind = Const_kind;
		t->token = currentToken;
		readNextToken();
		return t;
	}

	if (peek(Scanner::LPARAN))
	{
		t = parse_expression();
		match(")");
	}
	else
	{
		if (!tokens->empty())
			secondToken = tokens->front();
		else
			exit(-1);
		if (currentToken.kind == Scanner::ID && secondToken.kind == Scanner::LPARAN)
			t = parse_call();
		else
			t = parse_var();
	}
	return t;
}

/*
�ķ�: call -> ID ( args )
*/
Parser::TreeNode * Parser::parse_call()
{
	TreeNode *t = new TreeNode();
	t->nodeKind = Call_kind;
	if (currentToken.kind == Scanner::ID)
	{
		TreeNode *p = new TreeNode();
		p->nodeKind = Identifier_kind;
		p->token = currentToken;
		t->child[0] = p;
		readNextToken();
	}
	match("(");
	t->child[1] = parse_args();
	match(")");
	return t;
}

/*
�ķ�: args -> arg_list 
            | empty
*/
Parser::TreeNode * Parser::parse_args()
{
	TreeNode *t = nullptr;
	if (currentToken.kind != Scanner::RPARAN)
		t = parse_expression();
	TreeNode *p = t;
	while (currentToken.kind == Scanner::COMMA)
	{
		match(",");
		TreeNode *q = parse_expression();
		if (q != nullptr)
		{
			if (t == nullptr)
			{
				t = p = q;
			}
			else
			{
				p->next = q;
				p = q;
			}
		}
	}
	return t;
}

bool Parser::hasError()
{
	return error;
}
