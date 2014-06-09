#include <algorithm>
#include <stdio.h>

#include <gwindows_manager.h>

GWindowsManager * GWindowsManager::instance = NULL;
Color activeTitleColor(0x30, 0x30, 0x30);
Color inactiveTitleColor(0x60, 0x60, 0x60);
Color borderColor(0, 0, 0);
GC whiteGC, blackGC, borderGC, activeTitleGC, inactiveTitleGC;

extern Display *display;

void GWindowsManager::init(Window rootWindow) {
  if (instance != NULL) {
    delete instance;
  }

  instance = new GWindowsManager();
  instance->rootWindow = rootWindow;
  instance->positionTranslation = 100;
  instance->currentInitXPosition = instance->positionTranslation;
  instance->currentInitYPosition = instance->positionTranslation;
    
  PronGCValues values;
  values.bg = Color(0, 0, 0);
  values.fg = Color(0xbe, 0xbe, 0xbe);
  whiteGC = pronCreateGC(display, values, GC_VAL_FG | GC_VAL_BG);
  
  values.fg = Color(0, 0, 0);
  blackGC = pronCreateGC(display, values, GC_VAL_FG | GC_VAL_BG);
  
  values.fg = borderColor;
  borderGC = pronCreateGC(display, values, GC_VAL_FG | GC_VAL_BG);

  values.fg = activeTitleColor;
  activeTitleGC = pronCreateGC(display, values, GC_VAL_FG | GC_VAL_BG);

  values.fg = inactiveTitleColor;
  inactiveTitleGC = pronCreateGC(display, values, GC_VAL_FG | GC_VAL_BG);
}


GWindowsManager * GWindowsManager::getInstance() {
  return instance;
}


void GWindowsManager::addGWindow(GWindow *w) {
  this->windowsList.push_back(w);
}

GWindow* GWindowsManager::getGWindow(Window w) {
  for (size_t i = 0; i < this->windowsList.size(); ++i) {
    if (this->windowsList[i]->window            == w
          || this->windowsList[i]->parent       == w
          || this->windowsList[i]->closeButton  == w
          || this->windowsList[i]->resizeButton == w
          || this->windowsList[i]->maximiseButton == w) {
      return this->windowsList[i];
    }
  }
  return NULL;
}

GWindow* GWindowsManager::get(unsigned int wnum) {
  if (this->windowsList.size() == 0) {
    return NULL;
  }

  return this->windowsList[wnum % this->windowsList.size()];
}

PronWindowAttributes & GWindowsManager::getRootWindowAttributes() {
  return this->rootWindowAttributes;
}

void GWindowsManager::destroy(Window w) {
  for (size_t i = 0; i < this->windowsList.size(); ++i) {
    if (this->windowsList[i]->window == w || this->windowsList[i]->parent == w) {
      delete this->windowsList[i];
      this->windowsList[i] = this->windowsList[this->windowsList.size()-1];
      this->windowsList.pop_back();
    }
  }
}

bool GWindowsManager::overlaps(GWindow *gw) {
  bool res = false;
  for (size_t i = 0; i < this->windowsList.size(); ++i) {
    if (gw->overlaps(this->windowsList[i])) {
      res = true;
      break;
    }
  }
  return res;
}


struct freeSpace
{
  int x;
  int y;
  int width;
  int height;
  bool isRecoveredBy(const freeSpace & space) {
    return space.x <= this->x && space.x + space.width >= this->x + this->width
      && space.y <= this->y && space.y + space.height >= this->y + this->height;
  }
  void print () {
    printf("%d, %d, %d, %d",
      this->x, this->y, this->width, this->height);
  }
  bool operator < (const freeSpace & space) const {
    return this->x * this->height + this->y < space.x * space.height + space.y
          || this->width * this->height > space.width * space.height;
  }
};

