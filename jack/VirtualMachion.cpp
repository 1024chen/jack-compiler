#include "VritualMachion.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>

static int sp;         // �Ĵ���  ջָ��: ָ��ջ����һ����Ļ�ַ
static int local;      // �Ĵ���  ָ��ǰVM����local�Ļ�ַ
static int argument;   // �Ĵ���  ָ��ǰVM����argument�εĻ�ַ
static int _this;      // �Ĵ���  ָ��ǰthis��(�ڶ���)�Ļ�ַ
static int that;       // �Ĵ���  ָ��ǰthat��(�ڶ���)�Ļ�ַ
static int ip;         // �Ĵ���  ָ����һ��Ҫִ�е�ָ��
static int temp[7];    // �Ĵ���  �洢��ʱֵ
/*
*       RAM��ַ                 ����
*          0~15            ������,δʹ��
*        16~155            VM���������VM�����ľ�̬����
*      256~2047            ջ
*    2048~16383            ��(���ڴ�Ŷ��������)
**/
static short ram[266385];                                   // ���ݴ洢��
static vector<vector<string>> instructions_ram;             // ָ��洢��
static int staticCount;                                     // ��¼��̬�����Ѿ����������
static unordered_map<string, int> staticVarNames;           // ��¼��̬�������ڴ��е�λ��
static unordered_map<string, int> instruction_address;      // ����label��functionָ����ָ��洢���еĵ�ַ
static vector<string> currentInstruction;                   // ���浱ǰ����ִ�е�ָ��
static string currentClassName;                             // ���浱ǰ����ִ�е�ָ�����ڵ��������
static bool arriveEnd = false;                              // ����Ƿ񵽴�����β

void executeArithmetic(string command)
{
    if (command == "add")
    {
        ram[sp - 2] = ram[sp - 2] + ram[sp - 1];
        sp--;
    }
    else if (command == "sub")
    {
        ram[sp - 2] = ram[sp - 2] - ram[sp - 1];
        sp--;
    }
    else if (command == "neg")
    {
        ram[sp - 1] = -ram[sp - 1];
    }
    else if (command == "eq")
    {
        if (ram[sp - 2] == ram[sp - 1])
            ram[sp - 2] = -1;
        else
            ram[sp - 2] = 0;
        sp--;
    }
    else if (command == "gt")
    {
        if (ram[sp - 2] > ram[sp - 1])
            ram[sp - 2] = -1;
        else
            ram[sp - 2] = 0;
        sp--;
    }
    else if (command == "lt")
    {
        if (ram[sp - 2] < ram[sp - 1])
            ram[sp - 2] = -1;
        else
            ram[sp - 2] = 0;
        sp--;
    }
    else if (command == "and")
    {
        ram[sp - 2] = ram[sp - 2] & ram[sp - 1];
        sp--;
    }
    else if (command == "or")
    {
        ram[sp - 2] = ram[sp - 2] | ram[sp - 1];
        sp--;
    }
    else if (command == "not")
    {
        ram[sp - 1] = ~ram[sp - 1];
    }
}

void executePush(string segment, int index)
{
    if (segment == "static")
    {
        string t;
        ostringstream iss(t);
        iss << index;
        string staticVarName = currentClassName + "." + t;
        auto result = staticVarNames.find(staticVarName);
        if (result == staticVarNames.end())
        {
            staticVarNames.insert({staticVarName, staticCount});
            staticCount++;
        }
        else
        {
            int temp = ram[16 + result->second];
            ram[sp++] = temp;
        }
    }
    else if (segment == "argument")
        ram[sp++] = ram[argument + index];
    else if (segment == "local")
        ram[sp++] = ram[local + index];
    else if (segment == "constant")
        ram[sp++] = index;
    else if (segment == "this")
        ram[sp++] = ram[_this + index];
    else if (segment == "that")
        ram[sp++] = ram[that + index];
    else if (segment == "pointer")
    {
        if (index == 0)
            ram[sp++] = _this;
        else if (index == 1)
            ram[sp++] = that;
    }
    else if (segment == "temp")
        ram[sp++] = temp[index];
}

