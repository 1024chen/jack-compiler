#include "Scanner.h"
#include <cstring>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <iterator>

using namespace std;

Scanner::Scanner(string sourceFile)
{
	row = column = 0;
	isEof = false;
	bufferPos = 0;
	
	reservedWords[0] = { RESERVEDWORD, "if", 0, 0 };
	reservedWords[1] = { RESERVEDWORD, "else", 0, 0 };
	reservedWords[2] = { RESERVEDWORD, "int", 0, 0 };
	reservedWords[3] = { RESERVEDWORD, "return", 0, 0 };
	reservedWords[4] = { RESERVEDWORD, "void", 0, 0 };
	reservedWords[5] = { RESERVEDWORD, "while", 0, 0 };

	fin.open(sourceFile.c_str());
	if (fin.fail())
	{
		std::cerr << "�ļ���ʧ�ܣ� �ļ�"
			<< sourceFile << "������" << std::endl;
		exit(-1);
	}
}

Scanner* Scanner::getInstance(string sourceFile)
{
	Scanner *scanner = new Scanner(sourceFile);
	return scanner;
}

Scanner::TokenType Scanner::searchReserved(string &s)
{
	for (auto it = begin(reservedWords); it != end(reservedWords); ++it)
		if (it->lexeme == s)
			return it->kind;
	return ID;
}

char Scanner::nextChar()
{
	if (bufferPos >= lineBuffer.size())
	{
		row++;
		getline(fin, lineBuffer);
		lineBuffer += '\n';
		if (!fin.fail())
		{
			bufferPos = 0;
			return lineBuffer[bufferPos++];
		}
		else
			return EOF;
	}
	else
		return lineBuffer[bufferPos++];
}

void Scanner::rollBack()
{
	assert(bufferPos > 0);
	bufferPos--;
}

void Scanner::scanToken()
{
	Token token = nextToken();
	while (token.kind != ENDOFFILE)
	{
		tokens.push_back(token);
		token = nextToken();
	}
}

deque<Scanner::Token> Scanner::getTokens()
{	
	return tokens;
}

void Scanner::output()
{
	scanToken();
	while (!tokens.empty())
	{
		auto token = tokens.front();
		tokens.pop_front();
		cout << " " << token.lexeme << "\t(" << token.currentRow << ", " << token.currentColumn << ")" << endl;
	}
}

