#include <Arduino.h>

#include "RHGpioLed.h"

RHGpioLed::RHGpioLed (int pinNumber, bool polarity) :
	RHLed (polarity), m_pinNumber (pinNumber)
{
}

//Effectue les opérations d'initialisation des ressources matérielles.
void RHGpioLed::begin()
{
	pinMode (m_pinNumber, OUTPUT);
	setState (false);
}

//Lecture de l'état binaire de la led.
//true si allumée.
bool RHGpioLed::state() const
{
	return digitalRead (m_pinNumber) == polarity();
}

//Modifie l'état allumée / éteinte
//true pour allumée.
void RHGpioLed::setState (bool on)
{
	digitalWrite (m_pinNumber, on ^ ! polarity());
}

//Bascule de l'état binaire d'une led.
//Si elle était éteinte, elle s'allume.
//Si elle était allumée, elle s'éteint.

void RHGpioLed::toggleState()
{
	setState (!state());
}
