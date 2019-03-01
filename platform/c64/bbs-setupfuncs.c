/**
 * \file
 *         bbs-setupfuncs.c - setup functions for the Contiki BBS setup program
 * \author
 *         (c) 2009-2015 by Niels Haedecke <n.haedecke@unitybox.de>
 */

#include "bbs-setup.h"

void clearScreen(void) {

  clrscr();
  gotoxy(1,1);

}

void mainMenu(void) {

  board.sys_device=8;

  printf("\n*** Magnetar BBS %s setup\n", BBS_STRING_VERSION);
  printf("\n1...BBS base setup");
  printf("\n2...BBS board setup");
  printf("\n3...BBS TCP/IP setup");
  printf("\n4...BBS user editor");
  printf("\nq...Quit");
  printf("\n\n> ");

}

int nibbleIP(unsigned char *src, unsigned int *addr)
{
   unsigned char *phlp=NULL;
   unsigned int  pass=0;

   phlp = src;

   do 
   {  
     if (*phlp != '.' || '\0') 
     {
        ++phlp;
     }
     else
     {
        *phlp='\0';

        *addr = atoi(src);
        ++phlp;
        ++addr;
        src = phlp;
     }             
   } while ( *phlp != '\0');

   *addr = atoi(src);

   return 0; 
}

int networkSetup(unsigned short drive)
{
   unsigned char  tmp[40];
   unsigned short cnt, ret;

   CTK_CFG_REC mycnf;

   memset(&mycnf, 0, sizeof(CTK_CFG_REC));
   do {
 
/*             1234567890123456789012345678901234567890   */
      printf("\n* BBS network Setup\n");

      printf("\nHost IP             : ");
      gets(tmp);
      nibbleIP(tmp, mycnf.srvip);

      printf("Netmask             : ");
      gets(tmp);
      nibbleIP(tmp, mycnf.netmask);

      printf("Gateway IP          : "); 
      gets(tmp);
      nibbleIP(tmp, mycnf.gateway);

      printf("DNS IP              : ");
      gets(tmp);
      nibbleIP(tmp, mycnf.nameserv);

      printf("Mem addr. ($de08)   : ");
      gets(tmp);
      sscanf(tmp, "%x", &mycnf.mem);

      printf("Driver (cs8900a.eth): ");
      gets(tmp);
      strcpy(mycnf.driver, tmp);

      printf("Write to drive # (8): ");
      gets(tmp);
      sscanf(tmp, "%d", &drive);

      printf("\nNetwork data correct (y/n)? ");

   } while (getchar() != 'y');

   strcpy(tmp, "@0:contiki.cfg,u,w");

   ret = cbm_open(2, drive, CBM_WRITE, tmp);

   if(! ret)
   {
        for(cnt = 0; cnt <= 3; cnt++)
           cbm_write(2, &mycnf.srvip[cnt], sizeof(unsigned char));

        for(cnt = 0; cnt <= 3; cnt++)
           cbm_write(2, &mycnf.netmask[cnt], sizeof(unsigned char));

        for(cnt = 0; cnt <= 3; cnt++) 
          cbm_write(2, &mycnf.gateway[cnt], sizeof(unsigned char));

        for(cnt = 0; cnt <= 3; cnt++)
           cbm_write(2, &mycnf.nameserv[cnt], sizeof(unsigned char));

        cbm_write(2, &mycnf.mem, sizeof(&mycnf.mem));

        cbm_write(2, mycnf.driver, sizeof(mycnf.driver));

   } else {

      printf("\n\r**error - can't write file: 'contiki.cfg'");      
   }

   cbm_close(2);
   
   return ret;
}


void bbs_setup(){

  	unsigned char file[25];

	sprintf(board.board_name, "\n\r     CENTRONIAN BBS\n\r");
	board.telnet_port = 6400;
	board.max_boards = 8;

	board.subs_device = 8;
	sprintf(board.subs_prefix, "//s/");

	board.sys_device = 8;
	sprintf(board.sys_prefix, "//x/");

	board.user_device = 8;
	//sprintf(board.user_prefix, "//u/u/");
	sprintf(board.user_prefix, "//u/");

	board.userstats_device = 8;
	//sprintf(board.userstats_prefix, "//u/s/");
	sprintf(board.userstats_prefix, "//u/");

	sprintf(file, "%s:%s",board.sys_prefix, BBS_CFG_FILE);

	/* read BBS base configuration */

	sprintf(board.sub_names[0], "magnetar bbs   ");
	sprintf(board.sub_names[1], "the lounge     ");
	sprintf(board.sub_names[2], "science & tech ");
	sprintf(board.sub_names[3], "la musique     ");
	sprintf(board.sub_names[4], "hardware corner");
	sprintf(board.sub_names[5], "games & warez  ");
	sprintf(board.sub_names[6], "vic64 news     ");
	sprintf(board.sub_names[7], "great outdoors ");
	sprintf(board.sub_names[8], "member intros  ");


	board.dir_boost=1;


	//Save system stats:
	sprintf(file, "@%s:%s",board.sys_prefix, BBS_SETUP_FILE);
	cbm_save (file, board.sys_device, &board, sizeof(board));

}




void bbs_config(){

  unsigned short fsize=0;
  unsigned short siRet=0;
  unsigned char file[25];
  unsigned char i;

  sprintf(file, "%s:%s",board.sys_prefix, BBS_CFG_FILE);

  cbm_open(10, board.sys_device, 10, file);
  cbm_read(10, &bbs_config, 2);
  fsize = cbm_read(10, &bbs_config, sizeof(bbs_config));
  cbm_close(10);

  if (fsize > 0) {
    printf("\x99 config loaded from file");
  }
  else{

    printf("\x96 config file not found, setting defaults");
    /* set sub msg counts */
    board.max_boards=9;
    for(i=0;i<board.max_boards)
      bbs_config.msg_id[i]=0;
    }
  }




void bbs_stats(){

  /* read BBS stats file */
  sprintf(file, "%s:%s",board.sys_prefix, BBS_STATS_FILE);

  cbm_open(10, board.sys_device, 10, file);
  cbm_read(10, &bbs_sysstats, 2);

  fsize = cbm_read(10, &bbs_sysstats, sizeof(bbs_sysstats));
  cbm_close(10);

  if (fsize > 0) {
    printf("\x99", "stats loaded from file");
  }
  else{
    printf("\x96", "stats file not found, using defaults");
    //bbs_sysstats.last_callers[12][BBS_STATS_USRS];
    bbs_sysstats.caller_ptr=0;
    bbs_sysstats.total_calls=0;
    bbs_sysstats.total_msgs=0;
    //bbs_sysstats.daily_calls[BBS_STATS_DAYS];
    //bbs_sysstats.daily_msgs[BBS_STATS_DAYS];
    bbs_sysstats.day_ptr=0;
  }

}
