/**************************************************************************/
/*!
    @file     PCAL6524.cpp
    @author   Jason Johnston
 
	This is a driver for NXP's PCAL6254 I2C GPIO
	This chip has 24 Individual GPIO that can be indepenenly set as Input or Ouput
    
	v1.0 - First release
	
	Please note this library is derived from Adafruits ADS1015/ADS1115 library.
*/
/**************************************************************************/
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#ifndef PCAL6524_h
	#define PCAL6524_h
		#include "PCAL6254.h"
#endif
/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static uint8_t i2cread(void) {
  #if ARDUINO >= 100
  return Wire.read();
  #else
  return Wire.receive();
  #endif
}

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static void i2cwrite(uint8_t x) {
  #if ARDUINO >= 100
  Wire.write((uint8_t)x);
  #else
  Wire.send(x);
  #endif
}


/**************************************************************************/
/*!
    @brief  Resets the CHIP To its defualt state clearing all registers
*/
/**************************************************************************/
static void restart()
{
	Wire.beginTransmission(0x00);
	i2cwrite((uint8_t)0x06);
	Wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  Writes 8-bits to the specified destination register
*/
/**************************************************************************/


static void writeRegister(uint8_t i2cAddress, uint8_t reg, uint8_t value) 
{
  Wire.beginTransmission(i2cAddress);
  i2cwrite((uint8_t)reg);
  i2cwrite((uint8_t)(value));
  Wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief reads 8-bits from the specified register
*/
/**************************************************************************/
static uint8_t readRegister(uint8_t i2cAddress, uint8_t reg) 
{
  Wire.beginTransmission(i2cAddress);
  i2cwrite(reg);
  Wire.endTransmission();
  Wire.requestFrom(i2cAddress, (uint8_t)1);
  return i2cread(); 
}


/**************************************************************************/
/*!
    @brief  Instantiates a new PCAL6524 class w/appropriate properties
*/
/**************************************************************************/

PCAL6524::PCAL6524(uint8_t i2cAddress) 
{
 m_i2cAddress = i2cAddress;
}

/**************************************************************************/
/*!
    @brief  Sets up the HW
*/
/**************************************************************************/
void PCAL6524::begin() {
  Wire.begin();
  restart();
}



/**************************************************************************/
/*!
    @brief  Allows for induvidaul Pins to setup as input or output and select if input has Pull Up or pull down internal resistors
*/
/**************************************************************************/


void PCAL6524::remotepinMode(uint16_t pin, uint8_t mode)
{
	uint8_t config_data;
	uint8_t pullup_config_data;
	uint8_t pullup_value_data;	
	uint8_t PCAL6524_CONFIGURATION;
	uint8_t PCAL6524_RESISTOR_PULL_ENABLE;
	uint8_t PCAL6524_RESISTOR_PULL_SELECTION;
	//Determine wich bank of pins the requested pin is in
	if (pin == P0_0 || pin == P0_1 || pin == P0_2 || pin == P0_3 || pin == P0_4 || pin == P0_5 || pin == P0_6 ||pin == P0_7) //If pin is in the First bank
	{
		PCAL6524_CONFIGURATION = PCAL6524_CONFIGURATION_PORT_0;
		PCAL6524_RESISTOR_PULL_ENABLE = PCAL6524_RESISTOR_PULL_ENABLE_PORT_0;
		PCAL6524_RESISTOR_PULL_SELECTION = PCAL6524_RESISTOR_PULL_SELECTION_PORT_0;
	}
	else if (pin == P1_0 || pin == P1_1 || pin == P1_2 || pin == P1_3 ||pin == P1_4 || pin == P1_5 || pin == P1_6 || pin == P1_7) //If pin is in the Second bank
	{
		PCAL6524_CONFIGURATION = PCAL6524_CONFIGURATION_PORT_1;
		PCAL6524_RESISTOR_PULL_ENABLE = PCAL6524_RESISTOR_PULL_ENABLE_PORT_1;
		PCAL6524_RESISTOR_PULL_SELECTION = PCAL6524_RESISTOR_PULL_SELECTION_PORT_1;
	}
	else if (pin == P2_0 || pin == P2_1 || pin == P2_2 || pin == P2_3 || pin == P2_4 || pin == P2_5 || pin == P2_6 || pin == P2_7) //If pin is in the Third bank
	{
		PCAL6524_CONFIGURATION = PCAL6524_CONFIGURATION_PORT_2;
		PCAL6524_RESISTOR_PULL_ENABLE = PCAL6524_RESISTOR_PULL_ENABLE_PORT_2;
		PCAL6524_RESISTOR_PULL_SELECTION = PCAL6524_RESISTOR_PULL_SELECTION_PORT_2;
	}
	//read the current Input/output configuration settings for the given bank of pins
	config_data = readRegister(m_i2cAddress,PCAL6524_CONFIGURATION);
	//read the current pullup enable configuration settings for the given bank of pins
	pullup_config_data = readRegister(m_i2cAddress,PCAL6524_RESISTOR_PULL_ENABLE);
	//read the current pullup enable configuration settings for the given bank of pins
	pullup_value_data = readRegister(m_i2cAddress,PCAL6524_RESISTOR_PULL_SELECTION);
	/*This section is used to determine what mode of operation is selected
		Input
		Output
		Input With PullUp Resistor Active
		Input with PullDown Resistor Active	
	*/
	if (mode == INPUT)
	{
		//Combine the current configuration with the request pin to ensure that only that pin has chaned
		config_data = config_data | (uint8_t)pin;  
		//Write the new configuration back to the Resistor
		writeRegister(m_i2cAddress,PCAL6524_CONFIGURATION,config_data);  //Write the new configuration back to the Resistor
		return;
	}
	else if (mode == OUTPUT)
	{
		//Combine the current configuration with the request pin to ensure that only that pin has chaned
		config_data = config_data ^ (uint8_t)pin;//Combine the current configuration with the request pin to ensure that only that pin has chaned
		//Write the new configuration back to the Resistor
		writeRegister(m_i2cAddress,PCAL6524_CONFIGURATION,config_data);  //Write the new configuration back to the Resistor
		return;
	}
	else if (mode == INPUT_PULLUP)
	{
		//Combine the current configuration with the request pin to ensure that only that pin has chaned
		config_data = config_data | (uint8_t)pin;
		//Write the new configuration back to the Resistor
		writeRegister(m_i2cAddress,PCAL6524_CONFIGURATION,config_data);//Write the new configuration back to the Resistor
		//This is used to configure the pullup/down resistor are actived and configured correctly
		pullup_config_data = pullup_config_data | (uint8_t)pin;
		writeRegister(m_i2cAddress,PCAL6524_RESISTOR_PULL_ENABLE,pullup_config_data);
		pullup_value_data = pullup_value_data | (uint8_t)pin;
		writeRegister(m_i2cAddress,PCAL6524_RESISTOR_PULL_SELECTION,pullup_value_data);
		return;
	}
	else if (mode == INPUT_PULLDOWN)
	{
		//Combine the current configuration with the request pin to ensure that only that pin has chaned
		config_data = config_data | pin;
		//Write the new configuration back to the Resistor
		writeRegister(m_i2cAddress,PCAL6524_CONFIGURATION,config_data);//Write the new configuration back to the Resistor
		//This is used to configure the pullup/down resistor are actived and configured correctly
		pullup_config_data = pullup_config_data | (uint8_t)pin;
		writeRegister(m_i2cAddress,PCAL6524_RESISTOR_PULL_ENABLE,pullup_config_data);
		pullup_value_data = pullup_value_data ^ (uint8_t)pin;
		writeRegister(m_i2cAddress,PCAL6524_RESISTOR_PULL_SELECTION,pullup_config_data);
		return;
	}
	return;
	}

/**************************************************************************/
/*!
    @brief  Allows for induvidaul Pins desiginated as outputs to be set as High or Low
*/
/**************************************************************************/
void PCAL6524::remotedigitalWrite(uint16_t pin, uint8_t mode){
	uint8_t output_reg_value;
	uint8_t PCAL6524_OUTPUT;
	
	//Determine wich bank of pins the requested pin is in
	if (pin == P0_0 || pin == P0_1|| pin == P0_2 || pin == P0_3 || pin == P0_4 || pin == P0_5 || pin == P0_6 || pin == P0_7)
	{
		PCAL6524_OUTPUT = PCAL6524_OUTPUT_PORT_0;
	}
	else if (pin == P1_0 || pin == P1_1 || pin == P1_2 || pin == P1_3 || pin == P1_4 || pin == P1_5 || pin == P1_6 || pin == P1_7)
	{
		PCAL6524_OUTPUT = PCAL6524_OUTPUT_PORT_1;
	}
	else if (pin == P2_0 || pin == P2_1 || pin == P2_2 || pin == P2_3 || pin == P2_4 || pin == P2_5 || pin == P2_6 || pin == P2_7)
	{
		PCAL6524_OUTPUT = PCAL6524_OUTPUT_PORT_2;
	}
	
	// Read the currnt Value of out the ouput register
	output_reg_value = readRegister(m_i2cAddress,PCAL6524_OUTPUT);
	//Deterime if Pin is being asked to go hi or to go low and set only that pins value;
	if (mode == HIGH)
	{
		output_reg_value = output_reg_value | (uint8_t)pin;
		writeRegister(m_i2cAddress,PCAL6524_OUTPUT,output_reg_value);
		return;
	}
	else if (mode == LOW){
		output_reg_value = output_reg_value & ~((uint8_t)pin);
		writeRegister(m_i2cAddress,PCAL6524_OUTPUT,output_reg_value);
		return;
	}
	return;	
}

uint8_t PCAL6524::remotedigitalRead(uint16_t pin)
{
	uint8_t input_reg_data;
	uint8_t PCAL6524_INPUT;
	//Determine wich bank of pins the requested pin is in
if (pin == P0_0 || pin == P0_1 || pin == P0_2 || pin == P0_3 || pin == P0_4 || pin == P0_5 || pin == P0_6 || pin == P0_7)
	{
		PCAL6524_INPUT = PCAL6524_INPUT_PORT_0;
	}
	else if (pin == P1_0 || pin == P1_1 || pin == P1_2 || pin == P1_3 || pin == P1_4 || pin == P1_5 || pin == P1_6 || pin == P1_7)
	{
		PCAL6524_INPUT = PCAL6524_INPUT_PORT_1;
	}
	else if (pin == P2_0 || pin == P2_1 || pin == P2_2 || pin == P2_3 || pin == P2_4 || pin == P2_5 || pin == P2_6 || pin == P2_7)
	{
		PCAL6524_INPUT = PCAL6524_INPUT_PORT_2;
	}
	//read the input register data
	input_reg_data = readRegister(m_i2cAddress,PCAL6524_INPUT);
	//Isolate the reqested pin value from all other values
	input_reg_data = input_reg_data & (uint8_t)pin;
	
	//Bit Shift the resulting data over so the pin's requested value becomes the LSB
	switch ((uint8_t)pin)
	{
		case (0x01):  //For Pins Px_0
		{
			return input_reg_data;
		}
		case (0x02)://For Pins Px_1
		{
			return input_reg_data>>1;
		}
		case (0x04)://For Pins Px_2
		{
			return input_reg_data>>2;
		}
		case (0x08)://For Pins Px_3
		{
			return input_reg_data>>3;
		}
		case (0x10)://For Pins Px_4
		{
			return input_reg_data>>4;
		}
		case (0x20)://For Pins Px_5
		{
			return input_reg_data>>5;
		}
		case (0x40)://For Pins Px_6
		{
			return input_reg_data>>6;
		}
		case (0x80)://For Pins Px_7
		{
			return input_reg_data>>7;
		}
	}
}

void PCAL6524::pinSetInterruptEnabled(uin16_t pin, bool enabled)
{
	uint8_t interrupt_mask_reg_data;
	uint8_t PCAL6524_INTERRUPT_PORT;
	
	//Determine wich bank of pins the requested pin is in
	if (pin == P0_0 || pin == P0_1|| pin == P0_2 || pin == P0_3 || pin == P0_4 || pin == P0_5 || pin == P0_6 || pin == P0_7)
	{
		PCAL6524_INTERRUPT_PORT = PCAL6524_INTERRUPT_MASK_PORT_0;
	}
	else if (pin == P1_0 || pin == P1_1 || pin == P1_2 || pin == P1_3 || pin == P1_4 || pin == P1_5 || pin == P1_6 || pin == P1_7)
	{
		PCAL6524_INTERRUPT_PORT = PCAL6524_INTERRUPT_MASK_PORT_1;
	}
	else if (pin == P2_0 || pin == P2_1 || pin == P2_2 || pin == P2_3 || pin == P2_4 || pin == P2_5 || pin == P2_6 || pin == P2_7)
	{
		PCAL6524_INTERRUPT_PORT = PCAL6524_INTERRUPT_MASK_PORT_2;
	}

	//Read the current Value of the interrupt mask register
	interrupt_mask_reg_data = readRegister(m_i2cAddress, PCAL6524_INTERRUPT_PORT);
	if (enabled == false)
	{
		interrupt_mask_reg_data = interrupt_mask_reg_data | (uint8_t)pin;
		writeRegister(m_i2cAddress, PCAL6524_INTERRUPT_PORT, interrupt_mask_reg_data);
		return;
	}
	else if (enabled == true){
		interrupt_mask_reg_data = interrupt_mask_reg_data & ~((uint8_t)pin);
		writeRegister(m_i2cAddress, PCAL6524_INTERRUPT_PORT, interrupt_mask_reg_data);
		return;
	}
} 

uint16_t PCAL6524::getLastInterruptPin()
{
	uint8_t intf;

	intf = readRegister(m_i2cAddress, PCAL6524_INTERRUPT_STATUS_PORT_0);
	for (int i = 0; i < 8; i++) {
		if (pca_bitRead(intf, i))
		{
			return convertNumberToPin(i);
		}
	}
	intf = readRegister(m_i2cAddress, PCAL6524_INTERRUPT_STATUS_PORT_1);
	for (int i = 0; i < 8; i++)
	{
		if (pca_bitRead(intf, i))
		{
			return convertNumberToPin(i + 8);
		}
	}
	intf = readRegister(m_i2cAddress, PCAL6524_INTERRUPT_STATUS_PORT_2);
	for (int i = 0; i < 8; i++)
	{
		if (pca_bitRead(intf, i))
		{
			return convertNumberToPin(i + 16);
		}
	}
	return INT_ERR;
}

uint8_t PCAL6524::getInterruptPinValue()
{
	uint16_t intPin = getInterruptPinValue();
	if (intPin != INT_ERR)
	{
		return remotedigitalRead(intPin);
	}
	return (uint8_t) INT_ERR;
}

uint8_t PCAL6524::pca_bitRead(uint8_t value, int bit)
{
	return (((value) >> (bit)) & 0x01);
}

uint16_t PCAL6524::convertNumberToPin(uint8_t pin_no)
{
	switch (pin_no)
	{
	case 0:
		return P0_0;
	case 1:
		return P0_1;
	case 2:
		return P0_2;
	case 3:
		return P0_3;
	case 4:
		return P0_4;
	case 5:
		return P0_5;
	case 6:
		return P0_6;
	case 7:
		return P0_7;
	case 8:
		return P1_0;
	case 9:
		return P1_1;
	case 10:
		return P1_2;
	case 11:
		return P1_3;
	case 12:
		return P1_4;
	case 13:
		return P1_5;
	case 14:
		return P1_6;
	case 15:
		return P1_7;
	case 16:
		return P2_0;
	case 17:
		return P2_1;
	case 18:
		return P2_2;
	case 19:
		return P2_3;
	case 20:
		return P2_4;
	case 21:
		return P2_5;
	case 22:
		return P2_6;
	case 23:
		return P2_7;
	default:
		return 0xFFFF;
	}
}