#include <Arduino.h>

#include "RHGpioPin.h"

RHGpioPin::RHGpioPin (int pinNumber, bool polarity) :
	RHPin (polarity), m_pinNumber (pinNumber)
{
}

//Effectue les opérations d'initialisation des ressources matérielles.
void RHGpioPin::begin()
{
	pinMode (m_pinNumber, OUTPUT);
	setState (false);
}

//Lecture de l'état binaire de la led.
//true si allumée.
bool RHGpioPin::state() const
{
	return digitalRead (m_pinNumber) == polarity();
}

//Modifie l'état allumée / éteinte
//true pour allumée.
void RHGpioPin::setState (bool on)
{
	digitalWrite (m_pinNumber, on ^ ! polarity());
}

//Bascule de l'état binaire d'une led.
//Si elle était éteinte, elle s'allume.
//Si elle était allumée, elle s'éteint.

void RHGpioPin::toggleState()
{
	setState (!state());
}
