#include "common.h"

#include "parse_para.h"


int print_version(void)
{
	printf("current version: \tagent %4.2f\ncreate time: \t\t%s\n", VERSION, CREATETIME);
	return 0;
}


int print_help(void)
{
        printf("                   \
this is a help document!        \n \
usage: ./agent [OPTION]...      \n \
list infomation of servers , \
for example: cpu, memory, disk, io, network...  \n\n \
collect info about cpu:         \n \
        --cpu  <cpu sequence>    \n \
        --load <miniute>        \n\n \
collect info about mem:         \n \
        --mproc \n \
        --mused \n \
        --mpri  \n \
        --mvir  \n \
        --mpri_ipcs     \n \
        --mswap_used     \n \
        --mswap_total   \n \
        --pgpgin        \n      \n \
collect info about disk:        \n \
        --disk <disk name>  --await      \n \
        --disk <disk name>  --aveq       \n \
        --disk <disk name>  --avgrq_sz   \n \
        --disk <disk name>  --util       \n\n \
collect info about exchange of mem and disk:    \n \
        --pgpgout       \n \
        --pswpin        \n \
        --pswpout       \n \
        --svctm sda     \n\n \
collcet info about network flow:        \n \
        --int <interface>  --net_byte_in         \n \
        --int <interface>  --net_byte_out        \n \
        --int <interface>  --net_pack_in        \n \
        --int <interface>  --net_pack_out        \n \
collect info about tcp or udp connecton:        \n \
        --passiveopens  \n \
        --currestab     \n \
        --indatagrams   \n \
        --outdatagrams  \n\n \
collect info about hardware:    \n\
        --hard_proc     \n \
        --hard_mem      \n \
        --hard_temps    \n \
        --hard_disk     \n \
        --hard_pwr      \n \
        --hard_batt     \n \
        --hard_nics     \n \
        --hard_fans     \n \
");

        return 0;
}


