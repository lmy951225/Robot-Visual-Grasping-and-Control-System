// gcc  test_pdo.c -o test_pdo -lethercat

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <malloc.h>
#include <stdlib.h>

/****************************************************************************/

#include "ecrt.h"

/****************************************************************************/

// Application parameters
#define FREQUENCY 1000
#define CLOCK_TO_USE CLOCK_REALTIME

/****************************************************************************/

#define NSEC_PER_SEC (1000000000L)
#define PERIOD_NS (NSEC_PER_SEC / FREQUENCY)

#define DIFF_NS(A, B) (((B).tv_sec - (A).tv_sec) * NSEC_PER_SEC + \
                       (B).tv_nsec - (A).tv_nsec)

#define TIMESPEC2NS(T) ((uint64_t)(T).tv_sec * NSEC_PER_SEC + (T).tv_nsec)

/****************************************************************************/
static ec_master_t *master = NULL;
static ec_domain_t *domain = NULL;
static ec_slave_config_t *sc = NULL;

/****************************************************************************/

// process data
static uint8_t *domain_pd = NULL;

// signal to turn off servo on state
static unsigned int run = 1;
static int cur_position;
static int cur_status;
const struct timespec cycletime = {0, PERIOD_NS};

/*****************************************************************************/
#define SlavePos 0, 0
#define DELTAINFO 0x000000ab ,0x00001030  // vid pid

// RxPDO

unsigned int slave_6040_00; // Contorl word
unsigned int slave_607a_00; // Profile target position
unsigned int slave_60ff_00; // Target velocity
unsigned int slave_60b1_00; // Velocity offset
unsigned int slave_60b2_00; // Torque offset
unsigned int slave_6071_00; // Target torque
unsigned int slave_6060_00; // Operating mode

unsigned int slave_6041_00; // status word
unsigned int slave_6064_00; // Actual motor position
unsigned int slave_60f4_00; // Position loop error
unsigned int slave_606c_00; // Actual motor velocity
unsigned int slave_6077_00; // Torque actual value
unsigned int slave_6061_00; // Display operation mode


const static ec_pdo_entry_reg_t domain_regs[] = {
   // RxPDO5:0x1600
{SlavePos, DELTAINFO, 0x6040, 0, &slave_6040_00, NULL},
{SlavePos, DELTAINFO, 0x607A, 0, &slave_607a_00, NULL},
{SlavePos, DELTAINFO, 0x60FF, 0, &slave_60ff_00, NULL},
{SlavePos, DELTAINFO, 0x60B1, 0, &slave_60b1_00, NULL},
{SlavePos, DELTAINFO, 0x60B2, 0, &slave_60b2_00, NULL},
{SlavePos, DELTAINFO, 0x6071, 0, &slave_6071_00, NULL},
{SlavePos, DELTAINFO, 0x6060, 0, &slave_6060_00, NULL},

// TxPDO5:0x1a00
{SlavePos, DELTAINFO, 0x6041, 0, &slave_6041_00},
{SlavePos, DELTAINFO, 0x6064, 0, &slave_6064_00},
{SlavePos, DELTAINFO, 0x60F4, 0, &slave_60f4_00},
{SlavePos, DELTAINFO, 0x606C, 0, &slave_606c_00}, //=606C, !=606F
{SlavePos, DELTAINFO, 0x6077, 0, &slave_6077_00},
{SlavePos, DELTAINFO, 0x6061, 0, &slave_6061_00},

{}


    
};

static ec_pdo_entry_info_t DELTA_pdo_entries[] = {
   // RxPDO 0x1600
{0x6040, 0x00, 16},
{0x607A, 0x00, 32},
{0x60FF, 0x00, 32},
{0x60B1, 0x00, 32},
{0x60B2, 0x00, 16},
{0x6071, 0x00, 16},
{0x6060, 0x00, 8},

// TxPDO 0x1A00
{0x6041, 0x00, 16},
{0x6064, 0x00, 32},
{0x60F4, 0x00, 32},
{0x606C, 0x00, 32},
{0x6077, 0x00, 16},
{0x6061, 0x00, 8},
};

static ec_pdo_info_t DELTA_pdos[] = {
    // RxPdo
    //{0x1700, 4, DELTA_pdo_entries + 0},
    {0x1600, 7, DELTA_pdo_entries + 0},

    // TxPdo
    {0x1A00, 6, DELTA_pdo_entries + 7}};

static ec_sync_info_t DELTA_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 1, DELTA_pdos + 0 /*,EC_WD_ENABLE*/ /*, EC_WD_DISABLE*/},
    {3, EC_DIR_INPUT, 1, DELTA_pdos + 1 /*,EC_WD_DISABLE*/ /*, EC_WD_DISABLE*/},
    {0xFF}};

/*****************************************************************************/

struct timespec timespec_add(struct timespec time1, struct timespec time2)
{
    struct timespec result;

