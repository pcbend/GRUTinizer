
#include "RConfigure.h"

#include <X11/Xlib.h>
#include <X11/xpm.h>

#include <Rtypes.h>
#include <cstdio>

#include <time.h>
#include <sys/time.h>

static Display     *gDisplay       = 0;
static Window       gLogoWindow    = 0;
static Pixmap       gLogoPixmap    = 0;
static Pixmap       gCreditsPixmap = 0;
static GC           gGC            = 0;
static XFontStruct *gFont          = 0;
static bool         gDone          = false;
static bool         gMayPopdown    = false;
static bool         gAbout         = false;
static unsigned int gWidth         = 0;
static unsigned int gHeight        = 0;
static int          gStayUp        = 4000;   // 4 seconds
static XRectangle   gCreditsRect   = { 15, 155, 285, 130 }; // clip rect in logo
static unsigned int gCreditsWidth  = gCreditsRect.width; // credits pixmap size
static unsigned int gCreditsHeight = 0;


//static Pixmap GetGrutLogo() { }


static Pixmap GetGrutLogo() {
  Pixmap logo = 0;
  Screen *xscreen = XDefaultScreenOfDisplay(gDisplay);
  if(!xscreen) { printf("xscreen default failed....\n"); return logo; }
  int depth = PlanesOfScreen(xscreen);

  XWindowAttributes win_attr;
  XGetWindowAttributes(gDisplay,gLogoWindow,&win_attr);

  XpmAttributes attr;
  attr.valuemask    = XpmVisual | XpmColormap | XpmDepth;
  attr.visual       = win_attr.visual;
  attr.colormap     = win_attr.colormap;
  attr.depth        = win_attr.depth;

  #ifdef XpmColorKey              // Not available in XPM 3.2 and earlier
  attr.valuemask |= XpmColorKey;
       if (depth > 4)   attr.color_key = XPM_COLOR;
  else if (depth > 2)   attr.color_key = XPM_GRAY4;
  else if (depth > 1)   attr.color_key = XPM_GRAY;
  else if (depth == 1)  attr.color_key = XPM_MONO;
  else                  attr.valuemask &= ~XpmColorKey;
  #endif // defined(XpmColorKey)
  
  char file[2048] = "config/Splash.xpm";
  int ret = XpmReadFileToPixmap(gDisplay, gLogoWindow,
                                 file, &logo, 0, &attr);
  XpmFreeAttributes(&attr);
  if (ret == XpmSuccess || ret == XpmColorError)
    return logo;

  printf("rootx xpm error: %s\n", XpmGetErrorString(ret));
  if (logo) XFreePixmap(gDisplay, logo);
  logo = 0;
  return logo;
}


void PopupGrutLogo(bool flag) { 
  gDisplay = XOpenDisplay("");
  if(!gDisplay) {
    //printf("gDisplay: XOpenDisplay failed.\n");
    return;
  }

  int screen = DefaultScreen(gDisplay);
  Pixel back = WhitePixel(gDisplay, screen);
  Pixel fore = BlackPixel(gDisplay, screen);
  gLogoWindow = XCreateSimpleWindow(gDisplay, DefaultRootWindow(gDisplay),
                                    -100, -100, 50, 50, 0, fore, back);
  gLogoPixmap = GetGrutLogo();
  if(!gLogoPixmap) {
    printf("getting logo pixmap from xpm file failed...\n");
    XCloseDisplay(gDisplay);
    gDisplay = 0;
    return;
  }
  Window root;
  int x,y;
  unsigned int bw,depth;
  XGetGeometry(gDisplay,gLogoPixmap,&root,&x,&y,&gWidth,&gHeight,&bw,&depth);

  Screen *xscreen = XDefaultScreenOfDisplay(gDisplay);
  if(!xscreen) { printf("xscreen default failed....\n");  XCloseDisplay(gDisplay); gDisplay=0; return;}
  x = (WidthOfScreen(xscreen)-gWidth)/2;
  y = (HeightOfScreen(xscreen)-gHeight)/2;

  XMoveResizeWindow(gDisplay, gLogoWindow, x, y, gWidth, gHeight);
  XSync(gDisplay, False);   // make sure move & resize is done before mapping

  unsigned long valmask;
  XSetWindowAttributes xswa;
  valmask = CWBackPixmap | CWOverrideRedirect;
  xswa.background_pixmap = gLogoPixmap;
  xswa.override_redirect = True;
  XChangeWindowAttributes(gDisplay, gLogoWindow, valmask, &xswa);

  gGC = XCreateGC(gDisplay, gLogoWindow, 0, 0);
  gFont = XLoadQueryFont(gDisplay, "-adobe-helvetica-medium-r-*-*-10-*-*-*-*-*-iso8859-1");

  XSelectInput(gDisplay, gLogoWindow, ButtonPressMask | ExposureMask);
  XMapRaised(gDisplay, gLogoWindow);

  //printf("now to become a logo...\n");  
  return;   
}



