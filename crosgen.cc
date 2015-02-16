//Crossword
#include <fstream>
#include <cstdlib>
#include <fcntl.h> 
#include <string.h>
#include <iostream>
#include <vector>
#include <algorithm>

//X-Window
#include <cstdio>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
using namespace std;

/*  */
//
struct CharAndCounter{
  char Char;
  int Counter;
  CharAndCounter(char _char = ' ', int _counter = 0):Char (_char), Counter(_counter) {}
};


//
struct VocElement{
    string String;
	bool Busy; //
  VocElement(const string& str = " ", bool b = false):String(str),Busy(b){}
};
 

vector< vector<CharAndCounter> > Field; // 
vector<VocElement> Vocabulary;          // 


static const char VERTICAL = 'v', HORIZONTAL = 'h';
int W,H;//  


//
struct WordCoords{
	int X, Y;
	char Dir;
	int Length;

	WordCoords(int _x, int _y, int _len, char _dir):X(_x), Y(_y), Dir(_dir), Length(_len){}

	int dx() {return (Dir == HORIZONTAL) ? 1 : 0;}
	int dy() {return (Dir == VERTICAL) ? 1 : 0;}


};	 


vector<WordCoords> Crossword; //

bool Less(const VocElement& lhs, const VocElement& rhs){
  return lhs.String.length() < rhs.String.length();
}

void ReadData(){

	ifstream crossw("crossword.txt"), voc("vocabulary_trans.txt");

  string temp;
  //
  while(!voc.eof()){
    voc >> temp;
    Vocabulary.push_back(VocElement(temp, false));
  }


  //
  sort(Vocabulary.begin(), Vocabulary.end(), Less);

  //
  //int W,H,x,y,len;
  int x,y,len;
  char dir;
  //
  crossw >> W; crossw >> H;

  for(;;){
    //
    crossw >> x; crossw >> y;
    crossw >> len; crossw >> dir;
    if(crossw.eof())
      break;
    Crossword.push_back(WordCoords(x,y,len,dir));
  }

  //
  for(int i = 0; i<W; i++){
    vector<CharAndCounter> col(H);//
	CharAndCounter _one(' ',0);//
    fill(col.begin(), col.end(), _one);//

    Field.push_back(col);
  }


 }


//
bool CanPlace(WordCoords c, const string& word){
  for(unsigned i = 0; i < word.length(); i++){
    if(Field[c.X + i*c.dx()][c.Y + i*c.dy()].Char != ' ' &&
       Field[c.X + i*c.dx()][c.Y + i*c.dy()].Char != word[i])
         return false;
  }   
          return true;
}  

//
void PlaceWord(WordCoords c, const string& word){
  for(unsigned i = 0; i < word.length(); i++){
    Field[c.X + i*c.dx()][c.Y + i*c.dy()].Char = word[i];
    Field[c.X + i*c.dx()][c.Y + i*c.dy()].Counter++;
  }
}

//
void RemoveWord(WordCoords c, const string& word){
  for(unsigned i = 0; i < word.length(); i++){
    if(--(Field[c.X + i*c.dx()][c.Y + i*c.dy()].Counter) == 0)
    Field[c.X + i*c.dx()][c.Y + i*c.dy()].Char = ' ';
  }   
}

//

bool Solve(unsigned CoordNo){

  if(CoordNo == Crossword.size())
    return true;

//
// Crossword[CoordNo].Length

  pair<vector<VocElement>::iterator, vector<VocElement>::iterator> range =
       equal_range(Vocabulary.begin(), Vocabulary.end(),
            string(Crossword[CoordNo].Length, ' '),Less);
//
  for(vector<VocElement>::iterator p = range.first;p != range.second;p++){
    if(!p->Busy && CanPlace(Crossword[CoordNo], p->String)){
    //
    // Crossword[CoordNo]
      PlaceWord(Crossword[CoordNo], p->String);// 
      p->Busy = true;//

      if(Solve(CoordNo + 1))return true;
      RemoveWord(Crossword[CoordNo], p->String);
      p->Busy = false;

    } //end if
  }//end for

  return false;
}


     


/*  */

