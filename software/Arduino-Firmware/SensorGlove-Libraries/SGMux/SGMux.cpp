//================================================================================================================
//                                                   SensorGlove_Mux-Code
//================================================================================================================

/*
	SGMux ermöglichst das Auslesen von analogen Werten aus einem der 15 Channel.
	Created by Paul Weber, 2015.07
	Freie Nutzung
*/

#include "SGMux.h"
#include "Arduino.h"


SGMux::SGMux(int Sig, int En, int S0, int S1, int S2, int S3)
{
	//Konstanten festlegen
	_muxPin_Sig=Sig;
	_muxPin_En=En;
	_muxPin_S0=S0;
	_muxPin_S1=S1;
	_muxPin_S2=S2;
	_muxPin_S3=S3;
	
	int _cPin [4] = {_muxPin_S0, _muxPin_S1, _muxPin_S2, _muxPin_S3};
	int _mChannel [16] [4]={
	{0,0,0,0}, //channel 0
	{1,0,0,0}, //channel 1
	{0,1,0,0}, //channel 2
	{1,1,0,0}, //channel 3
	{0,0,1,0}, //channel 4
	{1,0,1,0}, //channel 5
	{0,1,1,0}, //channel 6
	{1,1,1,0}, //channel 7
	{0,0,0,1}, //channel 8
	{1,0,0,1}, //channel 9
	{0,1,0,1}, //channel 10
	{1,1,0,1}, //channel 11
	{0,0,1,1}, //channel 12
	{1,0,1,1}, //channel 13
	{0,1,1,1}, //channel 14
	{1,1,1,1}  //channel 15
	};
	
	for (int _i=0; _i<4; _i++){
		_controlPin [_i] = _cPin [_i];
	}
	
	for (int _i=0; _i<16; _i++){
		for (int _j=0; _j<4; _j++){
			_muxChannel [_i][_j] = _mChannel [_i][_j];
		}
	}

	//PinModi
	pinMode(_muxPin_Sig,INPUT);
	pinMode(_muxPin_En,OUTPUT);

	pinMode(_muxPin_S0,OUTPUT);
	pinMode(_muxPin_S1,OUTPUT);
	pinMode(_muxPin_S2,OUTPUT);
	pinMode(_muxPin_S3,OUTPUT);

	//Pins vorbereiten
	digitalWrite(_muxPin_S0,LOW);
	digitalWrite(_muxPin_S1,LOW);
	digitalWrite(_muxPin_S2,LOW);
	digitalWrite(_muxPin_S3,LOW);

	digitalWrite(_muxPin_En,LOW);				//Mux deaktivieren
}


int SGMux::read(int _channel)
{
	//4 Ausgangspins werden nach binärer Logik beschrieben. Dadurch wird der entsprechende Analog-Channel am Sig_Pin durchgeschaltet
	for(int _i = 0; _i < 4; _i ++){
	digitalWrite(_controlPin[_i], _muxChannel[_channel][_i]);
	}

	//Sig_Pin wird ausgelesen
	int _val = analogRead(_muxPin_Sig);

	//Wert ausgeben
	return _val; 
}



void SGMux::on()
{
	//Mux aktivieren
	digitalWrite(_muxPin_En,LOW); 
}



void SGMux::off()
{
	//Mux deaktivieren
	digitalWrite(_muxPin_En,HIGH);
}