void add(vector<freeSpace> & spaces, vector<freeSpace> & oldSpaces, freeSpace space) {
  bool covered = false;
  vector<freeSpace>::iterator it;
  for ( it = spaces.begin() ; it < spaces.end(); ) {
    //printf("\t\t\t\ttest ("); space.print(); printf(") par ("); it->print(); printf(")\n");
    if (space.isRecoveredBy(*it)) {
      covered = true;
      //printf("\t\t\tcovered\n");
      break;
    } else if (it->isRecoveredBy(space)) {
      //printf("\t\t\tcovers\n");
      spaces.erase(it);
    } else {
      ++it;
    }
  }
  for ( it = oldSpaces.begin() ; it < oldSpaces.end(); ) {
    //printf("\t\t\t\ttest ("); space.print(); printf(") par ("); it->print(); printf(")\n");
    if (space.isRecoveredBy(*it)) {
      covered = true;
      //printf("\t\t\tcovered\n");
      break;
    } else if (it->isRecoveredBy(space)) {
      //printf("\t\t\tcovers\n");
      oldSpaces.erase(it);
    } else {
      ++it;
    }
  }
  if (!covered) {
    spaces.push_back(space);
  }
  /*for (size_t i = 0; i < spaces.size(); ++i) {
    printf("\t\tRes final %d (%d, %d, %d, %d)\n",
      (int)i,
      spaces[i].x, spaces[i].y, spaces[i].width, spaces[i].height);
  }*/
}

