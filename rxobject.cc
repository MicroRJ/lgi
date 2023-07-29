
rxarticle_t *
rxarticle_create(void *unknown)
{
  rxarticle_t *tan=cctblputP(rx.instance_table,unknown);
  ccassert(ccerrnon());

  tan->unknown=(rxunknown_t)(unknown);

  return tan;
}

rxarticle_t *
rxarticle_attach(rxarticle_t *tan, rxlinkage_k lin, void *unknown)
{
  tan->linkage[lin]=(rxunknown_t)(unknown);

  return tan;
}

rxterminal_t *
rxlinker_register_terminal(
  const char *master)
{
  rxterminal_t *contents=cctblsetS(rx.contents_table,master);
  ccassert(ccerrnon());

  return contents;
}

rxterminal_t *
rxlinker_onlyquery_terminal(
  const char *master)
{
  rxterminal_t *contents=cctblgetS(rx.contents_table,master);

  if(ccerrnit())
  {
    contents=ccnull;
  }

  return contents;
}

int
rxlinker_labelshas_terminal(
  const char *master, int labels)
{
  rxterminal_t *contents=rxlinker_onlyquery_terminal(master);

  if(contents != 0)
  {
    return contents->labels & labels;
  }

  return 0;
}

int
rxlinker_labelsadd_terminal(
  const char *master, int labels)
{
  rxterminal_t *contents=rxlinker_onlyquery_terminal(master);

  if(contents != 0)
  {
    contents->labels |= labels;
  }

  return contents != 0;
}

int
rxlinker_labelsrem_terminal(
  const char *master, int labels)
{
  rxterminal_t *contents=rxlinker_onlyquery_terminal(master);

  if(contents != 0)
  {
    contents->labels |= labels;
  }

  return contents != 0;
}

rxterminal_t *
rxlinker_resolve_terminal(
  const char *master)
{
  ccassert(master    != 0);
  ccassert(master[0] != 0);

  rxterminal_t *contents=rxlinker_register_terminal(master);

  if((~contents->labels&rxlabel_kLOADED)||(contents->labels&rxlabel_kINVALIDATED))
  {
    ccu32_t length;
    void *  memory;
    void *  handle;

    length=0;
    handle=ccopenfile(master,"r");
    memory=ccpullfile(handle,0,&length);


    ccclosefile(handle);

    if(length != 0 && memory != 0)
    {
      if((contents->length!=length)||memcmp(memory,contents->memory,length))
      {
        ccdebuglog("'%s': file contents re/loaded",master);

        contents->loaded=rx.total_ticks;
        contents->labels&=~rxlabel_kERRONEOUS;
        contents->labels&=~rxlabel_kINVALIDATED;
        contents->labels|=rxlabel_kLOADED;
      }

      if(contents->memory)
      {
        ccfree(contents->memory);
      }

      contents->memory=memory;
      contents->length=length;
      contents->master=master;

    } else
      cctracewar("'%s': file contents could not be read",master);
  }

  return contents;
}
