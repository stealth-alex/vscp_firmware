 /**
 * @brief           VSCP Level II common functionality
 * @file            vscp_firmware_level2.h
 * @author          Ake Hedman, eurosource, <a href="www.vscp.org">VSCP Project</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_vscp
 *
 *
 * @section description Description
 **********************************
 * This module contains the code that implements that 
 *
 *********************************************************************/

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2015 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
*/

#ifndef VSCP_FIRMWARE_LEVEL2_H
#define VSCP_FIRMWARE_LEVEL2_H


#include <vscp_compiler.h>
#include <vscp_projdefs.h>

// * * * Constants * * *


#include <inttypes.h>


// ******************************************************************************
//  	VSCP Constants
// ******************************************************************************

#define VSCP_MAJOR_VERSION          1
#define VSCP_MINOR_VERSION          5

#define	VSCP_LEVEL2_UDP_PORT        9598
#define	VSCP_LEVEL2_TCP_PORT        9598

#define VSCP_SIZE_GUID              16      // # GUID bytes
#define VSCP_SIZE_DEVURL            32      // # of device URL bytes

#define LIMITED_DEVICE_DATASIZE     128     // This is the max size used for
                                            // a limited data Level II unit.
                                            // VSCP_LEVEL2_LIMITED_DEVICE should
                                            // be define in vscp_projdefs.h for
                                            // it to be used

#define VSCP_BOOT_FLAG              0xff    // Boot flag is stored in persistent storage
                                            // and if it is there the bootloader will be
                                            // activated.
// Bootloaders
#define VSCP_BOOTLOADER_VSCP        0x00    // VSCP bootloader algorithm
#define VSCP_BOOTLOADER_PIC1        0x01    // PIC
#define VSCP_BOOTLOADER_LPC1        0x10    // NXP LPC algortithm
#define VSCP_BOOTLOADER_NONE        0xff    // No bootloader

// State machine states 
#define VSCP_STATE_STARTUP          0x00	// Cold/warm reset
#define VSCP_STATE_INIT             0x01	// Assigning nickname
#define VSCP_STATE_PREACTIVE        0x02	// Waiting for host initialixation
#define VSCP_STATE_ACTIVE           0x03	// The normal state
#define VSCP_STATE_ERROR            0x04	// error state. Big problems.

// State machine sub states 
#define VSCP_SUBSTATE_NONE          0x00	// No state


// VSCP message

// This structure is for VSCP Level II
typedef struct _vscpEvent {

// CRC should be calculated from
// here to end + datablock
    uint8_t head;           // bit 765 prioriy, Priority 0-7 where 0 is highest.
                            // bit 4 = hardcoded, true for a hardcoded device.
                            // bit 3 = Dont calculate CRC, false for CRC usage.
    uint16_t  vscp_class;   // VSCP class
    uint16_t  vscp_type;    // VSCP type
    uint8_t   GUID[ 16 ];   // Node address MSB -> LSB
    uint16_t  sizeData;     // Number of valid data bytes
	
    // Pointer to data. Normally Max 487 (512- 25) bytes
    // but can be restrictedto 8 bytes. This means that
    // that all Level II events will not be handled
    // on a low-end Level II device.
#ifdef VSCP_LEVEL2_LIMITED_DEVICE	
    uint8_t  data[ LIMITED_DEVICE_DATASIZE ];
#else
    uint8_t  data[ 512- 25 ];
#endif

 uint16_t  crc;             // crc checksum
	
} vscpEvent;


typedef vscpEvent *PVSCPEVENT;


#define VSCP_PRIORITY_0				0x00
#define VSCP_PRIORITY_1				0x20
#define VSCP_PRIORITY_2				0x40
#define VSCP_PRIORITY_3				0x60
#define VSCP_PRIORITY_4				0x80
#define VSCP_PRIORITY_5				0xA0
#define VSCP_PRIORITY_6				0xC0
#define VSCP_PRIORITY_7				0xE0

#define VSCP_PRIORITY_HIGH			0x00
#define VSCP_PRIORITY_MEDIUM                    0x80
#define VSCP_PRIORITY_NORMAL                    0x80
#define VSCP_PRIORITY_LOW			0xE0

#define VSCP_MASK_PRIORITY			0xE0
#define VSCP_MASK_HARDCODED			0x10
#define VSCP_MASK_NOCRCCALC			0x08

#define VSCP_LEVEL1_MAXDATA			8
#define VSCP_LEVEL2_MAXDATA			(512 - 25)


//
// Descision Matrix row
//