void executePop(string segment, int index)
{
    if (segment == "static")
    {
        string t;
        ostringstream iss(t);
        iss << index;
        string staticVarName = currentClassName + "." + t;
        auto result = staticVarNames.find(staticVarName);
        if (result == staticVarNames.end())
        {
            staticVarNames.insert({ staticVarName, staticCount });
            staticCount++;
        }
        else
        {
            int temp = ram[--sp];
            ram[16 + result->second] = temp;
        }
    }
    if (segment == "argument")
        ram[argument + index] = ram[--sp];
    else if (segment == "local")
        ram[local + index] = ram[--sp];
    else if (segment == "this")
        ram[_this + index] = ram[--sp];
    else if (segment == "that")
        ram[that + index] = ram[--sp];
    else if (segment == "pointer")
    {
        if (index == 0)
            _this = ram[--sp];
        else if (index == 1)
            that = ram[--sp];
    }
    else if (segment == "temp")
        temp[index] = ram[--sp];
}
void executeLabel(string label)
{
    // do nothing
}

void executeGoto(string label)
{
    ip = instruction_address.find(label)->second;
}

void executeIf(string label)
{
    int temp = ram[--sp];
    if (temp != 0)
        ip = instruction_address.find(label)->second;
}

void executeCall(string functionName, int numArgs)
{
    if (functionName == "IO.putchar")
    {
        putchar(ram[sp-1]);
        return;
    }
    else if (functionName == "IO.getchar")
    {
        ram[sp++] = getchar();
        return;
    }
    ram[sp++] = ip;
    ram[sp++] = local;
    ram[sp++] = argument;
    ram[sp++] = _this;
    ram[sp++] = that;
    argument = sp - numArgs - 5;
    local = sp;
    ip = instruction_address.find(functionName)->second;
}

void executeReturn()
{
    int temp = local;
    ip = ram[temp - 5];
    ram[argument] = ram[--sp];      // ���õ����ߵķ���ֵ
    sp = argument + 1;
    that = ram[temp - 1];
    _this = ram[temp - 2];
    argument = ram[temp - 3];
    local = ram[temp - 4];
}

void executeFunction(string functionName, int numLocals)
{
    auto iter = functionName.cbegin();
    while (iter != functionName.cend())
        if (*iter++ == '.')
            break;
    currentClassName = string(functionName.cbegin(), --iter);
    for (int i = 0; i < numLocals; i++)
        ram[sp++] = 0;
}

void loadProgram()
{
    std::ifstream fin;
    int count = 0;
    
    for (auto iter = filenames.cbegin(); iter != filenames.cend(); ++iter)
    {
        fin.open(*iter);
        if (fin.fail())
        {
            std::cerr << "Error: file " << *iter << "does not exsist!\n";
            exit(-1);
        }
        string temp;
        while (std::getline(fin, temp))
        {
            vector<string> instruction;
            std::istringstream iss(temp);
            string word;
            while (iss >> word)
            {
                instruction.push_back(word);
            }
            if (instruction[0] == "label" || instruction[0] == "function")
                instruction_address.insert({ instruction[1], count });
            instructions_ram.push_back(instruction);
            count++;
        }
        fin.close();
    }
    vector<string> end;
    end.push_back("end");
    instructions_ram.push_back(end);
}

void init()
{
    ip = instruction_address.find("Sys.init")->second;
    sp = 50;
    local = sp;
    ram[local - 5] = instructions_ram.size() - 1;
}

void run()
{
    init();
    while (true)
    {
        instructionFetch();
        if (ip == 1)
        {
            int temp;
            temp = 0;
            sp;
            local;
            argument;
            _this;
            that;
            currentInstruction;
        }
        ++ip;
        execute();
        if (arriveEnd == true)
            break;
    }
}

void instructionFetch()
{
    currentInstruction = instructions_ram[ip];
}

void execute()
{
    string command = currentInstruction[0];
    if (command == "add" || command == "sub" || command == "neg" || command == "eq" || command == "gt" ||
        command == "lt" || command == "and" || command == "or" || command == "not")
        executeArithmetic(currentInstruction[0]);
    else if (command == "push")
        executePush(currentInstruction[1], atoi(currentInstruction[2].c_str()));
    else if (command == "pop")
        executePop(currentInstruction[1], atoi(currentInstruction[2].c_str()));
    else if (command == "label")
        executeLabel(currentInstruction[1]);
    else if (command == "goto")
        executeGoto(currentInstruction[1]);
    else if (command == "if-goto")
        executeIf(currentInstruction[1]);
    else if (command == "call")
        executeCall(currentInstruction[1], atoi(currentInstruction[2].c_str()));
    else if (command == "return")
        executeReturn();
    else if (command == "function")
        executeFunction(currentInstruction[1], atoi(currentInstruction[2].c_str()));
    else if (command == "end")
        arriveEnd = true;
}
