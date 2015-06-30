/**
 ****************************************************************************************
 *
 * @file uart.c
 *
 * @brief UART driver
 *
 * Copyright (C) RivieraWaves 2009-2012
 *
 * $Rev: 6880 $
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup UART
 * @{
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stddef.h>     // standard definition
#include "timer.h"      // timer definition
#include "user_uart.h"       // uart definition
#include "reg_uart.h"   // uart register
#include "hcic_eif.h"   // hci uart definition
#include "global_io.h"
//#include "uart.h"       // uart definition
#include "arch.h"

#include "rwble_config.h"
/*
 * DEFINES
 *****************************************************************************************
 */

/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */
/// INT_PRIO values
#if 0 //@WIK, commented because this is not in line with our IIR bitfields
enum UART_INTPRIO
{
    UART_INTPRIO_MODEMSTAT     = 0,
    UART_INTPRIO_ALLDATASENT   = 1,
    UART_INTPRIO_DATAAVAILABLE = 2,
    UART_INTPRIO_LINESTAT      = 3,
    UART_INTPRIO_TIMEOUT       = 6
};
#endif

//@WIK, added next enum
enum UART_ID
{
    MODEM_STAT         = 0,
    NO_INT_PEND        = 1,
    THR_EMPTY          = 2,
    RECEIVED_AVAILABLE = 4,    
    UART_TIMEOUT       = 12
};

/// RX_LVL values
enum UART_RXLVL
{
    UART_RXLVL_1,
    UART_RXLVL_4,
    UART_RXLVL_8,
    UART_RXLVL_14
};

/// WORD_LEN values
enum UART_WORDLEN
{
    UART_WORDLEN_5,
    UART_WORDLEN_6,
    UART_WORDLEN_7,
    UART_WORDLEN_8
};

/// FIFO_SZ values
enum UART_FIFOSIZE
{
    UART_FIFOSIZE_16,
    UART_FIFOSIZE_32,
    UART_FIFOSIZE_64,
    UART_FIFOSIZE_128
};

/*
 * STRUCT DEFINITIONS
 *****************************************************************************************
 */
/* TX and RX channel class holding data used for asynchronous read and write data
 * transactions
 */
/// UART TX RX Channel
struct user_uart_txrxchannel
{
    /// size
    uint32_t  size;
    /// buffer pointer
    uint8_t  *bufptr;
    /// call back function pointer
    void (*callback) (uint8_t);
};

