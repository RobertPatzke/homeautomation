//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Broadcast Socket Interface
// Datei:   socManNetUser.h
// Editor:  Igor Farber, Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//

#ifndef _socManNetUser_h
#define _socManNetUser_h

#define TestAtMFP
// Delete this definition to use the content of (your) socManNetUser.h for
// the parameters of the network.
// Otherwise there is an #include in socManNetUser.h to another file,
// which holds private network parameters and is not delivered with this project


#ifndef TestAtMFP

//---------------------------------------------------------------------------//
//-------------------------------------------------------------------------
// MAC-Adresse des Ethernet-Shield
//-------------------------------------------------------------------------
#define LOCAL_MAC_ADR_B0 0x90
#define LOCAL_MAC_ADR_B1 0xa2
#define LOCAL_MAC_ADR_B2 0xda
#define LOCAL_MAC_ADR_B3 0x0f
#define LOCAL_MAC_ADR_B4 0x62
#define LOCAL_MAC_ADR_B5 0xd0
// MAC-Adresse

#define LOCAL_MAC_ADR_STR (char *) "90-a2-da-0f-62-d0"
// MAC-Adresse als String

#define LOCAL_PORT 4100
// Portnummer lokal

#define BROADCAST_PORT 4100
// Portnummer Rundruf

//-------------------------------------------------------------------------
// Sub-Netz-Maske
//-------------------------------------------------------------------------
#define SUB_NET_MASK_B0 255
#define SUB_NET_MASK_B1 255
#define SUB_NET_MASK_B2 255
#define SUB_NET_MASK_B3 0

//----------------------------------------------------------------------------
// IP-Adresse, ist auf das verwendete Netzwerk anzupassen (kein DHCP)
//----------------------------------------------------------------------------


// IP-Lokal
// ----------------------------------------------
//
#define LOCAL_IP_B0 192
#define LOCAL_IP_B1 168
#define LOCAL_IP_B2 99
#define LOCAL_IP_B3 205

#define LOCAL_IP_ADR_STR (char *) "192.168.99.205"

// IP-Broadcast
// ----------------------------------------------
//
#define BROADCAST_IP_B0 192
#define BROADCAST_IP_B1 168
#define BROADCAST_IP_B2 99
#define BROADCAST_IP_B3 255

// IP-Gateway
// ----------------------------------------------
//
#define GATEWAY_IP_B0 192
#define GATEWAY_IP_B1 168
#define GATEWAY_IP_B2 99
#define GATEWAY_IP_B3 1


/******************************************************************************
 * Netzwerkname
 * Passwort
 */
#define SMNSSID "MPZ-Labor"    //Netzwerk Name
#define SMNPASS "MPZMPZMPZ"    //Netzwerk Passwort

// IP-primaryDNS
// ----------------------------------------------
//
#define PRIMDNS_IP_B0   8
#define PRIMDNS_IP_B1   8
#define PRIMDNS_IP_B2   8
#define PRIMDNS_IP_B3   8

// IP-secondaryDNS
// ----------------------------------------------
//
#define SECDNS_IP_B0    8
#define SECDNS_IP_B1    8
#define SECDNS_IP_B2    4
#define SECDNS_IP_B3    4

#else
  #undef _socManNetUser_h
  #include "socManNetMFP.h"
#endif // TestAtMFP

//---------------------------------------------------------------------------//
#endif // _socManNetUser_h
