#include <assert.h>
#include <iostream>
#include "icma/util/CPPStringUtils.h"

using namespace std;
/**
 * test CPPStringUtils
 */
int main(int argc, char** argv) {

    //ios_base::sync_with_stdio(false);
    //wcout.imbue(locale("zh_CN.utf-8"));

    string s = "中文and英文";

    assert(s.length() == 15);

    wstring ws = CPPStringUtils::from_utf8w(s);
    assert(ws.length() == 7);

    wstring fe1(L"A_");
    fe1.append(1, ws[0]);
    assert(fe1.length() == 3);

    string fs1 = CPPStringUtils::to_utf8(fe1);
    assert(fs1.length() == 5);

    cout<<"All tests PASSED!"<<endl;

    return 0;
}

