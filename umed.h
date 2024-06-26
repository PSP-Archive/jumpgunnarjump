// generated by Fast Light User Interface Designer (fluid) version 1.0104

#ifndef umed_h
#define umed_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
Fl_Double_Window* make_box();

class Fl_Editor_Map : public Fl_Box {
public:
  void draw();
  Fl_Editor_Map(int x,int y,int w,int h);
  int handle(int event);
};

class Fl_Preview_Map : public Fl_Box {
public:
  void draw();
  Fl_Preview_Map(int x,int y,int w,int h,const char *s);
};
extern Fl_Double_Window *mainWindow;
#include <FL/Fl_Group.H>
#include <FL/Fl_Round_Button.H>
extern Fl_Round_Button *rb01;
extern Fl_Round_Button *rb02;
extern Fl_Round_Button *rb03;
extern Fl_Round_Button *rb04;
extern Fl_Round_Button *rb05;
extern Fl_Round_Button *rb06;
extern Fl_Round_Button *rb07;
extern Fl_Round_Button *rb08;
extern Fl_Round_Button *rb09;
extern Fl_Round_Button *rb10;
extern Fl_Round_Button *rb11;
extern Fl_Round_Button *rb12;
extern Fl_Round_Button *rb13;
extern Fl_Round_Button *rb14;
extern Fl_Round_Button *rb15;
extern Fl_Round_Button *rb16;
extern Fl_Round_Button *rb17;
extern Fl_Round_Button *rb18;
extern Fl_Round_Button *rb19;
extern Fl_Round_Button *rb20;
extern Fl_Round_Button *rb21;
extern Fl_Round_Button *rb22;
extern Fl_Round_Button *rb23;
extern Fl_Round_Button *rb24;
extern Fl_Round_Button *rb25;
extern Fl_Round_Button *rb26;
extern Fl_Round_Button *rb27;
extern Fl_Round_Button *rb28;
extern Fl_Round_Button *rb29;
extern Fl_Round_Button *rb30;
extern Fl_Round_Button *rb31;
extern Fl_Round_Button *rb32;
extern Fl_Round_Button *rb33;
extern Fl_Round_Button *rb34;
extern Fl_Round_Button *rb35;
extern Fl_Round_Button *rb36;
extern Fl_Round_Button *rb37;
extern Fl_Round_Button *rb38;
extern Fl_Round_Button *rb39;
extern Fl_Round_Button *rb40;
extern Fl_Round_Button *rb41;
extern Fl_Round_Button *rb42;
extern Fl_Round_Button *rb43;
extern Fl_Round_Button *rb44;
extern Fl_Round_Button *rb45;
extern Fl_Round_Button *rb46;
extern Fl_Round_Button *rb47;
extern Fl_Round_Button *rb48;
extern Fl_Round_Button *rb49;
extern Fl_Round_Button *rb50;
extern Fl_Round_Button *rb51;
extern Fl_Round_Button *rb52;
extern Fl_Round_Button *rb53;
extern Fl_Round_Button *rb54;
extern Fl_Round_Button *rb55;
extern Fl_Round_Button *rb56;
extern Fl_Round_Button *rb57;
extern Fl_Round_Button *rb58;
extern Fl_Round_Button *rb59;
extern Fl_Round_Button *rb60;
extern Fl_Round_Button *rb61;
extern Fl_Round_Button *rb62;
extern Fl_Round_Button *rb63;
extern Fl_Round_Button *rb64;
extern Fl_Round_Button *rb65;
extern Fl_Round_Button *rb66;
extern Fl_Round_Button *rb67;
extern Fl_Round_Button *rb68;
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Counter.H>
extern Fl_Counter *counterWidth;
extern Fl_Counter *counterHeight;
#include <FL/Fl_Scrollbar.H>
extern Fl_Scrollbar *verticalScroll;
extern Fl_Scrollbar *horizontalScroll;
extern Fl_Editor_Map *editorMap;
#include <FL/Fl_Button.H>
extern Fl_Button *comments;
extern Fl_Preview_Map *previewMap;
extern Fl_Menu_Item menu_[];
extern Fl_Double_Window *aboutBox;
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Output.H>
Fl_Double_Window* make_about();
extern Fl_Double_Window *quickstartWindow;
Fl_Double_Window* make_quickstart();
void new_level();
void save_level();
void saveas_level();
void open_level();
void quit_editor();
extern "C" { void draw_tile(int id,int x,int y); }
extern "C" { void draw_tile_rect(int id,int x,int y,int w,int h); }
extern "C" { void play_sound(int id); }
void resized_map();
void update_button(Fl_Round_Button *button,int tile);
void undo();
void redo();
void save_undo(int x,int y, int w, int h, int tile);

class Undo {
public:
  int cells[3][3];
  int x,y,w,h;
  int tile;
};
#endif
