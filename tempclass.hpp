#define INCL_DOSFILEMGR
#define INCL_DOSDEVIOCTL
#define INCL_ERRORS
#include <os2.h>
#include "cio.h"

class TempData{
   double vCore;
   double vIO;
   double v5;
   double v12;
   double vneg12;
   double vneg5;
   int RPM1;
   int RPM2;
   int RPM3;
   int mbTemp;
   double cpuTemp;

   unsigned short LM78;
   unsigned short LM75;
public:
   TempData();
   ~TempData() {}
   int read();
   double Vcore() { return vCore;}
   double Vio() { return vIO; }
   double V5() { return v5; }
   double V12() { return v12; }
   double Vneg12() { return vneg12; }
   double Vneg5() { return vneg5; }
   int Rpm1() { return RPM1; }
   int Rpm2() { return RPM2; }
   int Rpm3() { return RPM3; }
   int Mbt() { return mbTemp; }
   double Cput() { return cpuTemp; }
};


TempData::TempData()
{
   unsigned int temp;
   LM78 = 0x290;
   io_init();
   LM75 = c_inl((unsigned short)0x0CF8);
   temp &= 0x7F000003;  //Make sure we keep all reserved bits as they were
   temp |= 0x80000800;  //Enable CONE on the MTXC
   temp |= (1<<11);     //Select Device number on the MTXC (PIIX4 = PCI Device 1)
   temp |= (3<<8);      //Select Function number on the MTXC
   temp |= (0x90);      //Select Address offset  on the MTXC
   c_outl( (unsigned short) 0x0CF8, temp );
   LM75 = (unsigned short)c_inw(0x0CFC)-1;

   c_outb(LM78+5, 0x47);
   temp = c_inb(LM78+6);
//   printf("Fan Div:\t%02X\n", (unsigned char)temp );

}

int TempData::read()
{
   c_outb(LM78+5, 0x61);
   vCore = (unsigned int)c_inb(LM78+6) * 0.016f;
   vIO = (unsigned int)c_inb(LM78+6) * 0.016f;
   v5 = (unsigned int)c_inb(LM78+6) * 0.016f * 1.69;
   v12 = (unsigned int)c_inb(LM78+6) * 0.016f * 3.81;
   vneg12 = (unsigned int)c_inb(LM78+6) * 0.016f * -3.47;
   vneg5 = (unsigned int)c_inb(LM78+6) * 0.016f * -1.49;
   mbTemp = c_inb(LM78+6);
   RPM1 = (unsigned char) c_inb(LM78+6);
   RPM2 = (unsigned char) c_inb(LM78+6);
   RPM3 = (unsigned char) c_inb(LM78+6);
   RPM1 = (RPM1==0xff?0:1.35e6/RPM1/4);
   RPM2 = (RPM2==0xff?0:(1.35e6/RPM2/4));
   RPM3 = (RPM3==0xff?0:1.35e6/RPM3/2);

   unsigned char tmp;
   tmp = c_inb(LM75);
   if( (tmp & 0x1e) != 0 ){
//      printf("Clearing inter.\n");
      c_outb( LM75, tmp );
//      printf("LM75:\t%08x\n", (unsigned int)c_inb(LM75) );
   }
   c_outb( LM75+3, 0x00 );
//   printf("SmbCMD:\t%02X\n", c_inb(LM75+3) );
   c_outb( LM75+4, 0x93 );
//   printf("SmbAdd:\t%02X\n", c_inb(LM75+4) );

   tmp = c_inb( LM75+2 );
   tmp = ( tmp & 0xe0 ) | 0x0c;
   c_outb( LM75+2, tmp );
//   printf("SmbCNT:\t%02X\n", c_inb(LM75+2) );

   c_outb( LM75+2, (tmp | 0x40) );
   while( c_inb(LM75) & 0x1 ){
//      printf("waiting\n");
   }
//   printf( "SmbTSTS:\t%02X\n", c_inb(LM75) );

   cpuTemp = c_inb(LM75+5);
   cpuTemp += 0.5f *(c_inb(LM75+6)>>7);
//   printf( "cpu temp %lf\n", cpuTemp);
   return 0;
}
