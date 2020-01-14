/*
 * Licensed under the GNU General Public License version 2 with exceptions.
 * Autor Kirill Pshenichnyi (c) 2019
 * Just simple test EtherCAT protocol on Trinamic controller using libsoem
 */

#include <stdio.h>
#include "ethercat.h"

char IOmap[4096];
OSAL_THREAD_HANDLE thread1;

int main(int argc,char *argv[]){
    if(argc<2){
        printf("%s interface_name\n",argv[0]);
        return 0;
    }

    if(!ec_init(argv[1])){
        printf("Error EtherCAT initializate! =(\n");
        return 0;
    }

    if(ec_config_init(FALSE) > 0){
        printf("slave devices: %d\n",ec_slavecount);
        ec_config_map(&IOmap);
        ec_configdc();
        ec_statecheck(0,EC_STATE_SAFE_OP,EC_TIMEOUTSTATE *4);

        printf("segments : %d : %d %d %d %d\n",ec_group[0].nsegments ,ec_group[0].IOsegment[0],ec_group[0].IOsegment[1],ec_group[0].IOsegment[2],ec_group[0].IOsegment[3]);

    }else{
        printf("EtherCAT init config false =(\n"
               "May be not slave devices or you not root?\n");
        return 0;
    }

    printf("Name: %s, Vendor ID: %.4x\n",ec_slave[1].name,ec_slave[1].eep_man);
    printf("CoE details: %.4x\n",ec_slave[1].CoEdetails);

    // READ
    /*int ec_SDOread(uint16 slave, uint16 index, uint8 subindex,
                      boolean CA, int *psize, void *p, int timeout);
                      */
    //WRITE
    // int ec_SDOwrite(uint16 Slave, uint16 Index, uint8 SubIndex,
    //    boolean CA, int psize, void *p, int Timeout);

    uint32 value = 0;
    int size = 4;
    ec_SDOread(1,0x1000,0,FALSE,&size,&value,100000);
    printf("size: %d, value: 0x%.8x\n",size,value);

    char name[16];
    size=16;

    ec_SDOread(1,0x1008,0,FALSE,&size,name,100000);
    printf("name from SDOread: %s\n",name);


    /* DRIVE */
    value = 0;
    size = 1;
    ec_SDOread(1,0x2003,0,FALSE,&size,&value,100000);
    printf("Max current: %d%\n", 100*value/255);

    /* Start */

    uint16 control_word = 0x0000;
    uint16 status_word = 0x0000;

    size = 2; ec_SDOread(1,0x6040,0,FALSE,&size,&control_word,100000);
    size = 2; ec_SDOread(1,0x6041,0,FALSE,&size,&status_word,100000);
    printf("Control Word: 0x%.4x, status word: 0x%.4x\n", control_word&0xffff,status_word&0xffff);

    /* set switches disabled */
    uint8 switchs = 0x03;
    size = 1; ec_SDOwrite(1,0x2005,0,FALSE,size,&switchs,100000);   //switches off

    /* read mode && and set position mode */
    uint8 mode = 0;
    size = 1; ec_SDOread(1,0x6060,0,FALSE,&size,&mode,100000);
    printf("Mode: %d\n",mode);
    mode = 1; size = 1; ec_SDOwrite(1,0x6060,0,FALSE,size,&mode,100000);
    mode = 0; size = 1; ec_SDOread(1,0x6060,0,FALSE,&size,&mode,100000);
    printf("Set mode: %d\n",mode);

    // set control word
    control_word = 6;  // READY_TO_SWITCH_ON
    size = 2; ec_SDOwrite(1,0x6040,0,FALSE,size,&control_word,100000);
    control_word = 7;  // SWITCHED_ON
    size = 2; ec_SDOwrite(1,0x6040,0,FALSE,size,&control_word,100000);
    control_word = 0xf;  // OPERATION_ENABLED
    size = 2; ec_SDOwrite(1,0x6040,0,FALSE,size,&control_word,100000);

    // target position
    uint32 target_position = 500000;
    size = 4; ec_SDOwrite(1,0x607a,0,FALSE,size,&target_position,100000);

    //Start!
    control_word = 0x1f; // new set point
    size = 2; ec_SDOwrite(1,0x6040,0,FALSE,size,&control_word,100000);

    //size = 2; ec_SDOread(1,0x6040,0,FALSE,&size,&control_word,100000);
    size = 2; ec_SDOread(1,0x6041,0,FALSE,&size,&status_word,100000);
    printf("Control Word: 0x%.4x, status word: 0x%.4x\n", control_word&0xffff,status_word&0xffff);

    return 0;
}

