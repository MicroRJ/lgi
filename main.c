#define _RX_STANDALONE
#include "rx.c"

int main(int c, char **v)
{
  int counter=0;
  {
    rxinit(L"Font Visualizer");
    rxload_font("fonts\\UnifrakturCook\\UnifrakturCook-Bold.ttf");

    for(;;counter++)
    { rxdraw_text(rx.center_x,rx.center_y,64.f,ccformat("Hello, Sailor! %i",counter));
      rxtick();
    }
  }
}