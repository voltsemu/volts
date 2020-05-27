# RSX backends

each platform has its own backend, these being

* vulkan
* d3d12 (directx12). win10 only
* metal. macos only

each conforms to a simple api of 
```cpp
// called once before the render loop starts
// perform setup in this 
//  - creating a window/frame
//  - creating pipeline state
//  - setup shaders etc
void init(const char* name, const char* version);

// called once the render loop is over
// teardown the renderer in this
// memory leaks are bad
void deinit();

// called at the begining of each frame
void begin();

// called at the end of each frame, present here
void end();
```
any new backends being added must fullfil these functions.