/// UART environment structure
struct user_uart_env_tag
{
    /// tx channel
    struct user_uart_txrxchannel tx;
    /// rx channel
    struct user_uart_txrxchannel rx;
    /// error detect
    uint8_t errordetect;
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
/// uart environment structure
static struct user_uart_env_tag user_uart_env __attribute__((section("retention_mem_area0"),zero_init));
/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Serves the receive data available interrupt requests. It clears the requests and
 *        executes the callback function.
 *
 ****************************************************************************************
 */

static void user_uart_rec_data_avail_isr(void)
{
    void (*callback) (uint8_t) = NULL;

    while (uart_data_rdy_getf())
    {
        // Read the received in the FIFO
        *user_uart_env.rx.bufptr = uart_rxdata_getf();

        // Update RX parameters
        user_uart_env.rx.size--;
        user_uart_env.rx.bufptr++;

        // Check if all expected data have been received
        if (user_uart_env.rx.size == 0)
        {
            // Reset RX parameters
            user_uart_env.rx.bufptr = NULL;

            // Disable Rx interrupt
            uart_rec_data_avail_setf(0);  //=SetBits16(UART_IER_DLH_REG, ETBEI_dlh0, 0); 

            // Retrieve callback pointer
            callback = user_uart_env.rx.callback;

            if(callback != NULL)
            {
                // Clear callback pointer
                user_uart_env.rx.callback = NULL;

                // Call handler
                callback(UART_STATUS_OK);
            }
            else
            {
                ASSERT_ERR(0);
            }

            // Exit loop
            break;
        }
    }
}

/**
 ****************************************************************************************
 * @brief Serves the receive data error interrupt requests. It clears the requests and
 *        executes the callback function.
 *
 ****************************************************************************************
 */

static void user_uart_rec_error_isr(void)
{
    void (*callback) (uint8_t) = NULL;
    // Reset RX parameters
    user_uart_env.rx.size = 0;
    user_uart_env.rx.bufptr = NULL;

    // Disable RX interrupt
    SetBits16(UART_IER_DLH_REG, ERBFI_dlh0, 0); // user_uart_rec_data_avail_setf(0);

    // Reset RX FIFO
    // user_uart_rxfifo_res_setf(1); @WIK commented

    // Retrieve callback pointer
    callback = user_uart_env.rx.callback;

    if(callback != NULL)
    {
        // Clear callback pointer
        user_uart_env.rx.callback = NULL;

        // Call handler
        callback(UART_STATUS_ERROR);
    }
    else
    {
        ASSERT_ERR(0);
    }
}

/**
 ****************************************************************************************
 * @brief Serves the transmit data fill interrupt requests. It clears the requests and
 *        executes the callback function.
 *
 * The callback function is called as soon as the last byte of the provided data is
 * put into the FIFO. The interrupt is disabled at the same time.
 ****************************************************************************************
 */
static void user_uart_thr_empty_isr(void)
{
    void (*callback) (uint8_t) = NULL;
    // Fill TX FIFO until there is no more room inside it
    while (uart_txfifo_full_getf())
    {
        // Put a byte in the FIFO
        uart_txdata_setf(*user_uart_env.tx.bufptr);

        // Update TX parameters
        user_uart_env.tx.size--;
        user_uart_env.tx.bufptr++;

        if (user_uart_env.tx.size == 0)
        {
            // Reset TX parameters
            user_uart_env.tx.bufptr = NULL;

            // Disable TX interrupt
            uart_thr_empty_setf(0);

            // Retrieve callback pointer
            callback = user_uart_env.tx.callback;

            if(callback != NULL)
            {
                // Clear callback pointer
                user_uart_env.tx.callback = NULL;

                // Call handler
                callback(UART_STATUS_OK);
            }
            else
            {
                ASSERT_ERR(0);
            }

            // Exit loop
            break;
        }
    }
}

/**
 ****************************************************************************************
 * @brief Check if RX FIFO is empty.
 *
 * @return FIFO empty      false: not empty  / true: empty
 *****************************************************************************************
 */
static bool user_uart_is_rx_fifo_empty(void)
{
    return (uart_data_rdy_getf() == 0);
}


/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void user_uart_init_func(uint8_t baudr, uint8_t mode)
{
		uint8_t user_buf[9]="UART init";
		SetBits16(CLK_PER_REG, UART1_ENABLE, 1);
		#ifdef Peri_Init
			//SetBits16(CLK_PER_REG, UART1_ENABLE, 1);  	// enable  clock for UART 1

			SetWord16(UART_LCR_REG, 0x80); // set bit to access DLH and DLL register
			SetWord16(UART_IER_DLH_REG,(UART_BAUDRATE_115K2&0xFF>>8));//set high byte
			SetWord16(UART_RBR_THR_DLL_REG,UART_BAUDRATE_115K2&0xFF);//set low byte
			SetWord16(UART_LCR_REG,UART_DATALENGTH|UART_PARITY|UART_STOPBITS);
			SetBits16(UART_MCR_REG, UART_SIRE, 0);  // mode 0 for normal , 1 for IRDA
			SetWord16(UART_IIR_FCR_REG,1);  // enable fifo  
			SetBits16(UART_IER_DLH_REG,ERBFI_dlh0,0);  // IER access, disable interrupt for available data
			printf2_string(user_buf);
		#else
			//ENABLE FIFO, REGISTER FCR IF UART_LCR_REG.DLAB=0
			SetBits16(UART_LCR_REG, UART_DLAB, 0);
			 // XMIT FIFO RESET, RCVR FIFO RESET, FIFO ENABLED,
			SetWord16(UART_IIR_FCR_REG,0x87); //rcv int when rx fifo 1/2 full

			//DISABLE INTERRUPTS, REGISTER IER IF UART_LCR_REG.DLAB=0
			SetWord16(UART_IER_DLH_REG, 0);

			// ACCESS DIVISORLATCH REGISTER FOR BAUDRATE 115200, REGISTER UART_DLH/DLL_REG IF UART_LCR_REG.DLAB=1
			SetBits16(UART_LCR_REG, UART_DLAB, 1);
			SetWord16(UART_IER_DLH_REG,0); // for serial clk 16MHz baudrate 115200 
			//SetWord16(UART_RBR_THR_DLL_REG,9); // set baudrate ~115200  = serial_clk/(16*9)
			SetWord16(UART_IER_DLH_REG, (baudr&0xFF>>0x8));
			SetWord16(UART_RBR_THR_DLL_REG,baudr&0xFF); // set baudrate ~115200  = serial_clk/(16*9)

			// NO PARITY, 1 STOP BIT, 8 DATA LENGTH AND 
			//SetWord16(UART_LCR_REG,3); 
			SetWord16(UART_LCR_REG,mode);

			//ENABLE TX INTERRUPTS, REGISTER IER IF UART_LCR_REG.DLAB=0
			SetBits16(UART_LCR_REG, UART_DLAB, 0);
			//SetBits16(UART_IER_DLH_REG, ETBEI_dlh1, 1);
			
			NVIC_DisableIRQ(UART_IRQn);
			NVIC_SetPriority(UART_IRQn,1);  
			NVIC_EnableIRQ(UART_IRQn);
			NVIC_ClearPendingIRQ(UART_IRQn);		
			//   NVIC->ICPR[UART_IRQn]=1; //clear eventual pending bit, but not necessary becasuse this is already cleared automatically in HW

			// Configure UART environment
			user_uart_env.errordetect = UART_ERROR_DETECT_DISABLED;
			user_uart_env.rx.bufptr = NULL;
			user_uart_env.rx.size = 0;
			user_uart_env.tx.bufptr = NULL;
			user_uart_env.tx.size = 0;

	/////////////
	//    SetWord32(UART_MCR_REG, UART_AFCE|UART_RTS); //Enable RTS/CTS handshake
			
			user_uart_write(user_buf,9,NULL);
		#endif
}
//extern const uint32_t jump_table_struct[];
void user_uart_init(uint8_t baudr, uint8_t mode )
{
    
/*    
    typedef void (*my_function)( uint8_t, uint8_t);
    my_function PtrFunc;
    PtrFunc = (my_function)(jump_table_struct[uart_init_pos]);
    PtrFunc(baudr,mode);
*/
    user_uart_init_func(baudr, mode);
    
}

//baudr=9-> 115k2
//mode=3->  // no parity, 1 stop bit 8 data length


void user_uart_flow_on_func(void)
{
    // Configure modem (HW flow control enable)

#if 0 
   // Configure modem (HW flow control enable)
    user_uart_mcr_pack(UART_ENABLE,    // extfunc
                  UART_ENABLE,    // autorts
                  UART_ENABLE,    // autocts
                  UART_ENABLE);   // rts
#endif
    SetWord32(UART_MCR_REG, UART_AFCE|UART_RTS);
}

void user_uart_flow_on(void)
{
/*    
    typedef void (*my_function)( void);
    my_function PtrFunc;
    PtrFunc = (my_function)(jump_table_struct[uart_flow_on_pos]);
    PtrFunc();
*/
    user_uart_flow_on_func();
}

bool user_uart_flow_off_func(void)
{
    bool flow_off = true;
		volatile unsigned int i;

    do
    {
        // First check if no transmission is ongoing
        if ((uart_temt_getf() == 0) || (uart_thre_getf() == 0))
        {
            flow_off = false;
            break;
        }

        // Configure modem (HW flow control disable, 'RTS flow off')
#if 0
        user_uart_mcr_pack(UART_ENABLE,     // extfunc
                      UART_DISABLE,    // autorts
                      UART_ENABLE,     // autocts
                      UART_DISABLE);   // rts
#endif
        SetWord32(UART_MCR_REG, 0);
 
				// Wait for 1 character duration to ensure host has not started a transmission at the
        // same time
        for (i=0;i<350;i++);

        // Wait for 1 character duration to ensure host has not started a transmission at the
        // same time
        #ifndef __GNUC__
//        timer_wait(UART_CHAR_DURATION);
        #endif //__GNUC__

        // Check if data has been received during wait time
        if(!user_uart_is_rx_fifo_empty())
        {
            // Re-enable UART flow
            user_uart_flow_on();

            // We failed stopping the flow
            flow_off = false;
        }
    } while(false);

    return (flow_off);
}


void user_uart_flow_off(void)
{
/*    
    typedef bool (*my_function)( void);
    my_function PtrFunc;
    PtrFunc = (my_function)(jump_table_struct[uart_flow_off_pos]);
    return PtrFunc();
*/
    user_uart_flow_off_func();
}


void user_uart_finish_transfers_func(void)
{
    // Configure modem (HW flow control disable, 'RTS flow off')
    uart_mcr_pack(UART_ENABLE,     // extfunc
                  UART_DISABLE,    // autorts
                  UART_ENABLE,     // autocts
                  UART_DISABLE);   // rts

    // Wait TX FIFO empty
    while(!uart_thre_getf() || !uart_temt_getf());
}

void user_uart_finish_transfers(void)
{
/*
    typedef void (*my_function)( void);
    my_function PtrFunc;
    PtrFunc = (my_function)(jump_table_struct[uart_finish_transfers_pos]);
    PtrFunc();
*/
    user_uart_finish_transfers_func();
}



void user_uart_read_func(uint8_t *bufptr, uint32_t size,void (*callback) (uint8_t))
{
    // Sanity check
    ASSERT_ERR(bufptr != NULL);
    ASSERT_ERR(size != 0);
    ASSERT_ERR(user_uart_env.rx.bufptr == NULL);

    // Prepare RX parameters
    user_uart_env.rx.size = size;
    user_uart_env.rx.bufptr = bufptr;
		user_uart_env.rx.callback = callback;	

    // Start data transaction
    uart_rec_data_avail_setf(1); //=SetBits16(UART_IER_DLH_REG, ETBEI_dlh0, 1); 
}

void user_uart_read(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t))
//void user_uart_read(uint8_t *bufptr, uint32_t size)
{
/*    
    typedef void (*my_function)( uint8_t*, uint32_t, void (*callback) (uint8_t));
    my_function PtrFunc;
    PtrFunc = (my_function)(jump_table_struct[uart_read_pos]);
    PtrFunc(bufptr,size,callback);
*/
    user_uart_read_func(bufptr, size, callback);
}

