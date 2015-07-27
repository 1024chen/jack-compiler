#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

#include "Parser.h"
#include <map>
#include <vector>

class SymbolTable
{
public:
    enum Kind
    {
        STATIC, FIELD, ARG, VAR, FUNCTION, METHOD, CONSTRUCTOR, NONE
    };
    class Info
    {
    public:
        string type;    // int, float, char, string
        Kind kind;      // kind : static, field, var, argument 
        vector<string> args;
        Info()
        {
            type = "0";
            kind = NONE;
        }
        friend bool operator==(Info info1, Info info2)
        {
            if (info1.type == info2.type && info1.kind == info2.kind && info1.args == info2.args)
                return true;
            else
                return false;
        }
    };
    static Info None;
private:
    vector<map<string, Info>> classesTable;     // ����ű�����
    map<string, int> classIndex;                // ����������������
    map<string, Info> subroutineTable;          // �������ű�
    
    int currentClassNumber;     // ��ǰ����ű���������
    string currentClass;        // ��ǰ������
    string getFunctionName(string name);
    string getClassName(string name);
    void initialSubroutineTable();
public:
    SymbolTable();
    void classesTableInsert(Parser::TreeNode *t);       // ����ű�Ĳ������
    void subroutineTableInsert(Parser::TreeNode *t);    // �������ű�Ĳ������
    Info subroutineTableFind(string name);

//    Info find(string name);     
    Info classesTableFind(string className, string functionName);
    bool classIndexFind(string className);

    void printClassesTable();
};

#endif