bool GWindowsManager::findFreeSpace(int & x, int & y, int width, int height) {
  //printf("findFreeSpace (width, height) : %d, %d\n", width, height);
  freeSpace root;
  root.x      = 0;
  root.y      = 0;
  root.width  = this->rootWindowAttributes.width;
  root.height = this->rootWindowAttributes.height;
  vector<freeSpace> oldSpaces;
  oldSpaces.reserve(this->windowsList.size()*this->windowsList.size());
  vector<freeSpace> newSpaces;
  newSpaces.reserve(this->windowsList.size()*this->windowsList.size());

  if (root.height >= height && root.width > width) {
    newSpaces.push_back(root);
  }

  freeSpace current;
  for (size_t i = 0; i < this->windowsList.size(); ++i) {
    oldSpaces = newSpaces;
    sort(oldSpaces.begin(), oldSpaces.end());
    newSpaces.clear();
    GWindow * gw = this->windowsList[i];
    /*printf("Nouvelle Window (x, y, width, height) : %d, %d, %d, %d\n",
      gw->parentAttributes.x, gw->parentAttributes.y, gw->parentAttributes.width, gw->parentAttributes.height);*/
    while (!oldSpaces.empty())
    {
      current = oldSpaces.front();
      oldSpaces.erase(oldSpaces.begin());
      //printf("Nouvel espace (x,y,width, height) : "); current.print(); printf("\n");
      bool isCut = false;
      // Coupure du haut (y of the window in the vertical space)
      // Et point HG à droite du coté gauche et point HD à gauche du cté droit
      if (current.y < gw->parentAttributes.y && gw->parentAttributes.y < current.y + current.height
          && current.x < gw->parentAttributes.x + width
          && current.x + current.width > gw->parentAttributes.x) {
        freeSpace newSpace = current;
        newSpace.height = gw->parentAttributes.y - current.y;
        //printf("\tCoupure en haut faite (x,y,width, height) : "); newSpace.print(); printf("\n");
        if (newSpace.height >= height && newSpace.width >= width) {
          //printf("\tCoupure valide !!!!!!!!!!!!!!\n");
          add(newSpaces, oldSpaces, newSpace);
        }
        isCut = true;
      }
      // Coupure de la gauche
      // Et point HG au dessus du bas et point BG en dessous du haut
      if (current.x < gw->parentAttributes.x && gw->parentAttributes.x < current.x + current.width
          && current.y + current.height > gw->parentAttributes.y
          && current.y < gw->parentAttributes.y + gw->parentAttributes.height) {
        freeSpace newSpace = current;
        newSpace.width = gw->parentAttributes.x - current.x;
        //printf("\tCoupure à gauche faite (x,y,width, height) : "); newSpace.print(); printf("\n");
        if (newSpace.height >= height && newSpace.width >= width) {
          //printf("\tCoupure valide !!!!!!!!!!!!!!\n");
          add(newSpaces, oldSpaces, newSpace);
        }
        isCut = true;
      }
      // Coupure de droite
      // Et point HG au dessus du bas et point BG en dessous du haut
      if (current.x + current.width > gw->parentAttributes.x + gw->parentAttributes.width
          && current.x < gw->parentAttributes.x + gw->parentAttributes.width
          && current.y + current.height > gw->parentAttributes.y
          && current.y < gw->parentAttributes.y + gw->parentAttributes.height) {
        freeSpace newSpace = current;
        newSpace.x = gw->parentAttributes.x + gw->parentAttributes.width;
        newSpace.width = current.x + current.width - newSpace.x;
        //printf("\tCoupure à droite faite (x,y,width, height) : "); newSpace.print(); printf("\n");
        if (newSpace.height >= height && newSpace.width >= width) {
          //printf("\tCoupure valide !!!!!!!!!!!!!!\n");
          add(newSpaces, oldSpaces, newSpace);
        }
        isCut = true;
      }
      // Coupure du bas
      // Et point HG à droite du coté gauche et point HD à gauche du cté droit
      if (current.y < gw->parentAttributes.y + gw->parentAttributes.height
          && current.y + current.height > gw->parentAttributes.y + gw->parentAttributes.height
          && current.x < gw->parentAttributes.x + width
          && current.x + current.width > gw->parentAttributes.x) {
        freeSpace newSpace = current;
        newSpace.y = gw->parentAttributes.y + gw->parentAttributes.height;
        newSpace.height = current.y + current.height - newSpace.y;
        //printf("\tCoupure en bas faite (x,y,width, height) : "); newSpace.print(); printf("\n");
        if (newSpace.height >= height && newSpace.width >= width) {
          //printf("\tCoupure valide !!!!!!!!!!!!!!\n");
          add(newSpaces, oldSpaces, newSpace);
        }
        isCut = true;
      }
      if (!isCut) {
        newSpaces.push_back(current);
      }
    }
  }
  sort(newSpaces.begin(), newSpaces.end());
  /*printf("\n");
  for (size_t i = 0; i < newSpaces.size(); ++i) {
    printf ("debug space %d (x, y, width, height) : %d, %d, %d, %d\n",
      (int)i, newSpaces[i].x, newSpaces[i].y, newSpaces[i].width, newSpaces[i].height);
  }*/
  if (!newSpaces.empty())
  {
    freeSpace res = newSpaces.front();
    x = res.x;
    y = res.y;
    //printf("Position trouvée : %d, %d\n", x, y);
    return true;
  } else {
    // The height stands at the current y position
    if (this->currentInitYPosition + height < this->rootWindowAttributes.height) {
      if (this->currentInitXPosition + width > this->rootWindowAttributes.width) {
        this->currentInitXPosition = this->positionTranslation;
        this->currentInitYPosition = this->positionTranslation;
      }
    } else {
      this->currentInitYPosition = this->positionTranslation;
      this->currentInitXPosition += this->positionTranslation;
      // If the new position is not big enougth
      if (this->currentInitYPosition + height > this->rootWindowAttributes.height
          || this->currentInitXPosition + width > this->rootWindowAttributes.width) {
        this->currentInitXPosition = this->positionTranslation;
        this->currentInitYPosition = this->positionTranslation;
      }
    }
    x = this->currentInitXPosition;
    y = this->currentInitYPosition;
    this->currentInitYPosition += this->positionTranslation;
    //printf("PAS de position libre trouvée\n");
    return false;
  }
}

void GWindowsManager::initWindowPosition(GWindow *gw) {
  this->findFreeSpace(gw->parentAttributes.x, gw->parentAttributes.y,
                        gw->parentAttributes.width, gw->parentAttributes.height);
}

bool GWindowsManager::empty() {
  return this->windowsList.empty();
}
