//================================================================================================================
//                                                   SensorGlove_Mux-Header
//================================================================================================================

/*
	SGMux ermöglichst das Auslesen von analogen Werten aus einem der 15 Channel.
	Created by Paul Weber, 2015.07
	Freie Nutzung
*/


#ifndef SGMux_h
#define SGMux_h

#include "Arduino.h"


class SGMux
{
	public:
		SGMux(int Sig, int En, int S0, int S1, int S2, int S3);		//Constrcutor "SGMux" mit Definition der Anschlüsse am Arduino
		int read(int channel);										//SGMux.read(channel) liest den analogen Wert aus
		void on();
		void off();
	
	
	private:
		int _muxPin_Sig;
		int _muxPin_En;
		int _muxPin_S0;
		int _muxPin_S1;
		int _muxPin_S2;
		int _muxPin_S3;
		
		int _controlPin[4];
		int _muxChannel[16][4];	

};
#endif
