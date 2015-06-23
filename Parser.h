#ifndef _PARSER_H
#define _PARSER_H

#include "Scanner.h"
#include <string>
#include <deque>

using namespace std;

class Parser
{	
public:
	enum NodeKind {
		Var_declaration_kind, Var_array_declaration_kind, Const_kind,Param_Array_kind,
		Function_declaration_kind, Param_kind, compound_statement_kind,
		If_kind, While_kind, Assign_kind, Call_kind, Return_kind,Break_kind, 
		Continue_kind, Operation_kind, Bool_kind, Identifier_kind, Type_kind, 
	};
	class TreeNode
	{
	public:
		Scanner::Token token;
		TreeNode *child[5];
		TreeNode *next;
		NodeKind nodeKind;
		TreeNode()
		{
			token.kind = Scanner::INT;
			token.lexeme = "";
			token.currentRow = 0;
			token.currentRow = 0;

			child[0] = child[1] = child[2] = child[3] = child[4] = nullptr;
			next = nullptr;
			nodeKind = Var_declaration_kind;
		}
	};
private:
	TreeNode *syntaxTree;
	Scanner scanner;
	deque<Scanner::Token> *tokens;
	Scanner::Token currentToken, secondToken, thirdToken;
	
	static Parser *instance;
	unsigned int errorNumbers;
	void syntaxError(string got);

	void readNextToken();
	void match(string expectedString);
	bool peek(Scanner::TokenType expectedToken);

	TreeNode * parse_program();
	TreeNode * parse_declaration_list();
	TreeNode * parse_declaration();
	TreeNode * parse_var_declaration();
	TreeNode * parse_function_declaration();
	TreeNode * parse_type();
	TreeNode * parse_params();
	TreeNode * parse_compound_statement();
	TreeNode * parse_param_list();
	TreeNode * parse_param();
	TreeNode * parse_local_declarations();
	TreeNode * parse_statement_list();
	TreeNode * parse_statement();
	TreeNode * parse_expression_statement();
	TreeNode * parse_selection_statement();
	TreeNode * parse_iteration_statement();
	TreeNode * parse_return_statement();
	TreeNode * parse_expression();
	TreeNode * parse_boolexpression();
	TreeNode * parse_var();
	TreeNode * parse_additive_expression();
	TreeNode * parse_term();
	TreeNode * parse_factor();
	TreeNode * parse_call();
	TreeNode * parse_args();
	void printSyntaxTree(TreeNode *tree);
public:
	Parser(string filename);
	TreeNode *getSyntaxTree();
	void print();
	void parse();
};

#endif
