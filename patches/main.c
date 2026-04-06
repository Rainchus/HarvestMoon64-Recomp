#include "patches.h"
#include "os.h"

int dummy = 1;
int dummyBSS;

extern volatile u8 stepMainLoop;
extern volatile u16 engineStateFlags;

void yield_self_1ms(void);
extern volatile u32	nuGfxTaskSpool;	/* number of tasks in the queue */

RECOMP_PATCH void nuGfxTaskAllEndWait(void) {
    while (nuGfxTaskSpool) {
        yield_self_1ms();
    }
}

// start up before main loop
RECOMP_PATCH void func_80026284(void) {
    u32 count = 1;
    goto loop_end;
    
    // first 60 frames
    while (!(engineStateFlags & 2)) {
        
        u16 counter = 1;
        u16 currentCount;

        if (count != 0) {
            
            do {
            
                stepMainLoop = FALSE;
                
                while (stepMainLoop == FALSE) {
                    yield_self_1ms();
                }

                currentCount = counter;
                counter++;

            } while (currentCount < count);

        }
        // func_80026248(1);

loop_end:     
        
    }

}

#define MAIN_LOOP_CALLBACK_FUNCTION_TABLE_SIZE 57
extern volatile u16 mainLoopCallbackCurrentIndex;
extern void (*mainLoopCallbacksTable[MAIN_LOOP_CALLBACK_FUNCTION_TABLE_SIZE])();

extern volatile u16 D_80182BA0;
extern volatile u16 D_8020564C;
extern void *currentGfxTaskPtr;

void nuGfxDisplayOn(void);

void resetBitmaps(void);
void updateAudio(void); 
void resetSceneNodeCounter(void);
void updateCutsceneExecutors(void);
void updateEntities(void);
void updateMapController(void);
void updateMapGraphics(void);
void updateNumberSprites(void);
void updateSprites(void); 
void dmaSprites(void); 
void updateBitmaps(void); 
void updateMessageBox(void); 
void updateDialogues(void);
void func_800293B8(void); 
// per 60; 1 = 60 fps
extern volatile u8 frameRate;
extern volatile u8 D_80237A04;
extern volatile u8 retraceCount;
void func_8004DEC8(void);
int rand(void);
void nuGfxDisplayOff(void);

extern s32 sRandNext;

RECOMP_PATCH void mainLoop(void) {

    stepMainLoop = FALSE;
    engineStateFlags = 1;
    
    // wait 60 frames until engineStateFlags |= 2
    func_80026284();

    // toggle flags
    func_8004DEC8();
    
    // could be inline func_80026230
    D_80182BA0 = 1;
    D_8020564C = 0;

    while (TRUE) {
      
        nuGfxDisplayOn();
          
        while (engineStateFlags & 1) {
            
            while (stepMainLoop == FALSE) {
                yield_self_1ms();
            }
            
            if (!D_8020564C) { 
              
              D_80182BA0 = 1;
              
              // handle specific logic depending on game mode/screen
              mainLoopCallbacksTable[mainLoopCallbackCurrentIndex](); 

              D_8020564C = D_80182BA0; 

            } 
            
            D_8020564C -= 1;    

            // dead code
            if (D_8020564C) {

            }
            
            resetBitmaps();
            updateAudio(); 
            resetSceneNodeCounter();
            updateCutsceneExecutors();
            updateEntities();
            updateMapController();
            updateMapGraphics();
            updateNumberSprites();
            updateSprites(); 
            dmaSprites(); 
            updateBitmaps(); 
            updateMessageBox(); 
            updateDialogues();

            // no op
            // shelved or debug code
            func_800293B8(); 

            stepMainLoop = FALSE; 

            // unused
            D_80237A04 = retraceCount;
              
            // update RNG seed
            // TODO: implement this somehow
            // the name `rand` conflicts with other things named rand
            // and then the sRandNext is required to be static...so you cant reference it directly
            //rand();
            //sRandNext = sRandNext * 1103515245 + 12345;
        }
        nuGfxDisplayOff();
    }
    
}