void user_uart_write_func(uint8_t *bufptr, uint32_t size,void (*callback) (uint8_t))
{
    // Sanity check
    ASSERT_ERR(bufptr != NULL);
    ASSERT_ERR(size != 0);
    ASSERT_ERR(user_uart_env.tx.bufptr == NULL);

    // Prepare TX parameters
    user_uart_env.tx.size = size;
    user_uart_env.tx.bufptr = bufptr;
		user_uart_env.tx.callback = callback; 


    /* start data transaction
     * first isr execution is done without interrupt generation to reduce
     * interrupt load
     */
    user_uart_thr_empty_isr();
    if (user_uart_env.tx.bufptr != NULL)
    {
        uart_thr_empty_setf(1);
    }
}
void user_uart_write(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t))
//void user_uart_write(uint8_t *bufptr, uint32_t size)
{
  
	user_uart_write_func(bufptr, size, callback);
}
void user_uart_send_byte(char ch){
	while((GetWord16(UART_LSR_REG)&0x20)==0);	// read status reg to check if THR is empty
	SetWord16(UART_RBR_THR_DLL_REG,(0xFF&ch)); // write to THR register
	return;
}
void printf2_string(uint8_t * str)
{
	while(*str!=0){	      // while not reach the last string character
		user_uart_send_byte(*str); // send next string character
		str++;
	}
}

#ifdef __GNUC__
void __real_UART_Handler(void);
#else
void $Super$$UART_Handler(void);
#endif

#ifdef __GNUC__
void __wrap_UART_Handler(void)
#else
void $Sub$$UART_Handler(void)
#endif
{
    uint32_t idd;
        
    idd = 0x0F & GetWord32(UART_IIR_FCR_REG);
    if(idd!=NO_INT_PEND)
    {
        switch(idd)
        {
            case UART_TIMEOUT:
                if ((user_uart_env.errordetect == UART_ERROR_DETECT_ENABLED) && uart_fifo_err_getf())
                {
                    user_uart_rec_error_isr();
                }
                break;
            case RECEIVED_AVAILABLE:
                user_uart_rec_data_avail_isr();               
                break;
            case THR_EMPTY:
                user_uart_thr_empty_isr();
                break;
            default:
                break;
        }
    }
}



/// @} UART
