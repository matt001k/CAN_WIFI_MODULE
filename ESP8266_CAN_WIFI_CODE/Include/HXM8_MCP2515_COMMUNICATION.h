#ifndef HXM8_MCP2515_COMMUNICATION
#define HXM8_MCP2515_COMMUNICATION

/*BEGIN USER CONFIGURATION PARAMETERS*/
#define CAN_BUS_TASK_PRIORITY                   15  //PRIORITY OF THE CAN BUS TASK  
/*END USER CONFIGURATION PARAMETERS*/

/*USER DEFINED PREPROCESSOR*/
#define GPIO_INPUT_IO_0                         4
#define GPIO_INPUT_IO_1                         5
#define GPIO_INPUT_PIN_SEL                      ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
/*END USER DEFINED PREPROCESSOR*/








void hxm8_mcp2515_init(void);

#endif