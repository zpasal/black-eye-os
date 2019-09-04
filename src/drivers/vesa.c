#include <vesa.h>
#include <x86.h>

void bga_write_register(unsigned short IndexValue, unsigned short DataValue){
    outpw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    outpw(VBE_DISPI_IOPORT_DATA, DataValue);
}

unsigned short bga_read_register(unsigned short IndexValue)
{
    outpw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    return inpw(VBE_DISPI_IOPORT_DATA);
}

int bga_is_available(void)
{
    return 1; //(bga_read_register(VBE_DISPI_INDEX_ID) == VBE_DISPI_ID4);
}

void bga_set_video_mode(unsigned int Width, unsigned int Height, unsigned int BitDepth, int UseLinearFrameBuffer, int ClearVideoMemory)
{
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, Width);
    bga_write_register(VBE_DISPI_INDEX_YRES, Height);
    bga_write_register(VBE_DISPI_INDEX_BPP, BitDepth);
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED |
        (UseLinearFrameBuffer ? VBE_DISPI_LFB_ENABLED : 0) |
        (ClearVideoMemory ? 0 : VBE_DISPI_NOCLEARMEM));
}

void bga_set_bank(unsigned short BankNumber)
{
    bga_write_register(VBE_DISPI_INDEX_BANK, BankNumber);
}
