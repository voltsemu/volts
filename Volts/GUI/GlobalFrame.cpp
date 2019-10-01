#include "Frame.h"

namespace Volts::GUI
{
    Frame* Frame::Singleton = nullptr;
    Frame* Frame::GetSingleton() 
    {
        if(Frame::Singleton)
            return Frame::Singleton;
        
        Frame::Singleton = new Frame();
        return Frame::Singleton;
    }

    void Frame::Log(std::string Msg)
    {
        LogBuffer.append(Msg.c_str());
    }
}