#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

void test(string filename);

int main()
{
    cout << "********************����������********************" << endl;
    test("testInteger.txt");   
    cout << "********************���Ը�����********************" << endl;
    test("testFloat.txt");   
    cout << "********************�����ַ���********************" << endl;
    test("testChar.txt");   
    cout << "********************���Ա�ʶ��********************" << endl;
    test("testIdentifier.txt");   
    cout << "********************���Ա�����********************" << endl;
    test("testReservedWords.txt");   
    cout << "********************�����ַ���********************" << endl;
    test("testString.txt");   
    
    return 0;
}

void test(string filename)
{
    string cmd;
    string parameter;
    string testFile;
    ifstream fin;
    
    cmd = "..\\C-Mini.exe";
    testFile = filename;
    fin.open(testFile.c_str());
    if (fin.fail())
    {
        cout << "�ļ�" << testFile << "������" << endl; 
    }
    else
    {
        cout << "�ļ�����Ϊ: " << endl;
        string txt;
        getline(fin, txt);
        while (fin)
        {
            cout << txt << endl;
            getline(fin, txt);
        }
        fin.close();
        cout << "\ntokenΪ: " << endl;
        cmd = cmd + " " + testFile;
        system(cmd.c_str());
        ifstream scannerOutput;
        scannerOutput.open("ScannerOutput.txt");
        if (scannerOutput.fail())
        {
            cout << "����ļ�������" << endl;
        }
        else
        {
            string out;
            while(scannerOutput)
            {
                getline(scannerOutput, out);
                cout << out << endl;
            }
            scannerOutput.close();
        }
    }    
}

