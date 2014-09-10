/*
 * \file VTimer.h
 * \author vernkin
 *
 * \date December 15, 2008, 2:06 PM
 */

#ifndef _VTIMER_H
#define	_VTIMER_H

#include <string>
#include <ctime>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

class VTimer{
public:
    VTimer(const string& title = "[Null]") : title_(title){
    }

    void start(){
        sctime = clock();
    }

    void startWithTitle(string title){
        setTitle(title);
        start();
    }

    void end(){
        ectime = clock();
    }

    void print(){
        cout<<title_<<" uses "<<(double)(ectime-sctime)/CLOCKS_PER_SEC <<" sec"<< endl;
    }

    void endAndPrint(){
        end();
        print();
    }

    void setTitle(const string& title){
        title_ = title;
    }

private:
    clock_t sctime, ectime;
    string title_;
};



#endif	/* _VTIMER_H */

