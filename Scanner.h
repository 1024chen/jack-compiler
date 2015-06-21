#ifndef _Scanner_H_
#define _Scanner_H_

#include <string>
#include <fstream>
#include <deque>

using namespace std;

class Scanner
{
private:
	enum State	// ת��ͼ�е�״̬
	{
		START_STATE,		// ��ʼ״̬
		ID_STATE,			// ��ʶ��״̬
		INT_STATE,			// ������״̬
		CHAR_STATE,			// �ַ�״̬		
		CHAR_STATE_A,
		CHAR_STATE_B,
		CHAR_STATE_C,
		
		FLOAT_STATE,		// ������״̬
		D_FLOAT_STATE,		// �ӽ���С����ĸ�����״̬
		E_FLOAT_STATE,		// �ӽ���ѧ�������ĸ�����״̬
		STRING_STATE,		// �ַ���״̬
		S_STRING_STATE,		// ����ת���ַ����ַ���
		PLUS_STATE,			// ��״̬
		MINUS_STATE,		// ��״̬
		MULT_STATE,			// ��״̬
		DIVIDE_STATE,		// ��״̬	
		ASSIGN_STATE,		// ��ֵ״̬
		EQ_STATE,			// ����״̬
		IN_NEQ_STATE,		// �ӽ�������״̬
		NEQ_STATE,			// ������״̬
		LT_STATE,			// С��״̬
		LE_STATE,			// С�ڵ���״̬
		GT_STATE,			// ����״̬
		GE_STATE,			// ���ڵ���״̬
		LPARAN_STATE,		// ������״̬
		RPARAN_STATE,		// ������״̬						
		SEMI_STATE,			// �ֺ�״̬
		COMMA_STATE,		// ����״̬
		LBRACKET_STATE,		// ��������״̬
		RBRACKET_STATE,		// ��������״̬
		LBRACE_STATE,		// ������״̬
		RBRACE_STATE,		// �һ�����״̬

		INCOMMENT_STATE,	// ע��״̬
		P_INCOMMENT_STATE,	// ��Ҫ����ע��״̬
		DONE_STATE,			// ����״̬
		ERROR_STATE			// ����״̬
	};

public:
	enum TokenType
	{
		// ������	
		RW_IF, RW_ELSE, RW_INT, RW_RETURN, RW_VOID, RW_FLOAT, RW_WHILE, RW_BREAK, RW_CONTINUE, RW_STRING, RW_CONST, RW_CHAR,
		
		ID,				// ��ʶ��
		INT,			// ��������
		FLOAT,			// ������
		CHAR,			// �ַ�
		STRING,			// �ַ���

		// �������
		PLUS,		// ��										+
		MINUS,		// ��										-
		MULT,		// ��										*
		DIVIDE,		// ��										/
		ASSIGN,		// ��ֵ										=
		EQ,			// ���� equal								==
		NEQ,		// ������ not equal                         !=
		LT,			// С�� less than							<
		LE,			// С�ڵ��� less than or equal				<=
		GT,			// ���� greater than						>
		GE,			// ���ڵ���  greater than or equal			>=
		LPARAN,		// ������									(
		RPARAN,		// ������									)
		SEMI,		// �ֺ�										;
		COMMA,		// ����										,
		LBRACKET,	// ��������									[
		RBRACKET,	// ��������									]
		LBRACE,		// ������									{
		RBRACE,		// �һ�����									}

		ERROR,		// ����
		ENDOFFILE	// �ļ�����
	};
	struct Token
	{
		TokenType kind;				// Token������
		string lexeme;				// Token��ֵ
		unsigned currentRow;	   	// ��ǰ��
		unsigned currentColumn;		// ��ǰ��
	};
private:
	string lineBuffer;					// ������, ����Դ�����е�һ������
	unsigned bufferPos;					// �����е�ָ��
	unsigned row;						// ���浱ǰ��������Դ�����е��к�
	unsigned column;					// ���浱ǰToken�ڻ������е��к�
	deque<Token> *tokens;				// ����Դ���������е�Token
	ifstream fin;						// Դ�����ļ�������������
	
	char nextChar();					// ���ػ������е���һ���ַ�
	void rollBack();					// �ع�������
	TokenType searchReserved(string &s);	// ���ҹؼ���
	Token nextToken();					// ������һ��Token
public:
	Scanner(string filename);
	deque<Token> * getTokens();		// ����Դ���������е�Token
	void scanToken();	// ɨ�����е�Token, ���浽tokens��
	// ���Ժ���
	void output();					// ���Դ�����е�����Token
};

#endif