typedef struct _vscp_DMatrixRow {

    uint32_t mask;	// Mask ( class + type )
    uint32_t filter;	// Filter (class + type )
									
    uint32_t control;	// Control word
			//  Bit 31 - Enabled (if == 1).
			//  Bit 30 -- Match GUID /beginnig of param field 0-15)
			//  But 4  -- Match Zone.
			//  Bit 3  -- Match Subzone.
	
    uint16_t action;	// Action code

    uint8_t action_params[ VSCP_ACTION_PARAM_SIZE ];	// Action parameter
                                                        // Should be define in vscp_projdefs.h
	
} vscp_DMatrixRow;

// DM positions
#define VSCP_DM_POS_MASK			0
#define VSCP_DM_POS_FILTER			4
#define VSCP_DM_POS_CONTROL			8
#define VSCP_DM_POS_ACTION			12
#define VSCP_DM_POS_PARAM			14

// DM Control flags
#define VSCP_DM_CONTROL_ENABLED			0x80000000
#define VSCP_DM_CONTROL_MATCH_GUID		0x40000000
#define VSCP_DM_CONTROL_MATCH_GUID		0x40000000
#define VSCP_DM_CONTROL_MATCH_ZONE		0x00000010
#define VSCP_DM_CONTROL_MATCH_SUBZONE           0x40000008

// VSCP LEVEL II UDP datagram offsets
#define VSCP2_UDP_POS_HEAD			0
#define VSCP2_UDP_POS_CLASS			1
#define VSCP2_UDP_POS_TYPE			3
#define VSCP2_UDP_POS_GUID			5
#define VSCP2_UDP_POS_SIZE			21

#define VSCP2_UDP_POS_DATA			23  // Holder for beginning of data
#define VSCP2_UDP_POS_CRC			25  // dummy: actual is len - 2


// ******************************************************************************
//  	VSCP Register
// ******************************************************************************

#define  VSCP_LEVEL2_COMMON_REGISTER_START	0xffffff80

#define VSCP_REG_ALARMSTATUS			0xffffff80
#define VSCP_REG_VSCP_MAJOR_VERSION		0xffffff81
#define VSCP_REG_VSCP_MINOR_VERSION		0xffffff82

#define VSCP_REG_NODE_CONTROL			0xffffff83

#define VSCP_REG_USERID0			0xffffff84
#define VSCP_REG_USERID1			0xffffff85
#define VSCP_REG_USERID2			0xffffff86
#define VSCP_REG_USERID3			0xffffff87
#define VSCP_REG_USERID4			0xffffff88

#define VSCP_REG_MANUFACTUR_ID0			0xffffff89
#define VSCP_REG_MANUFACTUR_ID1			0xffffff8A
#define VSCP_REG_MANUFACTUR_ID2			0xffffff8B
#define VSCP_REG_MANUFACTUR_ID3			0xffffff8C

#define VSCP_REG_MANUFACTUR_SUBID0		0xffffff8D
#define VSCP_REG_MANUFACTUR_SUBID1		0xffffff8E
#define VSCP_REG_MANUFACTUR_SUBID2		0xffffff8F
#define VSCP_REG_MANUFACTUR_SUBID3		0xffffff90

#define VSCP_REG_NICKNAME_ID			0xffffff91

#define VSCP_REG_PAGE_SELECT_MSB		0xffffff92
#define VSCP_REG_PAGE_SELECT_LSB		0xffffff93

#define VSCP_REG_FIRMWARE_MAJOR_VERSION		0xffffff94
#define VSCP_REG_FIRMWARE_MINOR_VERSION		0xffffff95
#define VSCP_REG_FIRMWARE_SUB_MINOR_VERSION	0xffffff96

#define VSCP_REG_BOOT_LOADER_ALGORITHM		0xffffff97 

#define VSCP_REG_BUFFER_SIZE			0xffffff98

#define VSCP_REG_GUID				0xffffffD0
#define VSCP_REG_GUID0				0xffffffD0
#define VSCP_REG_GUID1				0xffffffD1
#define VSCP_REG_GUID2				0xffffffD2
#define VSCP_REG_GUID3				0xffffffD3
#define VSCP_REG_GUID4				0xffffffD4
#define VSCP_REG_GUID5				0xffffffD5
#define VSCP_REG_GUID6				0xffffffD6
#define VSCP_REG_GUID7				0xffffffD7
#define VSCP_REG_GUID8				0xffffffD8
#define VSCP_REG_GUID9				0xffffffD9
#define VSCP_REG_GUID10                         0xffffffDA
#define VSCP_REG_GUID11				0xffffffDB
#define VSCP_REG_GUID12                         0xffffffDC
#define VSCP_REG_GUID13                         0xffffffDD
#define VSCP_REG_GUID14				0xffffffDE
#define VSCP_REG_GUID15				0xffffffDF

