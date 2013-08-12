#include <iframe.hpp>
#include <icmdhdr.hpp>
#include <istring.hpp>
#include <istattxt.hpp>
#include <imcelcv.hpp>
#include <ientryfd.hpp>

#include "tempclass.hpp"

class MainWindow : public IFrameWindow, public ICommandHandler
{
   TempData       Data;
   IStaticText **Labels;
   IStaticText **Values;
   IMultiCellCanvas canvas;
protected:
   Boolean command(ICommandEvent & );
public:
   MainWindow( unsigned long windowId );
   ~MainWindow();
   Boolean Refresh();
   void Running();
};
