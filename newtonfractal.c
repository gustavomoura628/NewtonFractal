#include <SDL2/SDL.h> 
//#include <SDL2/SDL_image.h> 
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h> 
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <complex.h>

const int SCREEN_WIDTH = 1366;
const int SCREEN_HEIGHT = 768;
const int FPS = 60;

void CSDL_RenderDrawRectangle(SDL_Renderer* renderer, int x, int y, int w, int h)
{
	int i;
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    SDL_RenderFillRect(renderer,&r);
}
void CSDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int r)
{
	int i,j;
	j=0;
	i=r;
	int k;
	int f=0;
	for(j=0;i>=j;j++){
		f=j*j+i*i-r*r;
		if(f>0)i--;
		SDL_RenderDrawLine(renderer, x-j, y+i, x+j, y+i);
		SDL_RenderDrawLine(renderer, x-j, y-i, x+j, y-i);
		SDL_RenderDrawLine(renderer, x-i, y+j, x+i, y+j);
		SDL_RenderDrawLine(renderer, x-i, y-j, x+i, y-j);
	}
}
void CSDL_RenderDrawLine(SDL_Renderer* renderer, int x0, int y0, int x1, int y1, int r)
{
    int dx = abs(x0-x1);
    int dy = abs(y0-y1);

	int i,j;
	for(i=-r/2;i<r/2;i++){
        if(dx<dy){
            SDL_RenderDrawLine(renderer, x0+i, y0, x1+i, y1);
        }else{
            SDL_RenderDrawLine(renderer, x0, y0+i, x1, y1+i);
        }
	}
}
void CSDL_RenderDrawRoundLine(SDL_Renderer* renderer, int x0, int y0, int x1, int y1, int r)
{
    CSDL_RenderDrawLine(renderer,x0,y0,x1,y1,r);
    CSDL_RenderDrawCircle(renderer,x0,y0,r/2);
    CSDL_RenderDrawCircle(renderer,x1,y1,r/2);
}
void CSDL_RenderDrawText(SDL_Renderer * renderer, char * textInput, int x, int y, int textSize, int outline_size)
{
    TTF_Font *Buran = TTF_OpenFont("fonts/Buran.ttf", textSize); //this opens a font style and sets a size
    TTF_Font *font_outline = TTF_OpenFont("fonts/Buran.ttf", textSize); 
    TTF_SetFontOutline(font_outline, outline_size); 
    SDL_Color textColor;
    SDL_GetRenderDrawColor(renderer, &textColor.r, &textColor.g, &textColor.b, &textColor.a);
    //= {r, g, b, 0};
    SDL_Color inverseTextColor = {255 - textColor.r, 255 - textColor.g, 255 - textColor.b, 0};
    SDL_Surface* surfaceMessage = TTF_RenderText_Blended(Buran, textInput, textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
    SDL_Surface* surfaceOutline = TTF_RenderText_Blended(font_outline, textInput, inverseTextColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
    TTF_CloseFont(Buran);
    TTF_CloseFont(font_outline);
    SDL_Rect rect = { outline_size, outline_size, surfaceMessage->w, surfaceMessage->h}; 
    SDL_SetSurfaceBlendMode(surfaceMessage, SDL_BLENDMODE_BLEND);
    SDL_BlitSurface(surfaceMessage, NULL, surfaceOutline, &rect);
    //SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture
    SDL_Texture* Outline = SDL_CreateTextureFromSurface(renderer, surfaceOutline); //now you can convert it into a texture
    SDL_FreeSurface(surfaceMessage); 
    SDL_FreeSurface(surfaceOutline); 
    SDL_Rect Outline_rect; //create a rect
    SDL_QueryTexture(Outline, NULL, NULL, &Outline_rect.w, &Outline_rect.h); 
    Outline_rect.x = x-(Outline_rect.w/2);  //controls the rect's x coordinate 
    Outline_rect.y = y-(Outline_rect.h/2); // controls the rect's y coordinte
    SDL_RenderCopy(renderer, Outline, NULL, &Outline_rect); 
    SDL_DestroyTexture(Outline); 	
}

float myrand(float a, float b)
{
    float r = rand()/(float)RAND_MAX;
    return r*fabs(a-b)+fmin(a,b);
}

int main(int argc, char ** argv) 
{ 
	int i;
    //Initialize srand
    struct timespec t;
    timespec_get(&t,TIME_UTC);
	srand(t.tv_nsec ^ t.tv_sec);

    //Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) printf("error initializing SDL: %s\n", SDL_GetError()); 
	 
	SDL_Window* window = SDL_CreateWindow("GAME", // creates a window 
									SDL_WINDOWPOS_CENTERED, 
									SDL_WINDOWPOS_CENTERED, 
									SCREEN_WIDTH, SCREEN_HEIGHT, 0); 
	Uint32 render_flags = SDL_RENDERER_ACCELERATED; 
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags); 
	TTF_Init();

	//controls animation loop 
	int close = 0; 
	int pause = 1;

    //Declares input handling variables
	int keyboard[1000];
	int keyboardprev[1000];
	int keyboardonce[1000];
	int keyboarddown[1000];
	int keyboardup[1000];
	int mouse[10];
	int mouseprev[10];
	int mouseonce[10];
	int mousedown[10];
	int mouseup[10];
	unsigned int counter;
	int lastTime = SDL_GetTicks();
	memset(keyboard,0,sizeof keyboard);
	
	int mouseX;
	int mouseY;

    //Declares Coordinate related variables
    double kx = 100;
    double ky = -100;
    double bx = SCREEN_WIDTH/2.0/kx;
    double by = SCREEN_HEIGHT/2.0/ky;

    //moving coordinate system:
    double coordmouseclickdownx = 0;
    double coordmouseclickdowny = 0;
    double coordmouseclickholdx = 0;
    double coordmouseclickholdy = 0;
    double coordmouseclickupx = 0;
    double coordmouseclickupy = 0;
    double coordmouseoriginalbx = 0;
    double coordmouseoriginalby = 0;
    int coormousetranslation = 0;

    //Declares Newton Fractal Related Variables
    int numberofiterations = 10;
    int levelofdetail = 4;

    double x0 = 1+0;
    double y0 = 1+0;
    double x1 = 1+1;
    double y1 = 1+0;
    double x2 = 1+.5;
    double y2 = 1+sqrt(3)/2;
    int pointtranslation = 0;

	//Animation loop 
	while (!close)
    {
		// Events mangement 
		SDL_Event event; 
		while (SDL_PollEvent(&event)) {
			switch (event.type) { 

			case SDL_QUIT: 
				// handling of close button 
				close = 1; 
				break; 
			case SDL_KEYDOWN:
				keyboard[event.key.keysym.scancode] = 1;
				keyboarddown[event.key.keysym.scancode] = 1;
				break;
			case SDL_KEYUP:
				keyboard[event.key.keysym.scancode] = 0;
				keyboardup[event.key.keysym.scancode] = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouse[event.button.button]=1;
				mousedown[event.button.button]=1;
				break;
			case SDL_MOUSEBUTTONUP:
				mouse[event.button.button]=0;
				mouseup[event.button.button]=1;
				break;
            case SDL_MOUSEWHEEL:
                if(event.wheel.y > 0) // scroll up
                {
                    //Handles zoom
                    double Sx = (mouseX)/kx-bx;
                    double Sy = (mouseY)/ky-by;
                    kx*=1.1;
                    ky*=1.1;
                    bx = (mouseX)/kx - Sx;
                    by = (mouseY)/ky - Sy;
                }
                else if(event.wheel.y < 0) // scroll down
                {
                    //Handles zoom
                    double Sx = (mouseX)/kx-bx;
                    double Sy = (mouseY)/ky-by;
                    kx*=1/(1.1);
                    ky*=1/(1.1);
                    bx = (mouseX)/kx - Sx;
                    by = (mouseY)/ky - Sy;
                }
                break;
			}
		} 

        //Input management
        //handle keyboardonce variable
		for(i=0;i<1000;i++)
		{
			keyboardonce[i] = 0;
			if(!keyboardprev[i] && keyboard[i])keyboardonce[i] = 1;
			keyboardprev[i] = keyboard[i];
		}
        //handle mouseonce variable
		for(i=0;i<10;i++){
			mouseonce[i] = 0;
			if(!mouseprev[i] && mouse[i])mouseonce[i]=1;
			mouseprev[i]=mouse[i];
		}
        SDL_GetMouseState(&mouseX,&mouseY);

        //handle actions
		if(keyboardonce[SDL_SCANCODE_RETURN])
		{
		}
		if(keyboard[SDL_SCANCODE_R])
		{
            //resets coordinate system
            kx = 100;
            ky = -100;
            bx = SCREEN_WIDTH/2.0/kx;
            by = SCREEN_HEIGHT/2.0/ky;
        }

		if(keyboardonce[SDL_SCANCODE_K])
		{
            //changes pixel size
            levelofdetail=fmax(levelofdetail-1,1);
		}
		if(keyboardonce[SDL_SCANCODE_J])
		{
            //changes pixel size
            levelofdetail++;
		}

		if(keyboardonce[SDL_SCANCODE_N])
		{
            //changes how deep the fractal goes
            numberofiterations++;
		}
		if(keyboardonce[SDL_SCANCODE_P])
		{
            //changes how deep the fractal goes
            numberofiterations=fmax(numberofiterations-1,0);
		}
		if(keyboardonce[SDL_SCANCODE_L])
		{
		}
		if(keyboardonce[SDL_SCANCODE_SPACE])
		{
			pause = !pause;
		}
		if(keyboard[SDL_SCANCODE_DOWN])
		{
		}
		if(keyboard[SDL_SCANCODE_UP])
		{
		}
        

		// clears the screen 
		SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
		SDL_RenderClear(renderer); 
		

        //Handles translating with mouse
        if(mouseonce[1]){
            coormousetranslation=0;
            pointtranslation=-1;
            if(pow((x0+bx)*kx - mouseX,2)+pow((y0+by)*ky - mouseY,2)<50){
                pointtranslation=0;
            }
            else if(pow((x1+bx)*kx - mouseX,2)+pow((y1+by)*ky - mouseY,2)<50){
                pointtranslation=1;
            }
            else if(pow((x2+bx)*kx - mouseX,2)+pow((y2+by)*ky - mouseY,2)<50){
                pointtranslation=2;
            }else{
                //translates coordinate system
                coormousetranslation = 1;
                coordmouseclickdownx = mouseX;
                coordmouseclickdowny = mouseY;
                coordmouseoriginalbx = bx;
                coordmouseoriginalby = by;
            }
        }
        if(mouse[1]){
            if(pointtranslation==0){
                x0 = mouseX/kx-bx;
                y0 = mouseY/ky-by;
            }
            if(pointtranslation==1){
                x1 = mouseX/kx-bx;
                y1 = mouseY/ky-by;
            }
            if(pointtranslation==2){
                x2 = mouseX/kx-bx;
                y2 = mouseY/ky-by;
            }else if(coormousetranslation==1){
                //translates coordinate system
                    double Sx = (coordmouseclickdownx)/kx-coordmouseoriginalbx;
                    double Sy = (coordmouseclickdowny)/ky-coordmouseoriginalby;
                    bx = (mouseX)/kx - Sx;
                    by = (mouseY)/ky - Sy;
            }
        }

        int i,j,k;

        //Declares Polynomial related variables
        double complex z0 = x0+y0*I;
        double complex z1 = x1+y1*I;
        double complex z2 = x2+y2*I;
        double complex a = 1;
        double complex b = -z0-z1-z2;
        double complex c = z0*z1+z1*z2+z0*z2;
        double complex d = -z0*z1*z2;


        //Loops for each metapixel and assigns its color
        for(i=0,k=levelofdetail;i<SCREEN_HEIGHT;i+=k){
            for(j=0;j<SCREEN_WIDTH;j+=k){
                //creates point
                double x = j/kx-bx;
                double y = i/ky-by;
                double complex z = x+y*I;

                //performs newton's method
                int l;
                for(l=0;l<numberofiterations;l++){
                    z -= (a*z*z*z+b*z*z+c*z+d)/(3*a*z*z+2*b*z+c);
                }

                //find nearest point
                x = creal(z);
                y = cimag(z);
                double max = (x0-x)*(x0-x)+(y0-y)*(y0-y);
                double maxi = 0;
                double dist = (x1-x)*(x1-x)+(y1-y)*(y1-y);
                if(dist < max){
                    maxi=1;
                    max = dist;
                }
                dist = (x2-x)*(x2-x)+(y2-y)*(y2-y);
                if(dist < max){
                    maxi=2;
                    max = dist;
                }


                //assigns color according to nearest point
                SDL_SetRenderDrawColor(renderer,(maxi==0)*255,(maxi==1)*255,(maxi==2)*255,255);

                //handles pixel/metapixel drawing
                if(k==1)SDL_RenderDrawPoint(renderer,j,i);
                else CSDL_RenderDrawRectangle(renderer, j,i,k,k);
            }
        }
        
        //Draws Axis and related
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        //Draws axis
        CSDL_RenderDrawRoundLine(renderer, 0, ( 0+by)*ky, SCREEN_WIDTH, (0+by)*ky, 4);
        CSDL_RenderDrawRoundLine(renderer, ( 0+bx)*kx, 0, (0+bx)*kx, SCREEN_HEIGHT, 4);
        //Draws Circles representing y-axis units
        for(i=-by+1;i>SCREEN_HEIGHT/ky-by-1;i--){
            CSDL_RenderDrawCircle(renderer,(0+bx)*kx,(i+by)*ky,15);

            //char str[100];
            char buffer[100];
            sprintf(buffer, "%d",i);
            //strcat(buffer,str);
            if(i!=0)CSDL_RenderDrawText(renderer,buffer, (30/kx+bx)*kx, (i+by)*ky, SCREEN_WIDTH*0.02,SCREEN_WIDTH*0.003);

        }
        //Draws Circles representing x-axis units
        for(j=-bx-1;j<SCREEN_WIDTH/kx-bx+1;j++){
            CSDL_RenderDrawCircle(renderer,(j+bx)*kx,(0+by)*ky,15);
            char buffer[100];
            sprintf(buffer, "%d",j);
            if(j!=0)CSDL_RenderDrawText(renderer,buffer, (j+bx)*kx, (-30/ky+by)*ky, SCREEN_WIDTH*0.02,SCREEN_WIDTH*0.003);
        }

        //Draws points related to fractal
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        CSDL_RenderDrawCircle(renderer, (x0+bx)*kx,(y0+by)*ky,10);
        CSDL_RenderDrawCircle(renderer, (x1+bx)*kx,(y1+by)*ky,10);
        CSDL_RenderDrawCircle(renderer, (x2+bx)*kx,(y2+by)*ky,10);
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        CSDL_RenderDrawCircle(renderer, (x0+bx)*kx,(y0+by)*ky,6);
        CSDL_RenderDrawCircle(renderer, (x1+bx)*kx,(y1+by)*ky,6);
        CSDL_RenderDrawCircle(renderer, (x2+bx)*kx,(y2+by)*ky,6);


        //Example of how to draw a curve
        //Drawing a curve
        //SDL_SetRenderDrawColor(renderer,0,0,255,255);
        //double dx = SCREEN_WIDTH/kx*0.005;
        //double x = -bx-1-dx;
        ////double a = -fmax((double)(counter%200)-100.0,-(double)(counter%200)+100.0)/100.0*10;
        //double a =-4;
        //double fpx = pow(x,-a);
        //for(x=-bx-1;x<SCREEN_WIDTH/kx-bx+1;){
        //    double fx = pow(x,a);
        //    //CSDL_RenderDrawCircle(renderer,(j+bx)*kx,(j*j+by)*ky,3);
        //    if(!(fx!=fx || fpx != fpx)){
        //        if(((fx+by)*ky>0&&(fx+by)*ky<SCREEN_HEIGHT) || ((fpx+by)*ky>0&&(fpx+by)*ky<SCREEN_HEIGHT)){
        //                CSDL_RenderDrawRoundLine(renderer, ( x+bx)*kx, (fx+by)*ky, (x-dx+bx)*kx, (fpx+by)*ky, 5);
        //        }
        //    }
        //    fpx=fx;
        //    x+=dx;
        //}

        //Displays coordinates of a point
        if(mouse[2]){
            SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
            char buffer[100];
            sprintf(buffer, "(%.4lf,%.4lf)",(mouseX/kx-bx),(mouseY/ky-by));
            CSDL_RenderDrawText(renderer,buffer,mouseX,mouseY-30, SCREEN_WIDTH*0.02,SCREEN_WIDTH*0.003);
            CSDL_RenderDrawCircle(renderer,mouseX,mouseY,10);
        }




        //Text Display Example
//		char buffer[100]="Test";
//		SDL_SetRenderDrawColor(renderer,255,255,255,255);
//		CSDL_RenderDrawText(renderer,buffer, SCREEN_WIDTH/2, SCREEN_HEIGHT/15, SCREEN_WIDTH/30, 3);
		
		SDL_RenderPresent(renderer); 

		// calculates to 60 fps 
		counter++;
		//SDL_Delay(1000 / FPS);
	} 

	//SDL cleanup
	// destroy renderer 
	SDL_DestroyRenderer(renderer); 

	// destroy window 
	SDL_DestroyWindow(window);	

	return 0; 
} 

