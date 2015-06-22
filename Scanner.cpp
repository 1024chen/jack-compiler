#include "Scanner.h"
#include <cstring>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <iterator>

using namespace std;

Scanner::Scanner(string filename)
{
	fin.open(filename.c_str());
	if (fin.fail())
	{
		cerr << "file " << filename << " does not exit" << endl;
		exit(-1);
	}
	row = column = 0;
	bufferPos = 0;
	tokens = new(deque<Token>);
}

Scanner::TokenType Scanner::searchReserved(string &s)
{
	if (s == "bool")
		return RW_BOOL;
	else if (s == "true")
		return RW_TRUE;
	else if (s == "false")
		return RW_FALSE;
	else if (s == "if")
		return RW_IF;
	else if (s == "else")
		return RW_ELSE;
	else if (s == "void")
		return RW_VOID;
	else if (s == "int")
		return RW_INT;
	else if (s == "float")
		return RW_FLOAT;
	else if (s == "while")
		return RW_WHILE;
	else if (s == "break")
		return RW_BREAK;
	else if (s == "continue")
		return RW_CONTINUE;
	else if (s == "return")
		return RW_RETURN;
	else if (s == "string")
		return RW_STRING;
	else if (s == "char")
		return RW_CHAR;
	else if (s == "const")
		return RW_CONST;
	else
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
			bufferPos = column = 0;
			if (lineBuffer[bufferPos] == '\t')
				column += 4;
			else
				column++;
			return lineBuffer[bufferPos++];
		}
		else
			return EOF;
	}
	else
	{
		if (lineBuffer[bufferPos] == '\t')
			column += 8;
		else
			column++;
		return lineBuffer[bufferPos++];
	}
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
		tokens->push_back(token);
		token = nextToken();
	}
	tokens->push_back(token);
}

deque<Scanner::Token> * Scanner::getTokens()
{
	return tokens;
}

