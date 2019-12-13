#ifndef RH_GPIOLED_H
#define RH_GPIOLED_H


#include "Led.h"

class GpioLed : public Led
{
public:
	GpioLed (int pin, bool polarity = true);


private:
	int m_pinNumber;


public:
	//Numéro de broche GPIO à laquelle la led est connectée.
	inline int pinNumber() const
	{
		return m_pinNumber;
	}

	//Lecture de l'état binaire de la led.
	//true si allumée.
	virtual bool state() const;

	//Modifie l'état allumée / éteinte
	//true pour allumée.
	virtual void setState (bool on);

	//Bascule de l'état binaire d'une led.
	//Si elle était éteinte, elle s'allume.
	//Si elle était allumée, elle s'éteint.
	virtual void toggleState();

	//Effectue les opérations d'initialisation des ressources matérielles.
	virtual  void begin();

};
#endif
