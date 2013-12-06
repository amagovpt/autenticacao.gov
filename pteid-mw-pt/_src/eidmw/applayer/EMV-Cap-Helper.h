/*
 *  PTeID Middleware Project.
 *  Copyright (C) 2011-2012
 *  Luis Medinas <lmedinas@gmail.com>
 *  Andre Guerreiro <andre.guerreiro@caixamagica.pt>
 */

#ifndef EMVCAPHELPER_H_
#define EMVCAPHELPER_H_

namespace eIDMW
{

#define PANSEQNUMBER "00"
#define CDOL1 "0000000000000000000000000000800000000000000000000000000000"
#define COUNTER "0"
#define PINTRYCOUNTER "3"

struct OTPParams
{
char *pan;
char *pin;
char *arqc;
char *cdol1;
char *atc;
char *pan_seq_nr;
char *counter;
unsigned char pin_try_counter;

};

class EMVCapHelper
{
public:
	EMVCapHelper(APL_Card *card, const char *new_pin);
	/**
	  * Destructor
	  */
	~EMVCapHelper();

	void GetPan();
	void GetArqc(unsigned char p1);
	bool getOtpParams(OTPParams *);
	char *changeCapPin(char * change_apdu_str);
	char *resetScriptCounter(char *cdol2);
	void getOnlineTransactionParams(OTPParams *);
private:
	bool checkSW12(CByteArray &in);
	APL_Card *m_card;
	char *m_pan;
	char *m_arqc;
	char *m_atc;
	unsigned char m_trycounter;
	const char *m_new_pin;
};

}
#endif /* EMVCAPHELPER_H_ */
