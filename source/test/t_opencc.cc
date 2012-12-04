#include "icma/openccxx.h"
#include <string>
#include <iostream>

using namespace cma;

int main(int argc, char ** argv)
{
    std::string in("曾經有一份真誠的愛情放在我面前，我沒有珍惜，等我失去的時候我才後悔莫及。人事間最痛苦的事莫過於此。如果上天能夠給我一個再來一次得機會，我會對那個女孩子說三個字，我愛你。如果非要在這份愛上加個期限，我希望是，一萬年");
    std::string out;
    const char* modelPath = "../db/icwb/utf8/opencc";
    OpenCC cc(modelPath);
    long ret = cc.convert(in, out);
    std::cout<<"out: "<<out<<" ret "<<ret<<std::endl;
    if(-1 == ret)
        std::cout<<cc.errno()<<std::endl;
    return 0;
}
