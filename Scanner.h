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
		S_CHAR_STATE,		// ����ת���ַ����ַ�״̬
		FLOAT_STATE,		// ������״̬
		D_FLOAT_STATE,		// �ӽ���С����ĸ�����״̬
		E_FLOAT_STATE,		// �ӽ���ѧ�������ĸ�����״̬
		STRING_STATE,		// �ַ���״̬
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
		RESERVEDWORD,	// ������	
		ID,				// ��ʶ��
		INT,			// ��������
		FLOAT,			// ������
		CHAR,			// �ַ�
		S_CHAR,			// ��ת���ַ����ַ�
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
private:
	struct Token
	{
		TokenType kind;				// Token������
		string lexeme;				// Token��ֵ
		unsigned currentRow;	   	// ��ǰ��
		unsigned currentColumn;		// ��ǰ��
	};

	Token reservedWords[8];				// ����������
	string lineBuffer;					// ������, ����Դ�����е�һ������
	unsigned bufferPos;					// �����е�ָ��
	unsigned row;						// ��ǵ�ǰ��������Դ�����е���
	unsigned column;					// ��ǵ�ǰToken�ڻ������е���
	deque<Token> tokens;				// ����Դ���������е�Token
	ifstream fin;						// Դ�����ļ�������������

	bool isLegalSymbol(char ch);		// �Ϸ��ķ���
	Scanner(string sourceFile);			// ���캯��
	char nextChar();					// ���ػ������е���һ���ַ�
	void rollBack();					// �ع�������
	Token nextToken();					// ������һ��Token
	void scanToken();					// ɨ�����е�Token, ���浽tokens��
	TokenType searchReserved(string &s);	// ���ҹؼ���
public:
	static Scanner* getInstance(string sourceFile = "input.c");	// ����Scanner�ĵ�ʵ��
	deque<Token> getTokens();		// ����Դ���������е�Token

	// ���Ժ���
	void output();					// ���Դ�����е�����Token
};

#endif
