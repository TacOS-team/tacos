#ifndef _GWINDOWS_MANAGER_H_
#define _GWINDOWS_MANAGER_H_

#include <vector>

#include <gwindow.h>
#include <pronlib.h>

using namespace std;


class GWindowsManager {
  vector<GWindow*> windowsList;
  Window rootWindow;
  PronWindowAttributes rootWindowAttributes;

  int currentInitXPosition;
  int currentInitYPosition;
  int positionTranslation;

  bool overlaps(GWindow *gw);

  static GWindowsManager *instance;
  GWindowsManager() {}

 public:
  static GWindowsManager * getInstance();
  void addGWindow(GWindow *w);

  // Get the guacamole window from a window id
  // matches if it is the id of the window or of the decorator
  GWindow* getGWindow(Window w);
  GWindow* get(unsigned int wnum);
  static void init(Window rootWindow);
  PronWindowAttributes & getRootWindowAttributes();

  void destroy(Window w);

  bool findFreeSpace(int & x, int & y, int width, int height);
  void initWindowPosition(GWindow *gw);

  bool empty();

};

#endif// _GWINDOWS_MANAGER_H_

