

#include "XString.h"
#include <iostream>
#include <string>

//#include "UniConversion.h"

using namespace std;

int main()
{
//	char ascii[4096];
	XTextParamList tpl;
	tpl.LoadFile("d:\\zh-CN.txt");

	tpl.Print();

	wstring wstr = L"中国字123很美";
	string str;
	utf8util::WStrToUtf8(wstr,str);
	utf8util::Utf8toWStr(str, wstr);

	str = utf8util::ws2s(wstr);
	wstr = utf8util::s2ws(str);

//	memset(ascii, 0, sizeof(ascii));
//	utf8_to_ascii(str.c_str(), str.length()*3+1, ascii);



//	cout << str << endl;
//	cout << ascii << endl;
	getchar();


	return 0;
}



