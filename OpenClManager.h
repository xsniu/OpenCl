//
// Created by niu19 on 2017/12/29.
//

#ifndef OPENCLANDROID_OPENCLMANGER_H
#define OPENCLANDROID_OPENCLMANGER_H

#include <memory>
#include <string>
#include <iostream>

#define CheckCLError(err) {std::cout << "Error" in << __FILE__ << " line " \
                     << __LINE__ << " error " << err << std::endl;}
class OpenClMgr
{
    class Imp;
    std::unique_ptr<Imp>    _ImpUptr;
    OpenClMgr();

public:
    ~OpenClMgr();
    static OpenClMgr& GetInstance();

    int Init();
    void Uint();
    int CreateProgram(const std::string& kernelFile);
    int CreateKernel(const std::string& kernelName);

};
#endif //OPENCLANDROID_OPENCLMANGER_H
