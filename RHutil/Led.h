#ifndef RH_LED_H
#define RH_LED_H


class Led
{
public:

	Led (bool polarity = true) :
		m_polarity (polarity)
	{
	}

	//Effectue les opérations d'initialisation des ressources matérielles.
	virtual void begin() = 0;

	//Polarité de la led
	//Indique par quelle broche la led est commandée.
	//true si la led est commandée par son anode.
	//false si la led est commandée par sa cathode.
	inline bool polarity() const
	{
		return m_polarity;
	}

	//Lecture de l'état binaire de la led.
	//true si allumée.
	virtual bool state() const = 0;

	//Modifie l'état allumée / éteinte
	//true pour allumée.
	virtual void setState (bool on) = 0;

	//Bascule de l'état binaire d'une led.
	//Si elle était éteinte, elle s'allume.
	//Si elle était allumée, elle s'éteint.
	virtual void toggleState() = 0;


private:
	bool m_polarity;
};

#endif // RH_LED_H defined
