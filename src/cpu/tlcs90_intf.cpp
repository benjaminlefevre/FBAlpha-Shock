// 2023.05.13 THK: prefixed read, write, readio, writeio with tlcs_ to prevent MSBuild conflicts

#include "burnint.h"
#include "tlcs90_intf.h"

static UINT8 (*tlcs_read)(UINT32) = NULL;
static void (*tlcs_write)(UINT32, UINT8) = NULL;
static UINT8 (*tlcs_readio)(UINT16) = NULL;
static void (*tlcs_writeio)(UINT16, UINT8) = NULL;

static UINT8 *mem[2][0x1000]; // only read/fetch & write

extern void t90_internal_registers_w(UINT16 offset, UINT8 data);
extern UINT8 t90_internal_registers_r(UINT16 offset);
INT32 tlcs90_init(INT32 clock);

static void core_set_irq(INT32 /*cpu*/, INT32 line, INT32 state)
{
	tlcs90SetIRQLine(line, state);
}

cpu_core_config tlcs90Config =
{
	tlcs90Open,
	tlcs90Close,
	tlcs90CheatRead,
	tlcs90WriteROM, 
	tlcs90GetActive,
	tlcs90TotalCycles,
	tlcs90NewFrame,
	tlcs90Idle,
	core_set_irq,
	tlcs90Run,
	tlcs90RunEnd,
	tlcs90Reset,
	0x100000,
	0
};

UINT8 tlcs90_program_read_byte(UINT32 address)
{
	address &= 0xfffff;

	// internal registers!!!!!!!!!!!!!!!!!
	if (address >= 0xffc0 && address <= 0xffef) {
		return t90_internal_registers_r(address & 0x3f);
	}

	if (mem[0][(address / 0x100)] != NULL) {
		return mem[0][(address / 0x100)][address & 0xff];
	}

	if (tlcs_read) {
		return tlcs_read(address);
	}

	return 0;
}

void tlcs90_program_write_byte(UINT32 address, UINT8 data)
{
	address &= 0xfffff;

	// internal registers!!!!!!!!!!!!!!!!!
	if (address >= 0xffc0 && address <= 0xffef) {
		t90_internal_registers_w(address & 0x3f, data);
		return;
	}

	if (mem[1][(address / 0x100)] != NULL) {
		mem[1][(address / 0x100)][address & 0xff] = data;
		return;
	}

	if (tlcs_write) {
        tlcs_write(address, data);
		return;
	}
}

UINT8 tlcs90_io_read_byte(UINT16 port)
{
	port &= 0xffff;

//	bprintf (0, _T("Read Port: %4.4x\n"), port);

	if (tlcs_readio) {
		return tlcs_readio(port);
	}

	return 0;
}

void tlcs90_io_write_byte(UINT16 port, UINT8 data)
{
	port &= 0xffff;

//	bprintf (0, _T("Write Port: %4.4x %2.2x\n"), port, data);

	if (tlcs_writeio) {
		return tlcs_writeio(port, data);
	}
}


void tlcs90SetReadHandler(UINT8 (*pread)(UINT32))
{
    tlcs_read = pread;
}

void tlcs90SetWriteHandler(void (*pwrite)(UINT32, UINT8))
{
	tlcs_write = pwrite;
}

void tlcs90SetReadPortHandler(UINT8 (*pread)(UINT16))
{
    tlcs_readio = pread;
}

void tlcs90SetWritePortHandler(void (*pwrite)(UINT16, UINT8))
{
    tlcs_writeio = pwrite;
}

void tlcs90MapMemory(UINT8 *ptr, UINT32 start, UINT32 end, INT32 flags)
{
	start &= 0xfffff;
	end &= 0xfffff;

	for (UINT32 i = start / 0x100; i < (end / 0x100) + 1; i++)
	{
		if (flags & (1 << 0)) mem[0][i] = ptr + ((i * 0x100) - start);
		if (flags & (1 << 1)) mem[1][i] = ptr + ((i * 0x100) - start);
	}
}

UINT8 tlcs90CheatRead(UINT32 address)
{
	return tlcs90_program_read_byte(address);
}

void tlcs90WriteROM(UINT32 address, UINT8 data)
{
	if (mem[0][(address / 0x100)] != NULL) {
		mem[0][(address / 0x100)][address & 0xff] = data;
	}

	if (mem[1][(address / 0x100)] != NULL) {
		mem[1][(address / 0x100)][address & 0xff] = data;
	}
}

INT32 tlcs90GetActive()
{
	return 0; // only one for now
}

void tlcs90Open(INT32)
{
	// only one cpu for now
}

void tlcs90Close()
{

}

INT32 tlcs90Init(INT32, INT32 clock)
{
	memset (mem, 0, 2 * 0x1000 * sizeof(UINT8 *));

    tlcs_read = NULL;
    tlcs_write = NULL;
    tlcs_readio = NULL;
    tlcs_writeio = NULL;

	CpuCheatRegister(0, &tlcs90Config);

	return tlcs90_init(clock);
}

INT32 tlcs90Exit()
{
	memset (mem, 0, 2 * 0x1000 * sizeof(UINT8 *));
    tlcs_read = NULL;
    tlcs_write = NULL;
    tlcs_readio = NULL;
    tlcs_writeio = NULL;

	return 0;
}


