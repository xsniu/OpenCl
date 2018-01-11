//
// Created by niu19 on 2017/12/29.
//

#ifndef OPENCLANDROID_OPENCLMANGER_H
#define OPENCLANDROID_OPENCLMANGER_H

#include <memory>
#include <string>
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



template<typename T>
class InfoDevice
{

    class Imp;
    std::unique_ptr<Imp> _ImpUptr;
public:
    InfoDevice();
    ~InfoDevice();
    
public:
    static void Display();
    void AppendBitFiled(T info, T value, std::string infoName, std::string &str)
    {
        if (info & value)
        {
            if (str.length() > 0)
            {
                str.append(" |");
            }

            str.append(infoName);
        }
    }

};
#endif //OPENCLANDROID_OPENCLMANGER_H
