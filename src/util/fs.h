#include <iostream>

class fs {
    public:
      static std::string readFile(const char* filename);
      static void sayHello();
    private:
      fs();
      ~fs();
};