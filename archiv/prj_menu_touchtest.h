#ifndef _PRJ_MENU_TOUCHTEST_H_
  #define _PRJ_MENU_TOUCHTEST_H_

  // includes
    #include <md_defines.h>
    #include <md_util.h>
    #include <md_touch.h>

  // const menu its
    const char TITLE_TXT_START[] = "M&M Smart Home";
    const char STAT_TXT_START[]  = "mir gehts gut";
    const char STAT_EMPTY[]      = "";
  // standard menu items (= cell of linked list)
    MENUITEM_t   itTitle;    MENUITEM_t*  pitTitle    = &itTitle;
    MENUITEM_t   itStatus;   MENUITEM_t*  pitStatus   = &itStatus;
    MENUITEM_t   itBMPleft;  MENUITEM_t*  pitBMPleft  = &itBMPleft;
    MENUITEM_t   itBMPright; MENUITEM_t*  pitBMPright = &itBMPright;
    MENUITEM_t   itBMPup;    MENUITEM_t*  pitBMPup    = &itBMPup;
    MENUITEM_t   itBMPdown;  MENUITEM_t*  pitBMPdown  = &itBMPdown;
    MENUITEM_t   itSmily;    MENUITEM_t*  pitSmily    = &itSmily;
    MENUITEM_t   itNotSmil;  MENUITEM_t*  pitNotSmil  = &itNotSmil;
    MENUITEM_t   itFrowny;   MENUITEM_t*  pitFrowny   = &itFrowny;

  // page 1: environment measure items (= cell of linked list)
    // labels
      MENUITEM_t   itT680L;    MENUITEM_t*  pitT680L    = &itT680L; const char STAT_LAB_T680L[]     = "Temp C";
      MENUITEM_t   itH680L;    MENUITEM_t*  pitH680L    = &itH680L; const char STAT_LAB_H680L[]     = "Feuchte %";
      MENUITEM_t   itP680L;    MENUITEM_t*  pitP680L    = &itP680L; const char STAT_LAB_P680L[]     = "P-Luft mb";
      MENUITEM_t   itG680L;    MENUITEM_t*  pitG680L    = &itG680L; const char STAT_LAB_G680L[]     = "R-Gas Ohm";
    // values
      MENUITEM_t   itT680V;    MENUITEM_t*  pitT680V    = &itT680V; static char pVal_T680V[MENU_VALMAX]  = "---";
      MENUITEM_t   itH680V;    MENUITEM_t*  pitH680V    = &itH680V; static char pVal_H680V[MENU_VALMAX]  = "---";
      MENUITEM_t   itP680V;    MENUITEM_t*  pitP680V    = &itP680V; static char pVal_P680V[MENU_VALMAX]  = "---";
      MENUITEM_t   itG680V;    MENUITEM_t*  pitG680V    = &itG680V; static char pVal_G680V[MENU_VALMAX]  = "---";
  // page 2: bluetti control & state
    // labels
      MENUITEM_t   itB1ACoutL; MENUITEM_t*  pitB1ACoutL = &itB1ACoutL; const char STAT_LAB_ACOUTL[] = "AC out W";
      MENUITEM_t   itB1DCoutL; MENUITEM_t*  pitB1DCoutL = &itB1DCoutL; const char STAT_LAB_DCOUTL[] = "DC out W";
      MENUITEM_t   itB1SNumL;  MENUITEM_t*  pitB1SNumL  = &itB1SNumL;  const char STAT_LAB_SNUML[]  = "Ser No";
      MENUITEM_t   itB1TypL;   MENUITEM_t*  pitB1TypL   = &itB1TypL;   const char STAT_LAB_TYPL[]   = "Bat Typ";
      MENUITEM_t   itB1ARML;   MENUITEM_t*  pitB1ARML   = &itB1ARML;   const char STAT_LAB_ARML[]   = "RAM Ver";
      MENUITEM_t   itB1DSPL;   MENUITEM_t*  pitB1DSPL   = &itB1DSPL;   const char STAT_LAB_DSPL[]   = "DSP Ver";
      MENUITEM_t   itB1DCinL;  MENUITEM_t*  pitB1DCinL  = &itB1DCinL;  const char STAT_LAB_DCinL[]  = "DC Pin W";
      MENUITEM_t   itB1ACinL;  MENUITEM_t*  pitB1ACinL  = &itB1ACinL;  const char STAT_LAB_ACinL[]  = "AC Pin W";
      MENUITEM_t   itB1PBTL;   MENUITEM_t*  pitB1PBTL   = &itB1PBTL;   const char STAT_LAB_P1BTL[]  = "B1 P   %";
      MENUITEM_t   itB1PB0L;   MENUITEM_t*  pitB1PB0L   = &itB1PB0L;   const char STAT_LAB_P1B0L[]  = "B1 P0  %";
      MENUITEM_t   itB1PB1L;   MENUITEM_t*  pitB1PB1L   = &itB1PB1L;   const char STAT_LAB_P1B1L[]  = "AC P1  %";
      MENUITEM_t   itB1PB2L;   MENUITEM_t*  pitB1PB2L   = &itB1PB2L;   const char STAT_LAB_P1B2L[]  = "AC P2  %";
      MENUITEM_t   itB1PB3L;   MENUITEM_t*  pitB1PB3L   = &itB1PB3L;   const char STAT_LAB_P1B3L[]  = "AC P3  %";
    // values
      MENUITEM_t   itB1ACoutV; MENUITEM_t*  pitB1ACoutV = &itB1ACoutV; static char pVal_ACOUTV[MENU_VALMAX]  = "---";
      MENUITEM_t   itB1DCoutV; MENUITEM_t*  pitB1DCoutV = &itB1DCoutV; static char pVal_DCOUTV[MENU_VALMAX]  = "---";
      MENUITEM_t   itB1SNumV;  MENUITEM_t*  pitB1SNumV  = &itB1SNumV;  static char pVal_SNUMV[MENU_VALMAX*2]= "---";
      MENUITEM_t   itB1TypV;   MENUITEM_t*  pitB1TypV   = &itB1TypV;   static char pVal_TYPV [MENU_VALMAX]  = "---";
      MENUITEM_t   itB1ARMV;   MENUITEM_t*  pitB1ARMV   = &itB1ARMV;   static char pVal_ARMV [MENU_VALMAX]  = "---";
      MENUITEM_t   itB1DSPV;   MENUITEM_t*  pitB1DSPV   = &itB1DSPV;   static char pVal_DSPV [MENU_VALMAX]  = "---";
      MENUITEM_t   itB1DCinV;  MENUITEM_t*  pitB1DCinV  = &itB1DCinV;  static char pVal_DCinV[MENU_VALMAX]  = "---";
      MENUITEM_t   itB1ACinV;  MENUITEM_t*  pitB1ACinV  = &itB1ACinV;  static char pVal_ACinV[MENU_VALMAX]  = "---";
      MENUITEM_t   itB1PBTV;   MENUITEM_t*  pitB1PBTV   = &itB1PBTV;   static char pVal_P1BTV[MENU_VALMAX]  = "---";
      MENUITEM_t   itB1PB0V;   MENUITEM_t*  pitB1PB0V   = &itB1PB0V;   static char pVal_P1B0V[MENU_VALMAX]  = "---";
      MENUITEM_t   itB1PB1V;   MENUITEM_t*  pitB1PB1V   = &itB1PB1V;   static char pVal_P1B1V[MENU_VALMAX]  = "---";
      MENUITEM_t   itB1PB2V;   MENUITEM_t*  pitB1PB2V   = &itB1PB2V;   static char pVal_P1B2V[MENU_VALMAX]  = "---";
      MENUITEM_t   itB1PB3V;    MENUITEM_t* pitB1PB3V   = &itB1PB3V;   static char pVal_P1B3V[MENU_VALMAX]  = "---";
  // page 3: power information
    //const char STAT_LAB_T680L[]  = "12345678901";    const char STAT_LAB_T680V[]  = "123456";
  // title bar
    void configTitle()
      {
          itTitle.menuid.scrrow    = 1;
          itTitle.menuid.scrcol    = 1;
          itTitle.menuid.menpage   = MENPAGE_ALL;
          itTitle.menuid.itw       = BOX_X_MAX - 2;
          itTitle.menuid.ittyp     = MENTYP_TEXT;
          itTitle.menuid.valtyp    = MENVAL_TEXT;
          itTitle.menuid.colmod    = COLMODE_BOXSEL;
          itTitle.pboxcols         = (COLORS_t*) pstatitBoxCols; // colors
          itTitle.boxtext.ptxtcol  = (COLORS_t*) pstatitTxtCols;
          itTitle.boxtext.txtsize  = 2;
          itTitle.boxtext.align    = TXT_CENTER;
          itTitle.boxtext.ptext    = (char*) TITLE_TXT_START;
          itTitle.boxtext.txtlen   = strlen(TITLE_TXT_START);
              SHEXVAL(" new pTitle  ", (uint32_t) &itTitle);
      }
  // status bar
    void configStatus()
      {
          itStatus.menuid.scrrow   = BOX_Y_MAX;
          itStatus.menuid.scrcol   = 1;
          itStatus.menuid.menpage  = MENPAGE_ALL;
          itStatus.menuid.itw      = BOX_X_MAX;
          itStatus.menuid.ittyp    = MENTYP_EDTEXT;
          itStatus.menuid.valtyp   = MENVAL_TEXT;
          itStatus.menuid.colmod   = COLMODE_BOXDEF;
          itStatus.pboxcols        = (COLORS_t*) pstatitBoxCols; // colors
          itStatus.boxtext.ptxtcol = (COLORS_t*) pstatitTxtCols;
          itStatus.boxtext.txtsize = 2;
          itStatus.boxtext.align   = TXT_CENTER;
          itStatus.menuid.menstat  = MENBIT_ISSEL;
          itStatus.boxtext.ptext   = (char*) STAT_TXT_START;
          itStatus.boxtext.txtlen  = strlen(STAT_TXT_START);
              SHEXVAL(" new pStatus ", (uint32_t) &itStatus);
      }
  // measurment menu its
    void configMenuItems(md_list* pits)
      {
        MENUITEM_t* _myp    = NULL;
        MENUITEM_t* _mypriv = NULL;
        // standard menu items
          // bitmaps
            _myp = pitBMPleft;  // bitmap <-
              _myp->menuid.scrcol   = 1;
              _myp->menuid.scrrow   = BOX_Y_MAX / 2;
              _myp->menuid.menpage  = MENPAGE_ALL;
              _myp->menuid.itw      = 1;
              _myp->menuid.bmpwh    = 16;
              _myp->menuid.ittyp    = MENTYP_CTRL;
              _myp->menuid.valtyp   = MENVAL_TEXT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pcurBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pcurTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_CENTER;
              _myp->boxtext.ptext   = (char*) STAT_EMPTY;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              _myp->pobj            = (void*) &mono_bmp16[MD_BM16_ARROW_L][0];
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pitBMPleft  cnt idx pNext(priv)", (uint32_t) _myp, pits->count(),
                                                                 _myp->index(),  0);
              _mypriv = _myp;
            _myp = pitBMPup;    // bitmap up
              _myp->menuid.scrcol   = BOX_X_MAX / 2;
              _myp->menuid.scrrow   = 2;
              _myp->menuid.menpage  = MENPAGE_ALL;
              _myp->menuid.itw      = 1;
              _myp->menuid.bmpwh    = 16;
              _myp->menuid.ittyp    = MENTYP_CTRL;
              _myp->menuid.valtyp   = MENVAL_TEXT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pcurBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pcurTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_CENTER;
              _myp->boxtext.ptext   = (char*) STAT_EMPTY;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              _myp->pobj            = (void*) &mono_bmp16[MD_BM16_ARROW_U][0];
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pitBMPup    cnt idx pNext(priv)", (uint32_t) _myp, pits->count(),
                                                                     _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
            _myp = pitBMPdown;  // bitmap down
              _myp->menuid.scrcol   = BOX_X_MAX / 2;
              _myp->menuid.scrrow   = BOX_Y_MAX - 1;
              _myp->menuid.menpage  = MENPAGE_ALL;
              _myp->menuid.itw      = 1;
              _myp->menuid.bmpwh    = 16;
              _myp->menuid.ittyp    = MENTYP_CTRL;
              _myp->menuid.valtyp   = MENVAL_TEXT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pcurBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pcurTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_CENTER;
              _myp->boxtext.ptext   = (char*) STAT_EMPTY;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              _myp->pobj            = (void*) &mono_bmp16[MD_BM16_ARROW_D][0];
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pitBMPdown  cnt idx pNext(priv)", (uint32_t) _myp, pits->count(),
                                                                       _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
            _myp = pitBMPright; // bitmap ->
              _myp->menuid.scrcol   = BOX_X_MAX;
              _myp->menuid.scrrow   = BOX_Y_MAX / 2;
              _myp->menuid.menpage  = MENPAGE_ALL;
              _myp->menuid.itw      = 1;
              _myp->menuid.bmpwh    = 16;
              _myp->menuid.ittyp    = MENTYP_CTRL;
              _myp->menuid.valtyp   = MENVAL_TEXT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pcurBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pcurTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_CENTER;
              _myp->boxtext.ptext   = (char*) STAT_EMPTY;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              _myp->pobj            = (void*) &mono_bmp16[MD_BM16_ARROW_R][0];
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pitBMPright cnt idx pNext(priv)", (uint32_t) _myp, pits->count(),
                                                                 _myp->index(), (uint32_t) _mypriv->pNext() );
              _mypriv = _myp;
          // smilies
            _myp = pitSmily;    // bitmap smily
              _myp->menuid.scrcol   = 11;
              _myp->menuid.scrrow   = 1; //BOX_Y_MAX - 2;
              _myp->menuid.menpage  = MENPAGE_ALL;
              _myp->menuid.itw      = 1;
              _myp->menuid.ith      = 2;
              _myp->menuid.bmpwh    = 32;
              _myp->menuid.ittyp    = MENTYP_BITMAP;
              _myp->menuid.valtyp   = MENVAL_TEXT;
              _myp->menuid.colmod   = COLMODE_BOXSEL;
              _myp->pboxcols        = (COLORS_t*) pbmpBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pbmpTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_CENTER;
              _myp->boxtext.ptext   = (char*) STAT_EMPTY;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              _myp->pobj            = (void*) pbmp32_smiley;
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pitSmily    cnt idx pNext(priv)", (uint32_t) _myp, pits->count(),
                                                                 _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
        // page 0 items
        // page 1 items = environment measures
          // value temperatur of BME680 - itT680L/itT680V
            _myp = pitT680L;    // label
              _myp->menuid.scrcol   = 2;
              _myp->menuid.scrrow   = 3;
              _myp->menuid.menpage  = 1;
              _myp->menuid.itw      = VAL_L_SIZE;
              _myp->menuid.ittyp    = MENTYP_TEXT;
              _myp->menuid.valtyp   = MENVAL_TEXT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pdefBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pdefTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_LEFT;
              _myp->boxtext.ptext   = (char*) STAT_LAB_T680L;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              _myp->pobj            = pitT680V;
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pT680V cnt idx pNext(priv)", (uint32_t) _myp, pits->count(), _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
            _myp = pitT680V;    // value
              _myp->menuid.scrcol   = VAL_L_SIZE + 2;
              _myp->menuid.scrrow   = 3;
              _myp->menuid.menpage  = 1;
              _myp->menuid.itw      = VAL_V_SIZE;
              _myp->menuid.ittyp    = MENTYP_OUTVAL;
              _myp->menuid.valtyp   = MENVAL_FLOAT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pdefBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pdefTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_RIGHT;
              _myp->boxtext.ptext   = pVal_T680V;
              _myp->boxtext.txtlen  = strlen(itStatus.boxtext.ptext);
              _myp->pobj            = pitT680L;
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pT680V cnt idx pNext(priv)", (uint32_t) _myp, pits->count(), _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
          // value humidity of BME680 - itH280L/itH680V
            _myp = pitH680L;    // label
              _myp->menuid.scrcol   = 2;
              _myp->menuid.scrrow   = 4;
              _myp->menuid.menpage  = 1;
              _myp->menuid.itw      = VAL_L_SIZE;
              _myp->menuid.ittyp    = MENTYP_TEXT;
              _myp->menuid.valtyp   = MENVAL_TEXT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pdefBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pdefTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_LEFT;
              _myp->boxtext.ptext   = (char*) STAT_LAB_H680L;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              _myp->pobj            = pitH680V;
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pT680V cnt idx pNext(priv)", (uint32_t) _myp, pits->count(), _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
            _myp = pitH680V;    // value
              _myp->menuid.scrcol   = VAL_L_SIZE + 2;
              _myp->menuid.scrrow   = 4;
              _myp->menuid.menpage  = 1;
              _myp->menuid.itw      = VAL_V_SIZE;
              _myp->menuid.ittyp    = MENTYP_OUTVAL;
              _myp->menuid.valtyp   = MENVAL_FLOAT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pdefBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pdefTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_RIGHT;
              _myp->boxtext.ptext   = pVal_H680V;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              _myp->pobj            = pitH680L;
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pT680V cnt idx pNext(priv)", (uint32_t) _myp, pits->count(), _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
          // value air pressure of BME680 - itP680L/itP680V
            _myp = pitP680L;    // label
              _myp->menuid.scrcol   = 2;
              _myp->menuid.scrrow   = 5;
              _myp->menuid.menpage  = 1;
              _myp->menuid.itw      = VAL_L_SIZE;
              _myp->menuid.ittyp    = MENTYP_TEXT;
              _myp->menuid.valtyp   = MENVAL_TEXT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pdefBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pdefTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_LEFT;
              _myp->boxtext.ptext   = (char*) STAT_LAB_P680L;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              _myp->pobj            = pitP680V;
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pT680V cnt idx pNext(priv)", (uint32_t) _myp, pits->count(), _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
            _myp = pitP680V;    // value
              _myp->menuid.scrcol   = VAL_L_SIZE + 2;
              _myp->menuid.scrrow   = 5;
              _myp->menuid.menpage  = 1;
              _myp->menuid.itw      = VAL_V_SIZE;
              _myp->menuid.ittyp    = MENTYP_OUTVAL;
              _myp->menuid.valtyp   = MENVAL_FLOAT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pdefBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pdefTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_RIGHT;
              _myp->boxtext.ptext   = pVal_P680V;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              _myp->pobj            = pitP680L;
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pT680V cnt idx pNext(priv)", (uint32_t) _myp, pits->count(), _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
          // value gas resistance of BME680 - itG680L/itG680V
            _myp = pitG680L;    // label
              _myp->menuid.scrcol   = 2;
              _myp->menuid.scrrow   = 6;
              _myp->menuid.menpage  = 1;
              _myp->menuid.itw      = VAL_L_SIZE;
              _myp->menuid.ittyp    = MENTYP_TEXT;
              _myp->menuid.valtyp   = MENVAL_TEXT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pdefBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pdefTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_LEFT;
              _myp->boxtext.ptext   = (char*) STAT_LAB_G680L;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              _myp->pobj            = pitG680V;
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pT680V cnt idx pNext(priv)", (uint32_t) _myp, pits->count(), _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
            _myp = pitG680V;    // value
              _myp->menuid.scrcol   = VAL_L_SIZE + 2;
              _myp->menuid.scrrow   = 6;
              _myp->menuid.menpage  = 1;
              _myp->menuid.itw      = VAL_V_SIZE;
              _myp->menuid.ittyp    = MENTYP_OUTVAL;
              _myp->menuid.valtyp   = MENVAL_FLOAT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pdefBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pdefTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_RIGHT;
              _myp->boxtext.ptext   = pVal_G680V;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              _myp->pobj            = pitG680L;
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pT680V cnt idx pNext(priv)", (uint32_t) _myp, pits->count(), _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
        // page 2 items = battery 1 = bluetti ac230
          // AC and DC switch outputs
            _myp = pitB1ACoutL;
              _myp->menuid.scrcol   = 2;
              _myp->menuid.scrrow   = 3;
              _myp->menuid.menpage  = 2;
              _myp->menuid.itw      = VAL_L_SIZE;
              _myp->menuid.ittyp    = MENTYP_TEXT;
              _myp->menuid.valtyp   = MENVAL_TEXT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pbmpBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pbmpTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_LEFT;
              _myp->boxtext.ptext   = (char*) STAT_LAB_ACOUTL;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pitB1ACoutL  cnt idx pNext(priv)", (uint32_t) _myp, pits->count(), _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
            _myp = pitB1ACoutV;   // bitmap frowny
              _myp->menuid.scrcol   = VAL_L_SIZE + 2;
              _myp->menuid.scrrow   = 3;
              _myp->menuid.menpage  = 2;
              _myp->menuid.itw      = VAL_L_SIZE;
              _myp->menuid.ittyp    = MENTYP_EDVAL;
              _myp->menuid.valtyp   = MENVAL_BOOL;
              _myp->menuid.colmod   = COLMODE_BOXSEL;
              _myp->pboxcols        = (COLORS_t*) pbmpBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pbmpTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_RIGHT;
              _myp->boxtext.ptext   = (char*) pVal_ACOUTV;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pitB1ACoutV   cnt idx pNext(priv)", (uint32_t) _myp, pits->count(), _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
            _myp = pitB1DCoutL;
              _myp->menuid.scrcol   = 2;
              _myp->menuid.scrrow   = 4;
              _myp->menuid.menpage  = 2;
              _myp->menuid.itw      = VAL_L_SIZE;
              _myp->menuid.ittyp    = MENTYP_TEXT;
              _myp->menuid.valtyp   = MENVAL_TEXT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pbmpBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pbmpTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_LEFT;
              _myp->boxtext.ptext   = (char*) STAT_LAB_DCOUTL;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pitB1ACoutL  cnt idx pNext(priv)", (uint32_t) _myp, pits->count(),
                                                                 _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
            _myp = pitB1DCoutV;   // bitmap frowny
              _myp->menuid.scrcol   = VAL_L_SIZE;
              _myp->menuid.scrrow   = 4;
              _myp->menuid.menpage  = 2;
              _myp->menuid.itw      = VAL_V_SIZE;
              _myp->menuid.ittyp    = MENTYP_EDVAL;
              _myp->menuid.valtyp   = MENVAL_BOOL;
              _myp->menuid.colmod   = COLMODE_BOXSEL;
              _myp->pboxcols        = (COLORS_t*) pbmpBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pbmpTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_RIGHT;
              _myp->boxtext.ptext   = (char*) pVal_DCOUTV;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pitB1DCoutV   cnt idx pNext(priv)", (uint32_t) _myp, pits->count(), _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
          // blueitti state outputs
            _myp = pitB1SNumL;
              _myp->menuid.scrcol   = 2;
              _myp->menuid.scrrow   = 5;
              _myp->menuid.menpage  = 2;
              _myp->menuid.itw      = VAL_L_SIZE - 2;
              _myp->menuid.ittyp    = MENTYP_TEXT;
              _myp->menuid.valtyp   = MENVAL_TEXT;
              _myp->menuid.colmod   = COLMODE_BOXDEF;
              _myp->pboxcols        = (COLORS_t*) pbmpBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pbmpTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_LEFT;
              _myp->boxtext.ptext   = (char*) STAT_LAB_SNUML;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pitB1SNumL  cnt idx pNext(priv)", (uint32_t) _myp, pits->count(), _myp->index(),  (uint32_t) _mypriv->pNext());
              _mypriv = _myp;
            _myp = pitB1SNumV;
              _myp->menuid.scrcol   = VAL_L_SIZE;
              _myp->menuid.scrrow   = 5;
              _myp->menuid.menpage  = 2;
              _myp->menuid.itw      = VAL_V_SIZE + 2;
              _myp->menuid.ittyp    = MENTYP_OUTVAL;
              _myp->menuid.valtyp   = MENVAL_BOOL;
              _myp->menuid.colmod   = COLMODE_BOXSEL;
              _myp->pboxcols        = (COLORS_t*) pbmpBoxCols; // colors
              _myp->boxtext.ptxtcol = (COLORS_t*) pbmpTxtCols;
              _myp->boxtext.txtsize = 2;
              _myp->boxtext.align   = TXT_RIGHT;
              _myp->boxtext.ptext   = (char*) pVal_SNUMV;
              _myp->boxtext.txtlen  = strlen(_myp->boxtext.ptext);
              pits->add((void*) _myp);
              S4HEXVAL(" new pitems pitB1SNumV   cnt idx pNext(priv)", (uint32_t) _myp, pits->count(), _myp->index(),  (uint32_t) _mypriv->pNext());
      }
  // screen and menu design
    void configMenu(md_list* pits)
      {
        STXT(" configMenu .. ");
        configTitle();
        configStatus();
        configMenuItems(pits);
        STXT(" .. fertig ");
      }
#endif