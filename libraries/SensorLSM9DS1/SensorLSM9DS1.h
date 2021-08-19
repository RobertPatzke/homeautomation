//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   SensorLSM9DS1.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//

#ifndef SENSORLSM9DS1_H
#define SENSORLSM9DS1_H

#include "Arduino.h"
#include "arduinoDefs.h"
#include "IntrfTw.h"

// ----------------------------------------------------------------------------

// ------------------------ Acceleration and Gyroscope -------
#define AG_Adr    0x6B
// ------------------------ Acceleration and Gyroscope -------
#define AG_Ctrl1  0x10
#define AG_Ctrl6  0x20
#define AG_Ctrl8  0x22
#define AG_Id     0x0F

#define AG_Rate(x)      (x << 5)
#define AG_Odr14_9      0x20
#define AG_Odr59_5      0x40
#define AG_Odr119       0x60
#define AG_Odr238       0x80
#define AG_Odr476       0xA0
#define AG_Odr952       0xC0

#define AG_FullScale(x) (x << 3)
#define A_Fs2g          0x00
#define A_Fs4g          0x10
#define A_Fs8g          0x18
#define A_Fs16g         0x08
#define G_Fs245         0x00
#define G_Fs2000        0x18
#define G_Fs500         0x08

#define AG_LowPass(x)   (x)
#define A_LpAuto        0x00
#define A_Lp50          0x07
#define A_Lp105         0x06
#define A_Lp211         0x05
#define A_Lp408         0x04
#define G_LpLL          0x00
#define G_LpLH          0x01
#define G_LpHL          0x02
#define G_LpHH          0x03

// ------------------------ Magnetic Field -------
#define M_Adr     0x1E
// ------------------------ Magnetic Field -------
#define M_Ctrl1   0x20
#define M_Ctrl2   0x21
#define M_Id      0x0F



class SensorLSM9DS1
{
private:
  // --------------------------------------------------------------------------
  // Lokale Daten und Funktionen
  // --------------------------------------------------------------------------
  //
  IntrfTw *twPtr;
  TwiByte twiByte;

public:
  // --------------------------------------------------------------------------
  // Initialisierungen der Basis-Klasse
  // --------------------------------------------------------------------------

  SensorLSM9DS1(IntrfTw *refI2C);

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  int resetAG();
  int resetM();
  int reset();

  void setScanAG(byte scValueAG, byte scValueA, byte scValueG);
  // Messparameter für Accel und Gyro
  // scValueAG = Abtastrate
  // scValueA  = Vollausschlag und Tiefpass für Beschleunigung
  // scValueB  = Vollausschlag und Tiefpass für Gyrometer

  void begin();

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //

  // --------------------------------------------------------------------------
  // Datenaustausch
  // --------------------------------------------------------------------------
  //


  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //

  // ----------------------------------------------------------------------------
  //                      D e b u g - H i l f e n
  // ----------------------------------------------------------------------------
  //

};

#endif // SENSORLSM9DS1_H

