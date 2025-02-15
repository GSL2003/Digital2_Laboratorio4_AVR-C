//******************************************************************************************************************
//UNIVERSIDAD DEL VALLE DE GUATEMALA
//IE3054: Electrónica Digital 2
//Archivo: Clase_I2C_MAESTRO
//AUTOR: Guillermo José Schwartz López
//Clase I2C
//HARDWARE: ATMEGA328P
//CREADO: 12/02/2025
//ÚLTIMA MODIFICACIÓN: 12/02/2025 - 16:17
//******************************************************************************************************************
#define F_CPU 16000000		// Definir CPU con el que trabajara el microcontrolador
#include <avr/io.h>			// Se incluye la libreria para poder utilizar los puertos del microcontrolador
#include <util/delay.h>		// Se incluye la libreria que implementa los delay


#include <string.h>			//Proporciona funciones para manipular cadenas de caracteres


//Se incluyen las Librerias Propias
#include "I2C/I2C.h"
#include "LCD/LCD.h"

//Se define la dirreción de los esclavos
#define slave1 0x30		//Direción del esclavo del ADC
#define slave2 0x40		//Direción del contador

//Se definen los prototipos de función
void setup_leds(void);		//Se define un prefunción de la conexión de los esclavos

void float_to_string(float num, char *buffer, int precision); //Configura de entero a caracter

//******************************************************************************************************************
//VARIABLES GLOBALES
//******************************************************************************************************************
uint8_t direccion;
uint8_t temp;			//Se define una variable temporal
uint8_t bufferI2C;		//Variable encargada de configurar comunicación: Maestro - Esclavo
uint8_t valorI2C = 0;	//Variable encargada de recibir el dato enviado por el esclavo


//Se establecen las variables del contador y potenciometro
float voltaje_pot = 0;	//Se establece una variable para almacenar el valor del potenciometro recibido
float contador = 0;		//Se establece una variable para almacenar el valor del contador recibido

//Almacenar los valores convertido a texto	
char V_pot[16];			//Variable encargada de almacenar la conversión del potenciometro a caracter
char CONT[16];			//Variable encargada de almacenar la conversión del contador a caracter

//******************************************************************************************************************
//LOOP
//******************************************************************************************************************

int main(void)
{
	I2C_Master_Init(1000,1); //Se inicializa como maestro Fscl 100Hz, prescaler 1
	setup_leds();		//Se llama a la función de leds "Conexión"
	
	initLCD8bits();		//Se inicializa la LCD 16X2
	
	//Se configuran textos iniciales en la LCD
	LCD_Set_Cursor(4, 1);
	LCD_Write_String("S1:  ");
	LCD_Write_String("  S2: ");
    while (1) 
    {
		
		//----> Lectura del Potenciometro 
		PORTB|=(1 << PORTB5);		//Se enciende la LED del ADC
		
		I2C_Master_Start();
		// Escritura
		bufferI2C = slave1 << 1 & 0b11111110;	//Se configura la comunicación con el Esclavo del ADC

		temp = I2C_Master_Write(bufferI2C);
		if(temp != 1){				//En el dado caso de no entablar conexión, se detiene la comunicación

			I2C_Master_Stop();

		}else{		//Si se entabla conexión, se puede seguir la comunicación
			
			I2C_Master_Write('R');
			I2C_Master_Stop();
			
			_delay_ms(500);
			
			//Se apaga la led del ADC para indicar comunicación (Parpadeara cada vez que entre al ciclo "else")
			PORTB &= ~(1 << PORTB5);		
			I2C_Master_Start();
			// Lectura
			bufferI2C = slave1 << 1 | 0b00000001;
			
			temp = I2C_Master_Write(bufferI2C);
			if(temp != 1){		
				I2C_Master_Stop();
			}

			TWCR=(1 << TWINT)|(1 << TWEN);
			while (!(TWCR & (1 << TWINT)));

			valorI2C= TWDR;							//Se almacena el valor de la ADC enviado por el esclavo	

			I2C_Master_Stop();
			
			voltaje_pot = (valorI2C*5.0)/255.0;		//Se convierte el valor de 255 del ADC a 5V
			float_to_string(voltaje_pot, V_pot, 2);	//Se convierte el valor entero a caracter

			LCD_Set_Cursor(3,2);
			LCD_Write_String(V_pot);				//Se envia el valor del caracter a la LCD
			
			LCD_Set_Cursor(7,2);
			LCD_Write_Char('V');
			
		}
		
		
		
		//----> Lectura del Contador
		
		PORTC|=(1 << PORTC0);	//Se enciende la LED del Contador (Permanecera encendida si no hay comunicación)
		
		I2C_Master_Start();
		// Escritura
		bufferI2C = slave2 << 1 & 0b11111110;	//Se configura la comunicación con el Esclavo del Contador

		temp = I2C_Master_Write(bufferI2C);
		if(temp != 1){				//En el dado caso de no entablar conexión, se detiene la comunicación

			I2C_Master_Stop();

			}else{					//Si se entabla conexión, se puede seguir la comunicación
			
			I2C_Master_Write('C');
			I2C_Master_Stop();
			
			_delay_ms(500);

			//Se apaga la led del contador indicacomunicación (Parpadeara cada vez que entre al ciclo "else")
			PORTC &= ~(1 << PORTC0);
			I2C_Master_Start();
			// Lectura
			bufferI2C = slave2 << 1 | 0b00000001;
			
			temp = I2C_Master_Write(bufferI2C);
			if(temp != 1){
				I2C_Master_Stop();
			}

			TWCR=(1 << TWINT)|(1 << TWEN);
			while (!(TWCR & (1 << TWINT)));

			valorI2C= TWDR;						//Se almacena el valor del contador enviado por el esclavo

			I2C_Master_Stop();
			
			contador = valorI2C;				
			float_to_string(contador, CONT, 2);		//Se transforma el valor del contador a caracter

			LCD_Set_Cursor(11,2);
			LCD_Write_String(CONT);
		
		}

    }
}


//******************************************************************************************************************
//FUNCIONES
//******************************************************************************************************************
void setup_leds(void){
	//Configuración de la Led Esclavo 1 - ADC
	DDRB |=(1 << DDB5);			//Se convigura el PB5 como salida
	PORTB &= ~(1 << PORTB5);	//Se apaga PB5
	
	//Configuración de la Led Esclavo 2 - Contador
	DDRC |=(1 << DDC0);			//Se convigura el PC0 como salida
	PORTC &= ~(1 << PORTC0);	//Se apaga PC0
}

void float_to_string(float num, char *buffer, int precision){
	// Parte entera
	int integer_part = (int)num;
	
	// Parte decimal
	float decimal_part = num - integer_part;
	
	// Convertir la parte entera a cadena
	char int_buffer[16]; // Buffer para la parte entera
	snprintf(int_buffer, sizeof(int_buffer), "%d", integer_part);
	
	// Convertir la parte decimal a cadena
	char dec_buffer[16]; // Buffer para la parte decimal
	for (int i = 0; i < precision; i++) {
		decimal_part *= 10;
	}
	
	snprintf(dec_buffer, sizeof(dec_buffer), "%d", (int)decimal_part);
	
	// Combinar ambas partes en el buffer final
	snprintf(buffer, 32, "%s.%s", int_buffer, dec_buffer);
}

