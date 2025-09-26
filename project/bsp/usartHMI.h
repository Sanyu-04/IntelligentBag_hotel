#ifndef __USARTHMI_H
#define __USARTHMI_H

#include "USART1.h"
#include "USART0.h"

void HMI_send_string(char* name, char* showdata);
void HMI_send_number(char* name, int num);
void HMI_send_float(char* name, float num);


#endif