Scanner::Token Scanner::nextToken()
{
	Token token;
	unsigned tokenStringIndex = 0;
	State state = START_STATE;
	while (state != DONE_STATE)
	{
		char ch = nextChar();
		if (ch == EOF)
		{
			isEof = true;
			token.kind = ENDOFFILE;
			break;
		}
		switch (state)
		{
		case START_STATE:										// ��ʼ״̬
			column = bufferPos;
			if (ch == ' ' || ch == '\t' || ch == '\n')
				;
			else if (isalpha(ch))
			{
				state = ID_STATE;		// �����ʶ��״̬
				token.kind = ID;
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (isdigit(ch))
			{
				state = NUM_STATE;		// ��������״̬
				token.kind = NUM;	
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '+')
			{
				state = PLUS_STATE;		// ����Ӻ�״̬
				token.kind = PLUS;	
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '-')
			{
				state = MINUS_STATE;	// �������״̬
				token.kind = MINUS;		
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '*')
			{
				state = MULT_STATE;		// ����˺�״̬
				token.kind = MULT;		
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '/')
			{
				state = DIVIDE_STATE;	// �������״̬
				token.kind = DIVIDE;	
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '(')
			{
				state = LPARAN_STATE;	// ����������״̬
				token.kind = LPARAN;	
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == ')')
			{
				state = RPARAN_STATE;	// ����������״̬
				token.kind = RPARAN;	
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '[')
			{
				state = LBRACE_STATE;	// ������������״̬
				token.kind = LBRACE;	
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == ']')
			{
				state = RBRACE_STATE;	// ������������״̬
				token.kind = RBRACE;	
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '{')
			{
				state = LBRACE_STATE;	// ����������״̬
				token.kind = LBRACE;	
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '}')
			{
				state = RBRACE_STATE;	// �����һ�����״̬
				token.kind = RBRACE;
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == ';')
			{
				state = SEMI_STATE;		// ����ֺ�״̬
				token.kind = SEMI;		
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == ',')
			{
				state = COMMA_STATE;	// ���붺��״̬
				token.kind = COMMA;		
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '<')
			{
				state = LT_STATE;		// ����С�ں�״̬
				token.kind = LT;		
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '>')
			{
				state = GT_STATE;		// ������ں�״̬
				token.kind = GT;		
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '=')
			{
				state = ASSIGN_STATE;	// ���븳ֵ��״̬
				token.kind = ASSIGN;	
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '!')
			{
				state = IN_NEQ_STATE;	// ����ӽ�������״̬
				token.lexeme += ch;		
				token.currentRow = row;
				token.currentColumn = column;
			}
			break;
		case NUM_STATE:											// ����״̬
			if (isalpha(ch))
			{
				state = ERROR_STATE;
				token.kind = ERROR;
				token.lexeme += ch;
				break;
			}
			else if (isdigit(ch))
			{
				token.lexeme += ch;
				break;
			}
			else
			{
				rollBack();
				state = DONE_STATE;
				break;
			}
		case ID_STATE:											// ��ʶ��״̬
			if (!isalnum(ch))
			{
				rollBack();
				state = DONE_STATE;
				break;
			}
			else
			{
				token.lexeme += ch;
				break;
			}
		case LPARAN_STATE:										// ������״̬
			rollBack();
			state = DONE_STATE;
			break;
		case RPARAN_STATE:										// ������״̬
			rollBack();
			state = DONE_STATE;
			break;
		case LBRACE_STATE:										// ������״̬
			rollBack();
			state = DONE_STATE;
			break;
		case RBRACE_STATE:										// �һ�����״̬
			rollBack();
			state = DONE_STATE;
			break;
		case LBRACKET_STATE:									// ��������״̬
			rollBack();
			state = DONE_STATE;
			break;
		case RBRACKET_STATE:									// ��������״̬
			rollBack();
			state = DONE_STATE;
			break;
		case SEMI_STATE:										// �ֺ�״̬
			rollBack();
			state = DONE_STATE;
			break;
		case COMMA_STATE:										// ����״̬
			rollBack();
			state = DONE_STATE;
			break;
		case PLUS_STATE:										// �Ӻ�״̬
			rollBack();
			state = DONE_STATE;
			break;
		case MINUS_STATE:										// ����״̬
			rollBack();
			state = DONE_STATE;
			break;
		case MULT_STATE:										// �˺�״̬
			rollBack();
			state = DONE_STATE;
			break;
		case DIVIDE_STATE:										// ����״̬
			if (ch != '*')
			{
				rollBack();
				state = DONE_STATE;
				break;
			}
			else
			{
				state = INCOMMENT_STATE;
				token.lexeme.pop_back();
				break;
			}
		case ASSIGN_STATE:
			if (ch == '=')
			{
				state = EQ_STATE;
				token.kind = EQ;
				token.lexeme += ch;
				break;
			}
			else
			{
				rollBack();
				state = DONE_STATE;
				break;
			}
		case LT_STATE:											// С�ں�״̬
			if (ch == '=')
			{
				state = LE_STATE;
				token.kind = LE;
				token.lexeme += ch;
				break;
			}
			else
			{
				rollBack();
				state = DONE_STATE;
				break;
			}
		case GT_STATE:											// ���ں�״̬
			if (ch == '=')
			{
				state = GE_STATE;
				token.kind = GE;
				token.lexeme += ch;
				break;
			}
			else
			{
				rollBack();
				state = DONE_STATE;
				break;
			}
		case IN_NEQ_STATE:										// �ӽ������ں�״̬
			if (ch == '=')
			{
				state = NEQ_STATE;
				token.kind = NEQ;
				token.lexeme += ch;
				break;
			}
			else
			{
				state = ERROR_STATE;
				token.kind = ERROR;
				token.lexeme += ch;
				break;
			}
		case LE_STATE:											// С�ڵ��ں�״̬
			rollBack();
			token.kind = LE;
			break;
		case GE_STATE:											// ���ڵ��ں�״̬
			rollBack();
			token.kind = GE;
			break;	
		case NEQ_STATE:											// �����ں�״̬
			rollBack();
			token.kind = NEQ;
			break;
		case ERROR_STATE:										// ����״̬
			if (ch == ' ' || ch == '\n' || ch == '\t')
				state = DONE_STATE;
			else
				token.lexeme += ch;
			break;
		case INCOMMENT_STATE:									// ע��״̬
			if (ch == '*')
				state = P_INCOMMENT_STATE;
			break;
		case P_INCOMMENT_STATE:									// �ӽ��˳�ע��״̬
			if (ch == '/')
				state = START_STATE;
			else
				state = INCOMMENT_STATE;
			break;
		}
		if (state == DONE_STATE && token.kind == ID)
			token.kind = searchReserved(token.lexeme);
	}
	return token;
}
