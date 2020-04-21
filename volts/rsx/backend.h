#pragma once

namespace volts
{
    template<typename T>
    struct backend
    {
        void begin_frame()
        {
            static_cast<T*>(this)->begin();
        }

        void end_frame()
        {
            static_cast<T*>(this)->end();
        }
    };
}