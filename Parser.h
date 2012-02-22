#include <iostream>
using namespace std;
//Serial pc(USBTX, USBRX);



class PARSER {
public:
    PARSER();
    void doThings(char* command, float* Vin, float* repCMD, float* leftmotor, float* rightmotor, float* OPspeed);
};
