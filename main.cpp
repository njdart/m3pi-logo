#include "mbed.h"
#include "m3pi.h"
#include "MSCFileSystem.h"
#include "Parser.h"

m3pi m3pi;
MSCFileSystem msc("usb");
DigitalOut led(LED1);
Serial pc(USBTX, USBRX);
Timer timer;
FILE* CMD;
PARSER parser();

// Global varibles
int Line_Number = 0;
float speed = 0.5;
char CM[20];
int var = 0.0;
int intrep = 0;
int Tot_rep = var;
char tok[100];
char * pch;
int s=0;
int Rep;
char command2[100];
char Buffer[200];


void text_edit() {
    //open file for overwrite
    CMD = fopen("/usb/Commands.txt", "w");
    //this shall house the line that has/is being written
    char Buffer[50];
    //used to end the while loop
    int i = 0;
    pc.printf("\n\r===Type code. Note, any mistakes will have to be edited in the textfile===\n\r===\"**\" will exit text editing===\n\r");
    while (i != 1) {
        pc.printf(">>");
        gets(Buffer);
        //if the user types '**' text editing will be stoped.
        if (( strcmp( Buffer, "**" ) >= -1) && (strcmp (Buffer, "***") <= 1)) {
            i=1;
        }
        pc.printf("%s\n\r", Buffer);
        fprintf(CMD, "%s\n\r", Buffer);
    }
    pc.printf("===END OF EDIT===");
    fclose(CMD);
    pc.printf("\n\r===RESETING===\n\r");
    m3pi.cls();
    m3pi.printf("  RESET ");
    wait(2.0);
//end text-edit
}

void check() {
    CMD = fopen("/usb/Commands.txt", "r");
    pc.printf("\n\r\n\r===M3PI LOGO INTERPRITER===\n\r\n\r");
    if (CMD == NULL) {
        m3pi.cls();
        m3pi.printf("NO CMD");
        m3pi.locate(0,1);
        m3pi.printf("TXT_EDIT");
        fclose(CMD);
        //Begin Hyperterminal code edit
        text_edit();
    }
    fclose(CMD);
}


void PARSER::doThings(char* command, float* Vin, float* repCMD, float* leftmotor, float* rightmotor, float* OPspeed) {
    if ( strcmp( command, "BACKWARD" ) == 0 ) {
        rightmotor[0] = -speed;
        leftmotor[0] = -speed;
        OPspeed[0] = (0.079617* *Vin);
    } else if ( strcmp( command, "LEFT" ) == 0 ) {
        rightmotor[0] = speed;
        leftmotor[0] = -speed;
        OPspeed[0] = (0.0157 * (*Vin/10));
    } else if ( strcmp( command, "RIGHT" ) == 0 ) {
        rightmotor[0] = -1;
        leftmotor[0] = speed;
        OPspeed[0] = (0.0157 * (*Vin/10));
    } else if ( strcmp( command, "STOP" ) == 0 ) {
        rightmotor[0] = speed;
        leftmotor[0] = speed;
    } else if ( strcmp( command, "FORWARD" ) == 0 ) {
        rightmotor[0] = speed;
        leftmotor[0] = speed;
        OPspeed[0] = ((0.079617)* *Vin);
    } else if ( strcmp( command, "REPEAT" ) == 0) {
        rightmotor[0] = 0;
        leftmotor[0] = 0;
        //repCMD[0] = 1;
    } else  {
        rightmotor[0] = 0;
        leftmotor[0] = 0;
        m3pi.cls();
        m3pi.printf("ERR CMD");
        m3pi.locate(0,1);
        m3pi.printf("INVALID");
        pc.printf("BAD COMMAND, Line: %d", Line_Number);
    }
}

int main() {
    //see if there is a file there, if not user can make one through hyperterminal
    check();
    m3pi.cls();
    wait(5.0);
    FILE* CMD = fopen("/usb/Commands.txt", "r");
    while (!feof(CMD)) {
        // Read from the command file and sort commands from variables
        fscanf(CMD, "%s %d\n", &CM, &var);
        float repeat;
        float r, l;
        //void doThings(char* command, float* Vin, float* repCMD, float* leftmotor, float* rightmotor, float* OPspeed);
        parser.doThings(CM,var,repeat,r,l,s);
        if (( strcmp( CM, "[" ) == 0)) {
            pch = strtok (CM,"1234567890 []");
            while (pch != NULL) {                   //count how many tokens there are
                fscanf(CMD, "%s %d\n", &CM, &var);
                pch = strtok (NULL, "1234567890 []");
                sprintf(Buffer, "%s %d \n", CM, var);
                s++;
            }
        }
        for (Rep=0; Rep < Tot_rep; Rep++) {         //number of repeats to do
            for (int Scan=0; Scan <= s; Scan++) {   //number of items to be repeated
                //scan for new command line
                sscanf(Buffer, "%s %d", CM, &var);
                //pass command to parser
                parser.doThings(CM,var,repeat,r,l,s);
                m3pi.right(r);
                m3pi.left(l);
                wait(s);
                Line_Number ++;
            }
        }
    }
    m3pi.right(r);
    m3pi.left(l);
    wait(s);
    s = 0;
    Line_Number ++;
}
