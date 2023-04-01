#define _RX_STANDALONE
#include "rx.c"

int main(int c, char **v)
{
  int counter=0;
  {
    rxinit(L"Font Visualizer");

    for(;;counter++)
    {

      rxdraw_text(rx.center_x,rx.center_y,64,ccformat("Hello, Sailor! %i",counter));

      rxtick();
    }
  }
}