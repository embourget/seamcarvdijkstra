#include <string>


#ifndef SEAM_CARVING_CORE
#define SEAM_CARVING_CORE
namespace seamcarvingcore {
    class Logger {
        public:
            bool verbose;
            Logger print(std::string str);
            Logger push(std::string  str);
            Logger print(int i);
            Logger push(int i);
            Logger endl();
    };
    void treat(unsigned char* in, unsigned char* out, 
            int height, int width, int channels, Logger log);
}
#endif //SEAM_CARVING_CORE