int main(int argc, char **argv)
 {
  /* Shall print? */
  int shlprint=0; //  ,  else  if(Solve(0)),  
  char symbprint[1]; //  field[x][y].Char, : XDrawString  char, Б 
  /*  */
  ReadData();
  /*  */
  shlprint=Solve(0); 

  /* X-Window */
  Display *ourDisplay;
  int ourScreen;
  Window  myWindow;
  unsigned int display_width, display_height;
  unsigned long bgcolor;
  int myDepth;
  XSetWindowAttributes myAttr;
  Visual *myVisual;  
  XSizeHints mySizeHints;
  XClassHint myClassHint;

  GC gc;
  XEvent report;
  XKeyEvent keyreport;
  XFontStruct *font_info; // 

  XWindowAttributes windowattr; /*  */
//  char sym[1];/*  */
//  int flag = 0;

  char *myPropertyData;
  char *myClassName="crosgen";
  char *window_name="Crossword Generator Program";
  char *myResName="CrosGen";
  char *iconName="CrosGen";
  
  ourDisplay=XOpenDisplay(NULL);
  if (ourDisplay==NULL)
    {
      printf(".\n");
      return 1;
    };

  /*  */
  ourScreen=DefaultScreen(ourDisplay);           /*         */
  bgcolor=WhitePixel(ourDisplay, ourScreen);     /*         */
  myDepth=DefaultDepth(ourDisplay, ourScreen);   /*         */
  myVisual=DefaultVisual(ourDisplay, ourScreen); /*         */
  display_width = DisplayWidth(ourDisplay, ourScreen);
  display_height = DisplayHeight(ourDisplay, ourScreen);

  /*  */
  myAttr.background_pixel=bgcolor;

  /*  */
  myWindow=XCreateWindow(ourDisplay, RootWindow(ourDisplay, ourScreen),(display_width/4), (display_height/4), 620, 400, 0, myDepth, InputOutput, myVisual, CWBackPixel, &myAttr);
    
  /*  */
  XStoreName(ourDisplay,myWindow,window_name);

  /*  */
  XSetIconName(ourDisplay,myWindow,iconName);

  /*  */
  mySizeHints.flags=PMinSize | PMaxSize | PResizeInc | PPosition | PSize;
  mySizeHints.x = 0;
  mySizeHints.y = 0;
  mySizeHints.width = display_width/3;
  mySizeHints.height = display_height/4;
  mySizeHints.min_width=192; 
  mySizeHints.min_height=128;
  mySizeHints.max_width=display_width-22; 
  mySizeHints.max_height=display_height-82;
  mySizeHints.width_inc=10; mySizeHints.height_inc=10;
  XSetWMNormalHints(ourDisplay, myWindow, &mySizeHints);

  /*  */
  XSetStandardProperties(ourDisplay, myWindow, window_name, iconName, 0, argv, argc, &mySizeHints);

  /*  */
  XSetCommand(ourDisplay, myWindow, argv, argc);

  /*  */
  myClassHint.res_name=myResName;
  myClassHint.res_class=myClassName;
  XSetClassHint(ourDisplay,myWindow,&myClassHint);

  /*  */
  XSelectInput ( ourDisplay, myWindow, ExposureMask | KeyPressMask );
//  : windowattr.width;
//  : windowattr.height; 

  /*  */
  XMapWindow(ourDisplay, myWindow);

  /*  */
  XFlush(ourDisplay); // 

 /*  */
 while ( 1 ) {
  XNextEvent ( ourDisplay, &report );
  /*  */
  gc = XCreateGC ( ourDisplay, myWindow, 0 , NULL );
  XSetForeground ( ourDisplay, gc, BlackPixel ( ourDisplay, 0) );

  if ( (font_info = XLoadQueryFont( ourDisplay, "*charter-medium-i-*-25-240-*")) == NULL ) {
    printf("Font not found!\n");
    exit(1);
 }; //  
  XSetFont( ourDisplay, gc, font_info->fid ); // 
  XFlush(ourDisplay); 
  
  switch ( report.type ) {
    case Expose :
     /*  */
     if ( report.xexpose.count != 0 )
      break;

     else{//else-1 open
     /*  */
     XGetWindowAttributes(ourDisplay, myWindow, &windowattr);
/*  char **fontlist;   int num_fonts=10000;     ofstream out ("xfonts.txt");//TEMP Font  ...-courier-...-bold-...
  fontlist = XListFonts (ourDisplay, "*courier*-bold*", 1000, &num_fonts);//TEMP Font
   for (int i=0; i<num_fonts; i++) out<<fontlist[i]<<endl;//TEMP Font*/
     /*  */

     if(shlprint){
        unsigned x = 0, t = 4;
    	for(unsigned y = 0; y < Field[0].size(); y++){
	      for(x = 0; x < Field.size(); x++){
//		symbprint[0]=("%c", (char)(65+x));//TEMP
		symbprint[0]=Field[x][y].Char;
	        XDrawString ( ourDisplay, myWindow, gc, ( windowattr.width/2 - Field[y].size()*20+x*20 -20 ), (windowattr.height/2-y*t*5 ), symbprint, strlen (symbprint));
	        //cout << Field[x][y].Char;//TEMP Output

		/*  */
		if (symbprint[0]!=' ') 
			XDrawRectangle( ourDisplay, myWindow, gc, ( windowattr.width/2 - Field[y].size()*20+x*20 -22 ), ( windowattr.height/2-y*t*5 -18), strlen (symbprint)*10, strlen (symbprint)*10 );
               }
              XDrawString ( ourDisplay, myWindow, gc, x*2, y*2, "\n", 1 );//cout << endl;
         }
      }
     else{

         XDrawString ( ourDisplay, myWindow, gc, ((windowattr.width)/2-strlen ( "No solutions!" )*3), windowattr.height/2, "No solutions!", strlen("No solutions!") );//cout << "No solutions!" << endl;
  }         
     XDrawString ( ourDisplay, myWindow, gc, ((windowattr.width)/2-strlen ( "Press any key to exit!" )*4), windowattr.height-20, "Press any key to exit!", strlen ( "Press any key to exit!" ) );
}//else-1 close
// }//if-o close
  /*  */
  XFlush(ourDisplay);
  XFreeGC ( ourDisplay, gc );
     break;
        
    case KeyPress :
/*  */
//                {
//                    char    xlat[1];
//                    KeySym  key;
//                    int     count;
//		    /* Translate keycode into keysym and string */
//                    count = XLookupString(&keyreport, xlat, 
//                                  1, &key, None);
//
//		    /* Up and Down arrows */
//                    if((key == XK_Up) or (key == XK_Down))
//                    {
     /*  */
     XDestroyWindow(ourDisplay, myWindow);/*  */
     XCloseDisplay ( ourDisplay );/*  */
     exit ( 0 );
//                    }
//	         }
	default: break;
  }

 }

  return 0;
 };