void Scanner::output()
{
	ofstream fout("ScannerOutput.txt");
	while (!tokens->empty())
	{
		auto token = tokens->front();
		tokens->pop_front();
		string kind;
		if (token.kind == RW_IF || token.kind == RW_ELSE || token.kind == RW_VOID || token.kind == RW_INT || 
			token.kind == RW_FLOAT ||token.kind == RW_STRING || token.kind == RW_CHAR || token.kind == RW_CONST || 
			token.kind == RW_WHILE || token.kind == RW_BREAK || token.kind == RW_CONTINUE || token.kind == RW_RETURN)
			cout << "�ؼ���: ";
		else if (token.kind == ID)
			cout << "��ʶ��: ";
		else if (token.kind == INT)
			cout << "��������: ";
		else if (token.kind == FLOAT)
			cout << "������: ";
		else if (token.kind == CHAR)
			cout << "�ַ�: ";
		else if (token.kind == STRING)
			cout << "�ַ���: ";
		else if (token.kind == PLUS)
			cout << "�Ӻ�: ";
		else if (token.kind == MINUS)
			cout << "����: ";
		else if (token.kind == MULT)
			cout << "�˺�: ";
		else if (token.kind == DIVIDE)
			cout << "����: ";
		else if (token.kind == ASSIGN)
			cout << "��ֵ��: ";
		else if (token.kind == EQ)
			cout << "���ں�: ";
		else if (token.kind == NEQ)
			cout << "�����ں�: ";
		else if (token.kind == LT)
			cout << "С�ں�: ";
		else if (token.kind == LE)
			cout << "С�ڵ��ں�: ";
		else if (token.kind == GT)
			cout << "���ں�: ";
		else if (token.kind == GE)
			cout << "���ڵ��ں�: ";
		else if (token.kind == LPARAN)
			cout << "������: ";
		else if (token.kind == RPARAN)
			cout << "������: ";
		else if (token.kind == SEMI)
			cout << "�ֺ�: ";
		else if (token.kind == COMMA)
			cout << "����: ";
		else if (token.kind == LBRACKET)
			cout << "��������: ";
		else if (token.kind == RBRACKET)
			cout << "��������: ";
		else if (token.kind == LBRACE)
			cout << "������: ";
		else if (token.kind == RBRACE)
			cout << "�һ�����: ";
		else if (token.kind == ERROR)
			cout << "����ĵ���: ";
		else if (token.kind == ENDOFFILE)
			cout << "�ļ�����: ";
		cout << token.lexeme << "\t(" << token.currentRow << ", " << token.currentColumn << ")" << endl;
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
			token.kind = ENDOFFILE;
			break;
		}
		switch (state)
		{
		case START_STATE:										// ��ʼ״̬
			if (ch == ' ' || ch == '\t' || ch == '\n')
				;
			else if (isalpha(ch) || ch == '_')
			{
				state = ID_STATE;		// �����ʶ��״̬
				token.kind = ID;
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (isdigit(ch))
			{
				state = INT_STATE;		// ��������״̬
				token.kind = INT;
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
				state = LBRACKET_STATE;	// ������������״̬
				token.kind = LBRACKET;
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == ']')
			{
				state = RBRACKET_STATE;	// ������������״̬
				token.kind = RBRACKET;
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
			else if (ch == '&')
			{
				state = IN_AND_STATE;	// ����ӽ���״̬
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '|')
			{
				state = IN_OR_STATE;	// ����ӽ���״̬
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '"')
			{
				state = STRING_STATE;	// �����ַ���״̬
				token.kind = STRING;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else if (ch == '\'')		// ���뵥�ַ�״̬
			{
				state = CHAR_STATE;
				token.kind = CHAR;
				token.currentRow = row;
				token.currentColumn = column;
			}
			else												// �����Ƿ��ַ�
			{
				state = ERROR_STATE;
				token.kind = ERROR;
				token.lexeme += ch;
				token.currentRow = row;
				token.currentColumn = column;
			}
			break;
		case INT_STATE:											// ����״̬
			if (isdigit(ch))
			{
				token.lexeme += ch;
			}
			else if (ch == '.')
			{
				state = D_FLOAT_STATE;
				token.lexeme += ch;
			}
			else if (ch == 'E' || ch == 'e')
			{
				state = E_FLOAT_STATE;
				token.lexeme += ch;
			}
			else
			{
				rollBack();
				state = DONE_STATE;
			}
			break;
		case ID_STATE:											// ��ʶ��״̬
			if (isalpha(ch) || isdigit(ch) || ch == '_')
			{
				token.lexeme += ch;
			}
			else
			{
				rollBack();
				state = DONE_STATE;
			}
			break;
		case D_FLOAT_STATE:										// �ӽ���С����ĸ�����״̬
			if (isdigit(ch))
			{
				state = FLOAT_STATE;
				token.kind = FLOAT;
				token.lexeme += ch;
			}
			else
			{
				state = ERROR_STATE;
				token.kind = ERROR;
				token.lexeme += ch;
			}
			break;
		case E_FLOAT_STATE:										// �ӽ���ѧ�������ĸ�����״̬
			if (isdigit(ch))
			{
				state = FLOAT_STATE;
				token.kind = FLOAT;
				token.lexeme += ch;
			}
			else
			{
				state = ERROR_STATE;
				token.kind = ERROR;
				token.lexeme += ch;
			}
			break;
		case FLOAT_STATE:										// ������״̬
			if (isdigit(ch))
			{
				state = FLOAT_STATE;
				token.lexeme += ch;
			}
			else if (ch == 'E' || ch == 'e')
			{
				state = E_FLOAT_STATE;
				token.kind = FLOAT;
				token.lexeme += ch;
			}
			else
			{
				rollBack();
				state = DONE_STATE;
			}
			break;
		case STRING_STATE:										// �ַ���״̬
			if (ch == '"')
			{
				state = DONE_STATE;
			}
			else if (ch == '\\')
			{
				state = S_STRING_STATE;
				token.lexeme += ch;
			}
			else
			{
				token.lexeme += ch;
			}
			break;
		case S_STRING_STATE:
			state = STRING_STATE;
			token.lexeme.pop_back();
			token.lexeme += ch;
			break;
		case CHAR_STATE:										// �ַ�״̬
			if (ch != '\\' && ch != '\'')
			{
				state = CHAR_STATE_A;
				token.lexeme += ch;
			}
			else if (ch == '\\')
			{
				state = CHAR_STATE_B;
				token.lexeme += ch;
			}
			else if (ch == '\'')
			{
				state = ERROR_STATE;
				token.kind = ERROR;
				token.lexeme += ch;
				token.lexeme += ch;
			}
			break;
		case CHAR_STATE_A:
			if (ch == '\'')
			{
				state = DONE_STATE;
			}
			else
			{
				state = ERROR_STATE;
				token.kind = ERROR;
				string tmp = "'";
				tmp.append(token.lexeme);
				tmp += ch;
				token.lexeme = tmp;
			}
			break;
		case CHAR_STATE_B:
			if (ch == 'a' || ch == 'b' || ch == 'f' || ch == 'n' || ch == 'r' || ch == 't' || ch == 'v'
				|| ch == '\\' || ch == '?' || ch == '\'' || ch == '"')
			{
				state = CHAR_STATE_C;
				token.lexeme += ch;
			}
			else
			{
				state = ERROR_STATE;
				token.kind = ERROR;
				string tmp = "'\\";
				tmp += ch;
				token.lexeme = tmp;
			}
			break;
		case CHAR_STATE_C:
			if (ch == '\'')
			{
				state = DONE_STATE;
			}
			else
			{
				state = ERROR_STATE;
				token.kind = ERROR;
				string tmp = "'";
				tmp.append(token.lexeme);
				tmp += ch;
				token.lexeme = tmp;
			}
			break;
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
		case ASSIGN_STATE:										// ��ֵ״̬
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
		case EQ_STATE:											// ���ں�״̬
			rollBack();
			token.kind = EQ;
			state = DONE_STATE;
			break;
		case AND_STATE:
			rollBack();
			token.kind = AND;
			state = DONE_STATE;
			break;
		case OR_STATE:
			rollBack();
			token.kind = OR;
			state = DONE_STATE;
			break;
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
		case IN_AND_STATE:										// �ӽ���״̬
			if (ch == '&')
			{
				state = AND_STATE;
				token.kind = AND;
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
		case IN_OR_STATE:
			if (ch == '|')
			{
				state = OR_STATE;
				token.kind = OR;
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
			state = DONE_STATE;
			break;
		case GE_STATE:											// ���ڵ��ں�״̬
			rollBack();
			token.kind = GE;
			state = DONE_STATE;
			break;
		case NEQ_STATE:											// �����ں�״̬
			rollBack();
			token.kind = NEQ;
			state = DONE_STATE;
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