    if ((time1.tv_nsec + time2.tv_nsec) >= NSEC_PER_SEC)
    {
        result.tv_sec = time1.tv_sec + time2.tv_sec + 1;
        result.tv_nsec = time1.tv_nsec + time2.tv_nsec - NSEC_PER_SEC;
    }
    else
    {
        result.tv_sec = time1.tv_sec + time2.tv_sec;
        result.tv_nsec = time1.tv_nsec + time2.tv_nsec;
    }

    return result;
}

/*****************************************************************************/

void endsignal(int sig)
{
    run = 0;
    printf("EtherCAT stop.\n");
    signal(SIGINT, SIG_DFL);
}

/****************************************************************************/

void cyclic_task()
{

    struct timespec wakeupTime, time;
    int count = 5;

    while (count > 0)
    {
        clock_gettime(CLOCK_TO_USE, &wakeupTime);
        wakeupTime = timespec_add(wakeupTime, cycletime);
        clock_nanosleep(CLOCK_TO_USE, TIMER_ABSTIME, &wakeupTime, NULL);
        ecrt_master_application_time(master, TIMESPEC2NS(wakeupTime));

        ecrt_master_receive(master);
        ecrt_domain_process(domain);

        cur_status = EC_READ_U16(domain_pd + slave_6041_00);
        cur_position = EC_READ_U32(domain_pd + slave_6064_00);

//        EC_WRITE_U8(domain_pd + offset.modes_operation, 8);

        printf("status:{%04x}, position:{%d}\n",cur_status, cur_position);

        if ((cur_status & 0x004f) == 0x0040)
        {
            EC_WRITE_U16(domain_pd + slave_6040_00, 0x0006);
            printf("1\n");
        }
        else if ((cur_status & 0x006f) == 0x0021)
        {
            EC_WRITE_U16(domain_pd + slave_6040_00, 0x0007);

            printf("2\n");
        }
        else if ((cur_status & 0x06f) == 0x023)
        {
            EC_WRITE_S32(domain_pd + slave_607a_00, cur_position);
            EC_WRITE_U16(domain_pd + slave_6040_00, 0x000f);

            printf("3\n");
        }
        else if ((cur_status & 0x06f) == 0x027)
        {

            //EC_WRITE_S32(domain_pd + offset.target_position, cur_position + 100);
            EC_WRITE_S32(domain_pd + slave_607a_00, cur_position);
            EC_WRITE_U16(domain_pd + slave_6040_00, 0x001f);

            printf("4\n");
        }

        if ((cur_status == 0x1208))
        {
            EC_WRITE_U16(domain_pd + slave_6040_00, 0x0080);

            printf("5\n");
        }
        else if ((cur_status == 0x1288))
        {
            EC_WRITE_U16(domain_pd + slave_6040_00, 0x0080);

            printf("6\n");
        }
        else if ((cur_status == 0))
        {
            EC_WRITE_U16(domain_pd + slave_6040_00, 0x0006);

            printf("7\n");
        }

        /****************************************************/

        clock_gettime(CLOCK_TO_USE, &time);
        ecrt_master_application_time(master, TIMESPEC2NS(time));
        ecrt_master_sync_reference_clock(master);
        ecrt_master_sync_slave_clocks(master);

        ecrt_domain_queue(domain);
        ecrt_master_send(master);

	count--;
    }
}

/****************************************************************************/

int main(int argc, char **argv)
{
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1)
    {
        perror("mlockall failed");
        return -1;
    }

    master = ecrt_request_master(0);
    if (!master)
    {
	    printf("----ecrt_request_master failed!---\n");
        return -1;
    }
    domain = ecrt_master_create_domain(master);
    if (!domain)
    {
	    printf("----ecrt_request_master failed!---\n");
        return -1;
    }

    if (!(sc = ecrt_master_slave_config(master, SlavePos, DELTAINFO)))
    {
        fprintf(stderr, "Failed to get slave configuration.\n");
        return -1;
    }

    printf("Configuring PDOs...\n");

    if (ecrt_slave_config_pdos(sc, EC_END, DELTA_syncs))
    {
        fprintf(stderr, "Failed to configure PDOs.\n");
        return -1;
    }

    printf("Configuring PDOs entry...\n");
    if (ecrt_domain_reg_pdo_entry_list(domain, domain_regs))
    {
        fprintf(stderr, "Slave PDO entry registration failed!\n");
        return -1;
    }

    ecrt_slave_config_dc(sc, 0x0300, 1000000, 1000000 / 2, 0, 0);

    printf("Activating master...\n");

    if (ecrt_master_activate(master))
    {
	    printf("----ecrt_master_activate failed!---\n");
        return -1;
    }

    if (!(domain_pd = ecrt_domain_data(domain)))
    {
	    printf("----ecrt_domain_data() failed!---\n");
        return -1;
    }

    pid_t pid = getpid();
    if (setpriority(PRIO_PROCESS, pid, -20))
    {
        fprintf(stderr, "Warning: Failed to set priority: %s\n", strerror(errno));
    }

    signal(SIGINT, endsignal);
    printf("Starting cyclic function.\n");

	cyclic_task();

    ecrt_release_master(master);

    return 0;
}