#define VSCP_REG_DEVICE_URL			0xffffffE0

// VSCP function flags
#define VSCP_FUNCTION_USE_TCP			0x01	// Use TCP or UDP
#define VSCP_FUNCTION_AUTODISCOVER		0x02	// If TCP enabled serach server
							// with UDP High End Discovery before 
							// trying to connect
#define VSCP_FUNCTION_FALLBACK			0x04	// If TCP is enabled fallback to UDP on
							// failure.
#define VSCP_FUNCTION_HARDCODED_IPADDR		0x08	// Use a hard TCP/IP address for the
							// VSCP TCP server.
#define VSCP_FUNCTION_PROBE_SENT		0x10  	// High end server probe sent
#define VSCP_FUNCTION_RXOBJ_IN_USE		0x40  	// TCP has receibed a frame
#define VSCP_FUNCTION_TXOBJ_IN_USE		0x80  	// Set to initiate TCP TX	

// FAST mode frame types
#define VSCP_FAST_FRAMETYPE_VSCP		0x00
#define VSCP_FAST_FRAMETYPE_ERROR		0x01
#define VSCP_FAST_FRAMETYPE_COMMAND		0x02

// FAST mode command codes
#define VSCP_FAST_COMMAND_NOOP			0x00
#define VSCP_FAST_COMMAND_READ			0x01
#define VSCP_FAST_COMMAND_CLOSE			0x02

// Define FAST error codes
#define VSCP_FAST_ERROR_OK			0x00

extern uint8_t vscp_alarmstatus;		
extern uint8_t vscp_node_state;	
extern uint8_t vscp_node_substate;

// Prototypes
void vscp2_init( void );
void vscp2_error( void );
void vscp2_rcv_new_node_online( void );
#ifdef VSCP_DISCOVER_SERVER
void sendHighEndServerProbe(void );
#endif

// Prototypes 
void vscp_readRegister( void );
void vscp_writeRegister( void );
uint8_t vscp_readStdReg( uint32_t reg );
uint8_t vscp_writeStdReg( uint32_t reg, uint8_t data );
 
 
//
// ============ START OF CALLBACKS ============
// The following methods must be defined
// somewhere and should return firmware version
// information
//
 
uint8_t vscp_getFirmwareMajorVersion( void );
uint8_t vscp_getFirmwareMinorVersion( void );
uint8_t vscp_getFirmwareSubMinorVersion( void );

int8_t vscp_sendEvent( void );
int8_t vscp_getEvent( void );

/// Init the VSCP functionality
void vscp_init( void );

// Fill in GUID information for our node  into event
void fillGUID( void );


// ***********************************************************************
// * * * The following methods should be implemented by the application
// ***********************************************************************

// UDP event functionality - Don't use directly
#ifdef VSCP_USE_UDP
int8_t vscp_udpinit( void );
int8_t vscp_sendUDPEvent( vscpEvent *pEvent );
int8_t vscp_getUDPEvent( vscpEvent *pEvent );
uint8_t vscp_getIPsddress( uint8_t pos );
#endif

// TCP event functionality - Don't use directly
#ifdef VSCP_USE_TCP
int8_t vscp_sendTCPEvent( vscpEvent *pEvent );
int8_t vscp_getTCPEvent( vscpEvent *pEvent );
uint8_t vscp_getIPsddress( uint8_t pos );
#endif

#ifdef VSCP_USE_RAW_ETHERNET
int8_t vscp_sendRawPacket( void );
int8_t vscp_getRawPacket( void );
#endif

uint8_t vscp_getBootLoaderAlgorithm( void );

uint8_t vscp_readAppReg( uint32_t reg );
uint8_t vscp_writeAppReg( uint32_t reg, uint8_t data );

uint8_t vscp_getControlByte( void );
void vscp_setControlByte( uint8_t ctrl );

uint8_t vscp_getUserID( uint8_t idx );
void vscp_setUserID( uint8_t idx, uint8_t data );

uint8_t vscp_getManufacturerId( uint8_t idx );
void vscp_setManufacturerId( uint8_t idx, uint8_t data );

uint8_t vscp_getBufferSize( void );

uint8_t vscp_getBootLoaderAlgorithm( void );

uint8_t vscp_getGUID( uint8_t idx );

uint8_t vscp_getMDF_URL( uint8_t idx );

uint8_t vscp_readAppReg( uint32_t idx );
uint8_t vscp_writeAppReg( uint32_t idx, uint8_t data );

void vscp_feedDM( void );

// ============ END OF CALLBACKS ============

#endif


