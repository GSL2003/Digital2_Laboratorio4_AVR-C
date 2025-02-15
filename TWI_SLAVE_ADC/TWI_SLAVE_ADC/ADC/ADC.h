/*
 * ADC.h
 *
 * Created: 2/13/2025 7:14:45 PM
 *  Author: gaadd
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

void ADC_init(void);					//Prototipo de función para configruar el ADC

uint16_t ADC_read(uint8_t canal);		//Prototipo de función para leer los canales del ADC

#endif /* ADC_H_ */