int parse_para(int argc, char **argv)
{
        // parse the monitor agent parameters
        int c, index;
        char *optstring = "i:l:m:n:s:p:dtvh";

        char *version, *help;
        char *disk, *cpu_seq, *interface, *loadlevel, *processid;
        char *mproc, *mused, *mpri, *mvir, *mpri_ipcs;
        char *mswap_used, *mswap_total, *pgpgin;
        char *pgpgout, *pswpin, *pswpout, *svctm, *await, *aveq, *avgrq_sz, *util;
        char *net_byte_in, *net_byte_out, *net_pack_in, *net_pack_out;
        char *passiveopens, *currestab, *indatagrams, *outdatagrams;
        char *hard_proc, *hard_mem, *hard_temp, *hard_disk,
                *hard_pwr, *hard_batt, *hard_nics, *hard_fans;


        char * para[40] ;

        int i = 0;
        for ( i = 0; i<40; i++ ) { para[i] = NULL; }


        struct option longopts[] = {
/* 0 */         {"help", no_argument, NULL, 'h'},
/* 1 */         {"version", no_argument, NULL, 'v'},
/* 2 */         {"daemon", no_argument, NULL, 'd'},

/* 3 */         {"url_post", required_argument, NULL, 'd'},
/* 4 */         {"rul_server", required_argument, NULL, 0},
/* 5 */         {"url_config", required_argument, NULL, 0},
/* 6 */         {"url_report", required_argument, NULL, 0},
/* 7 */         {"processid", required_argument, NULL, 0},


/* 8 */         {"mproc", no_argument, NULL, 0},
/* 9 */         {"mused", no_argument, NULL, 0},
/* 10 */        {"mpri", no_argument, NULL, 0},
/* 11 */        {"mvir", no_argument, NULL, 0},
/* 12 */        {"mpri_ipcs", no_argument, NULL, 0},

/* 13 */        {"mswap_used", no_argument, NULL, 0},
/* 14 */        {"mswap_total", no_argument, NULL, 0},

/* 15 */        {"pgpgin", no_argument, NULL, 0},
/* 16 */        {"pgpgout", no_argument, NULL, 0},
/* 17 */        {"pswpin", no_argument, NULL, 0},
/* 18 */        {"pswpout", no_argument, NULL, 0},

/* 19 */        {"svctm", no_argument, NULL, 0},
/* 20 */        {"await", no_argument, NULL, 0},
/* 21 */        {"aveq", no_argument, NULL, 0},
/* 22 */        {"avgrq_sz", no_argument, NULL, 0},
/* 23 */        {"util", no_argument, NULL, 0},

/* 24 */        {"net_byte_in", no_argument, NULL, 0},
/* 25 */        {"net_byte_out", no_argument, NULL, 0},
/* 26 */        {"net_pack_in", no_argument, NULL, 0},
/* 27 */        {"net_pack_out", no_argument, NULL, 0},

/* 28 */        {"passiveopens", no_argument, NULL, 0},
/* 29 */        {"currestab", no_argument, NULL, 0},
/* 30 */        {"indatagrams", no_argument, NULL, 0},
/* 31 */        {"outdatagrams", no_argument, NULL, 0},

/* 32 */        {"hard_proc", no_argument, NULL, 0},
/* 33 */        {"hard_mem", no_argument, NULL, 0},
/* 34 */        {"hard_temps", no_argument, NULL, 0},
/* 35 */        {"hard_disk", no_argument, NULL, 0},
/* 36 */        {"hard_pwr", no_argument, NULL, 0},
/* 37 */        {"hard_batt", no_argument, NULL, 0},
/* 38 */        {"hard_nics", no_argument, NULL, 0},
/* 39 */        {"hard_fans", no_argument, NULL, 0},


/* end */       {NULL, no_argument, NULL, 0},

        };


        while ((c = getopt_long(argc, argv, optstring, longopts, &index)) != -1) {
                switch (c) {


                case 'h':
                        para[0]="";
                        break;

                case 'v':
                        para[1]="";
                        break;

                case 'd':
                        para[2]="";
                        break;

                case 'i':
                        para[5]=optarg;
                        break;

                case 'l':
                        loadlevel = optarg;
                        break;

                case '?':
                        c = (char)optopt;
                        printf("parameters is not correct! \n");
                        break;

                case ':':
                        printf("1111111111111111111\n");
                        break;
                }


                switch(index){
                case 0:
                        para[0] = "";
                        break;

                case 1:
                        para[1] = "";
                        break;

                case 2:
                        para[2] = "";
                        break;

                case 3:
                        para[3] = optarg;
                        break;

                case 4:
                        para[4] = optarg;
                        break;

                case 5:
                        para[5] = optarg;
                        break;

                case 6:
                        para[6] = optarg;
                        break;

                case 7:
                        para[7] = optarg;
                        break;

                case 8:
                        para[8] = "";
                        break;

                case 9:
                        para[9] = "";
                        break;

                case 10:
                        para[10] = "";
                        break;

                case 11:
                        para[11] = "";
                        break;

                case 12:
                        para[12] = "";
                        break;

                case 13:
                        para[13] = "";
                        break;

                case 14:
                        para[14] = "";
                        break;

                case 15:
                        para[15] = "";
                        break;

                case 16:
                        para[16] = "";
                        break;

                case 17:
                        para[17] = "";
                        break;

                case 18:
                        para[18] = "";
                        break;

                case 19:
                        para[19] = "";
                        break;

                case 20:
                        para[20] = "";
                        break;

                case 21:
                        para[21] = "";
                        break;

                case 22:
                        para[22] = "";
                        break;

                case 23:
                        para[23] = "";
                        break;

                case 24:
                        para[24] = "";
                        break;

                case 25:
                        para[25] = "";
                        break;

                case 26:
                        para[26] = "";
                        break;

                case 27:
                        para[27] = "";
                        break;

                case 28:
                        para[28] = "";
                        break;

                case 29:
                        para[29] = "";
                        break;

                case 30:
                        para[30] = "";
                        break;

                case 31:
                        para[31] = "";
                        break;

                case 32:
                        para[32] = "";
                        break;

                case 33:
                        para[33] = "";
                        break;

                case 34:
                        para[34] = "";
                        break;

                case 35:
                        para[35] = "";
                        break;

                case 36:
                        para[36] = "";
                        break;

                case 37:
                        para[37] = "";
                        break;

                case 38:
                        para[38] = "";
                        break;

                case 39:
                        para[39] = "";
                        break;

                case 40:
                        para[40] = "";
                        break;

                }

        //      printf("the swich finished!\n");

        }


/* 0 */         if( para[0]      != NULL ) { print_help(); }
/* 1 */         if( para[1]      != NULL ) { print_version(); }
/* 2 */         if( para[2]      != NULL ) { run_daemon(); }
/* 3 */         if( para[3]      != NULL ) { disk       = para[3]; }
/* 4 */         if( para[4]      != NULL ) { cpu_seq    = para[4];      collect_cpu(cpu_seq);   }
/* 5 */         if( para[5]      != NULL ) { interface  = para[5]; }
/* 6 */         if( para[6]      != NULL ) { loadlevel  = para[6];      cpu_load(loadlevel);    }
/* 7 */         if( para[7]      != NULL ) { processid  = para[7]; }
/* 8 */         if( para[8]      != NULL ) { collect_mem_proc( "mem_proc" ); }
/* 9 */         if( para[9]      != NULL ) { collect_mem_proc( "mem_used" ); }
/* 10 */        if( para[10]     != NULL ) { collect_mem_pri(); }
/* 11 */        if( para[11]     != NULL ) { collect_mem_vir(); }
/* 12 */        if( para[12]     != NULL ) { collect_mem_pri_ipcs(); }
/* 13 */        if( para[13]     != NULL ) { collect_mem_proc( "mswap_used" ); }
/* 14 */        if( para[14]     != NULL ) { collect_mem_proc( "mswap_total" ); }
/* 15 */        if( para[15]     != NULL ) { collect_disk_vmstat( "pgpgin"); }
/* 16 */        if( para[16]     != NULL ) { collect_disk_vmstat( "pgpgout"); }
/* 17 */        if( para[17]     != NULL ) { collect_disk_vmstat( "pswpin"); }
/* 18 */        if( para[18]     != NULL ) { collect_disk_vmstat( "pswpout"); }
/* 19 */        if( para[19]     != NULL ) { collect_diskstats( disk, "svctm" ); }
/* 20 */        if( para[20]     != NULL ) { collect_diskstats( disk, "await" ); }
/* 21 */        if( para[21]     != NULL ) { collect_diskstats( disk, "aveq" ); }
/* 22 */        if( para[22]     != NULL ) { collect_diskstats( disk, "avgrq_sz" ); }
/* 23 */        if( para[23]     != NULL ) { collect_diskstats( disk, "util" ); }
/* 24 */        if( para[24]     != NULL ) { collect_net_dev ( interface, "net_byte_in" ); }
/* 25 */        if( para[25]     != NULL ) { collect_net_dev ( interface, "net_byte_out" ); }
/* 26 */        if( para[26]     != NULL ) { collect_net_dev ( interface, "net_pack_in" ); }
/* 27 */        if( para[27]     != NULL ) { collect_net_dev ( interface, "net_pack_out" ); }
/* 28 */        if( para[28]     != NULL ) { collect_net_snmp( "passiveopens" ); }
/* 29 */        if( para[29]     != NULL ) { collect_net_snmp( "currestab" ); }
/* 30 */        if( para[30]     != NULL ) { collect_net_snmp( "indatagrams" ); }
/* 31 */        if( para[31]     != NULL ) { collect_net_snmp( "outdatagrams" ); }
/* 32 */        if( para[32]     != NULL ) { collect_hardinfo ( "hard_proc" ); }
/* 33 */        if( para[33]     != NULL ) { collect_hardinfo ( "hard_mem"  ); }
/* 34 */        if( para[34]     != NULL ) { collect_hardinfo ( "hard_temp" ); }
/* 35 */        if( para[35]     != NULL ) { collect_hardinfo ( "hard_disk" ); }
/* 36 */        if( para[36]     != NULL ) { collect_hardinfo ( "hard_pwr"  ); }
/* 37 */        if( para[37]     != NULL ) { collect_hardinfo ( "hard_batt" ); }
/* 38 */        if( para[38]     != NULL ) { collect_hardinfo ( "hard_nics" ); }
/* 39 */        if( para[39]     != NULL ) { collect_hardinfo ( "hard_fans" ); }

//      printf("the while loop finished\n");

        return 0;
}

