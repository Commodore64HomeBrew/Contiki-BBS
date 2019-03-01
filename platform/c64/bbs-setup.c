/**
 * \file
 *         bbs-setup.c - main program of the Contiki BBS setup program
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#include "bbs-setup.h"

/* int main(int argc, char *argv[]) { */
int main(void) {

   unsigned char drive=8;
   char input;

   do {
      /* a little effect */
      clearScreen();

      mainMenu();
      input=getchar();

      switch (input) {

         case '1':
                   clearScreen();
                   bbs_setup();
                   break;

         case '2':
                   clearScreen();
                   //boardSetup(drive);
                   break;

         case '3':
                   clearScreen();
                   networkSetup(drive);
                   break;
 
         case '4':
                   clearScreen();
                   //userSetup(drive);
                   break;
 
         default:
                   break;

      }
 
   } while (input != 'q');

   return 0;
}
