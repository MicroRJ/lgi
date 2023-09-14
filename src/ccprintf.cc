/*
**
**                      -+- cc -+-
**
**                 Mundane Utilities API.
**
**  *--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
**  Copyright(C) Dayan Rodriguez, 2022, All Rights Reserved
**  *--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
**
**        NOT PRODUCTION READY/NOT PRODUCTION QUALITY
**
**
**               HONI SOIT QUI MAL Y PENSE
**
**
**                 github.com/MicroRJ/cc
**
**
*/

/* This is absolute crap - XXX - the one called rj */



// Todo: this is not the best way to do things!
#define CCFG_BLACK        "<!0"
#define CCFG_BLUE         "<!1"
#define CCFG_GREEN        "<!2"
#define CCFG_CYAN         "<!3"
#define CCFG_RED          "<!4"
#define CCFG_MAGENTA      "<!5"
#define CCFG_BROWN        "<!6"
#define CCFG_LIGHTGRAY    "<!7"
#define CCFG_GRAY         "<!8"
#define CCFG_LIGHTBLUE    "<!9"
#define CCFG_LIGHTGREEN   "<!0"
#define CCFG_LIGHTCYAN    "<!1"
#define CCFG_LIGHTRED     "<!2"
#define CCFG_LIGHTMAGENTA "<!3"
#define CCFG_YELLOW       "<!4"
#define CCFG_WHITE        "<!F"
#define CCEND             "!>"

// Todo:
#define ccmove_color( x )  cc.color_value=x
#define ccpush_color(   )  cc.color_stack[cc.color_index++]=cc.color_value
#define ccload_color(   )  cc.color_value=cc.color_stack[--cc.color_index]

ccfunc void
ccprintf(const char *r, ...)
{
#define cccolornext( l )  r+=l,c=*r
#define cccolorpval(   )  (void  *) va_arg(vli,void  *)
#define cccolorsval(   )  (char  *) va_arg(vli,char  *)
#define cccolorlval(   )  (cci64_t) va_arg(vli,cci64_t)
#define cccolorival(   )  (cci32_t) va_arg(vli,cci32_t)
#define cccolorfval(   )  (ccf64_t) va_arg(vli,ccf64_t)
#define cccolorwval(   )  (cci16_t) va_arg(vli,cci32_t)
#define cccolorcval(   )  (char   ) va_arg(vli,char   )

  va_list vli;
  va_start(vli,r);

  char c;
  for(;c=*r;c)
  {
    for(; (c!='\0')&&(c!='%') &&
         !(c=='<'&&r[1]=='!') &&
         !(c=='!'&&r[1]=='>'); cccolornext(1)) printf("%c",c);

    if(c=='<')
    { cccolornext(2);
      if(CCWITHIN(c,'0','9'))
        ccpush_color(),ccmove_color(0x00+c-'0'),cccolornext(1);
      else
      if(CCWITHIN(c,'A','F'))
        ccpush_color(),ccmove_color(0x0A+c-'A'),cccolornext(1);
      else
      if(CCWITHIN(c,'a','f'))
        ccpush_color(),ccmove_color(0x0A+c-'a'),cccolornext(1);
      else
      if(c=='%'&&r[1]=='i')
        ccpush_color(),ccmove_color(cccolorwval()),cccolornext(2);
      else
        ccassert(!"error");
    } else
    if(c=='!')
    { ccload_color(),cccolornext(2);
    } else
    if(c=='%')
    { cccolornext(1);
      if(c=='%')
        printf("%%"),cccolornext(1);
      else
      if(c=='i')
        printf("%i",cccolorival()),cccolornext(1);
      else
      if(c=='f')
        printf("%f",cccolorfval()),cccolornext(1);
      else
      if(c=='s')
        printf("%s",cccolorsval()),cccolornext(1);
      else
      if(c=='c')
        printf("%c",cccolorcval()),cccolornext(1);
      else
      if(c=='p')
        printf("%p",cccolorpval()),cccolornext(1);
      else
      { if(c=='l'&&r[1]=='l'&&r[2]=='i')
          printf("%lli",cccolorlval()),cccolornext(3);
        else
        if(c=='l'&&r[1]=='l'&&r[2]=='u')
          printf("%llu",cccolorlval()),cccolornext(3);
        else
          ccassert(!"error");
      }
    } else
    if(c=='\0')
      break;

#ifdef _WIN32
    SetConsoleTextAttribute((HANDLE)cc.console,cc.color_value);
#endif
  }
  va_end(vli);
}