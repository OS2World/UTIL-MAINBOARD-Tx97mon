#define INCL_DOSPROCESS
#include "os2.h"
#include <iapp.hpp>
#include <istring.hpp>
#include <ithread.hpp>
#include "window.hpp"

#define ID_CANVAS 30
#define IDM_REFRESH 1000

void main( void )
{
   MainWindow appWindow( 10 );
   IApplication::current().run();
}

MainWindow::MainWindow( unsigned long windowId )
:IFrameWindow( windowId, IFrameWindow::defaultStyle() ),
canvas( ID_CANVAS, this, this ),
Data()
{
   setClient(&canvas);
   Labels = new IStaticText*[5];
   Values = new IStaticText*[5];
   Data.read();
   for( int i=0; i<11; i++ ){
      Labels[i] = new IStaticText( ID_CANVAS+(2*i)+1, &canvas, &canvas );
      Values[i] = new IStaticText( ID_CANVAS+(2*i)+2, &canvas, &canvas );
      switch(i){
         case 0:
            Labels[0]->setText( "Core voltage" );
            break;
         case 1:
            Labels[1]->setText( "IO voltage" );
            break;
         case 2:
            Labels[2]->setText( "+5 voltage" );
            break;
         case 3:
            Labels[3]->setText( "+12 Voltage" );
            break;
         case 4:
            Labels[4]->setText( "-12 Voltage" );
            break;
         case 5:
            Labels[5]->setText( "-5 Voltage" );
            break;
         case 6:
            Labels[6]->setText( "RPM 1" );
            break;
         case 7:
            Labels[7]->setText( "RPM 2" );
            break;
         case 8:
            Labels[8]->setText( "RPM 3" );
            break;
         case 9:
            Labels[9]->setText( "MB Temp" );
            break;
         case 10:
            Labels[10]->setText( "CPU Temp" );
            break;
         }
      canvas.addToCell( Labels[i], 2, 2*i+2 );
      canvas.addToCell( Values[i], 4, 2*i+2 );
      }
   ICommandHandler::handleEventsFor(this);
   setFocus();
   show();
   Refresh();
   IThread thread( new IThreadMemberFn<MainWindow>
                      (*this, MainWindow::Running) );

};

MainWindow::~MainWindow()
{
   ICommandHandler::stopHandlingEventsFor(this);
}

Boolean MainWindow :: command(ICommandEvent &cmdEvent)
{
   switch (cmdEvent.commandId())
   {
      case IDM_REFRESH:
         Refresh();
         return true;
   } /* end switch */
   return false;
}

void MainWindow::Running()
{
   while(1){
      DosSleep( 1500 );
      postEvent( IWindow::command, IDM_REFRESH );
   }
}



Boolean MainWindow::Refresh()
{
   static int cnt=0;
   Data.read();
   Values[0]->setText( IString(Data.Vcore()).subString(1,4) );
   Values[1]->setText( IString(Data.Vio()).subString(1,4) );
   Values[2]->setText( IString(Data.V5()).subString(1,4) );
   Values[3]->setText( IString(Data.V12()).subString(1,4) );
   Values[4]->setText( IString(Data.Vneg12()).subString(1,5) );
   Values[5]->setText( IString(Data.Vneg5()).subString(1,4) );
   Values[6]->setText( IString(cnt) );
   Values[7]->setText( IString(Data.Rpm2()) );
   Values[8]->setText( IString(Data.Rpm3()) );
   Values[9]->setText( IString(Data.Mbt()) );
   Values[10]->setText( IString(Data.Cput()).subString(1,4) );
   return true;
}
