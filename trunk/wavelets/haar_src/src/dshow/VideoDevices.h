

#ifndef VideoDevices_h
#define VideoDevices_h


class VideoDevices
{
public:
        //VideoDevices();
        //VideoDevices(const VideoDevices& vds);
        //~VideoDevices();

// Operators
        //const VideoDevices& operator=(const VideoDevices& vds);

// Operations
        static HRESULT Enumerate(std::vector<std::wstring>& names);     //enumerate available devices        
        
// Access
// Inquiry

protected:
private:
        //VideoDevices(const VideoDevices& vds);
        //const VideoDevices& operator=(const VideoDevices& vds);

};

// Inlines


#endif VideoDevices_h

