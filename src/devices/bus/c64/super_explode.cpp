// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Super Explode! cartridge emulation

**********************************************************************/

#include "emu.h"
#include "super_explode.h"



//**************************************************************************
//  MACROS/CONSTANTS
//**************************************************************************

// t = R*C = 3.3K * 0.47uF * 40% Vtr = 792.2905424610516 usec
// (3.3K pull-up on the EXROM line inside the C64, PLA Vih min = 2.0V = 40% of 5.0V)
#define TIMER_PERIOD    792



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(C64_SUPER_EXPLODE, c64_super_explode_cartridge_device, "c64_super_explode", "C64 Super Explode! cartridge")



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  c64_super_explode_cartridge_device - constructor
//-------------------------------------------------

c64_super_explode_cartridge_device::c64_super_explode_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, C64_SUPER_EXPLODE, tag, owner, clock),
	device_c64_expansion_card_interface(mconfig, *this), m_bank(0), m_exrom_timer(nullptr)
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void c64_super_explode_cartridge_device::device_start()
{
	// allocate timer
	m_exrom_timer = timer_alloc();

	// state saving
	save_item(NAME(m_bank));
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void c64_super_explode_cartridge_device::device_reset()
{
	m_bank = 0;

	m_exrom = 0;
	m_exrom_timer->adjust(attotime::from_usec(TIMER_PERIOD), 0);
}


//-------------------------------------------------
//  device_timer - handler timer events
//-------------------------------------------------

void c64_super_explode_cartridge_device::device_timer(emu_timer &timer, device_timer_id id, int param)
{
	m_exrom = 1;
}


//-------------------------------------------------
//  c64_cd_r - cartridge data read
//-------------------------------------------------

uint8_t c64_super_explode_cartridge_device::c64_cd_r(offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2)
{
	if (!roml)
	{
		m_exrom = 0;
		m_exrom_timer->adjust(attotime::from_usec(TIMER_PERIOD), 0);

		data = m_roml[(m_bank << 13) | (offset & 0x1fff)];
	}
	else if (!io1)
	{
		m_exrom = 0;
		m_exrom_timer->adjust(attotime::from_usec(TIMER_PERIOD), 0);
	}
	else if (!io2)
	{
		data = m_roml[(m_bank << 13) | (offset & 0x1fff)];
	}

	return data;
}


//-------------------------------------------------
//  c64_cd_w - cartridge data write
//-------------------------------------------------

void c64_super_explode_cartridge_device::c64_cd_w(offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2)
{
	if (!io1)
	{
		m_exrom = 0;
		m_exrom_timer->adjust(attotime::from_usec(TIMER_PERIOD), 0);
	}
	else if (!io2)
	{
		m_bank = BIT(data, 7);
	}
}
