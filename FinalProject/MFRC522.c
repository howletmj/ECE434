/*
 * MFRC522(RFID -RC522 Module) IC library to use with BeagleBone Black
 * Protocol -> SPI (SPI0)
 *
 * Copyright (c) 2017  PeakUp
 * Copyright (c) 2017  Berkay Ozturk <berkay13ozturk@gmail.com>
 * Modified Matthew Howlett
 * Pin Configurations(for BeagleBone Black)
 *
 * RC522 -> BeagleBoneBlack
 * SDA      SPI0_CS0  (17)
 * SCK      SPI0_SCLK (22)
 * MOSI     SPI0_D1   (18)
 * MISO     SPI0_D0   (21)
 *
 * GND      DGND
 * 3.3V     VDD_3V3
 * RST	    VDD_3V3
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "RC522.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

typedef uint8_t bool;
#define true 1
#define false 0

static void pabort(const char *s)
{
        perror(s);
        abort();
}

static const char *device = "/dev/spidev2.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 10000;
static uint16_t delay;
int fd;
uint8_t CRC_A_Result[2];
uint8_t Rx_Global[100];
uint8_t UID_Value[5];
uint8_t UID_Old[5];
uint8_t SAK_Response[3];
uint8_t ATQA_Buffer[2];
uint8_t ISNewUID_Detected(void){
        if((UID_Old[0]!=UID_Value[0])&&
                (UID_Old[1]!=UID_Value[1])&&
                (UID_Old[2]!=UID_Value[2])&&
                (UID_Old[3]!=UID_Value[3])&&
                (UID_Old[4]!=UID_Value[4])){
	UID_Old[0]=UID_Value[0];
 UID_Old[0]=UID_Value[0];
 UID_Old[1]=UID_Value[1];
 UID_Old[2]=UID_Value[2];
 UID_Old[3]=UID_Value[3];
 UID_Old[4]=UID_Value[4];
        return 1;
}
        else return 0;
}

//Function Declerations
void MFRC522_Initialize(void);
void AntennaOn(void);
bool IsNewCardPresent(void);
uint8_t REQA_or_WUPA_Request(uint8_t Request);
uint8_t CommunicateWithPICC(uint8_t Command, uint8_t ValidBits, uint8_t InterruptRequestBits, uint8_t SendCount, uint8_t *SendValues,
                            uint8_t ReturnCount, uint8_t *ReturnValues);
uint8_t SelectPICC(void);
bool BCC_CheckOk(void);
bool CRC_A_Calculation(uint8_t *SendValues, uint8_t SendCount);
void DigitalSelfTestOperation (void);

void RegisterWrite(uint8_t RegisterAddress, uint8_t RegisterValue);
uint8_t RegisterRead(uint8_t RegisterAddress);
void SetRegisterBits(uint8_t RegisterAddress, uint8_t Mask);
void ClearRegisterBits(uint8_t RegisterAddress, uint8_t Mask);

void RegisterWriteMultiple(uint8_t RegisterAddress, uint8_t Count, uint8_t *RegisterValues);
void RegisterReadMultiple(uint8_t RegisterAddress, uint8_t Count);
//Function Declerations

/**
 * Use the CRC coprocessor in the MFRC522 to calculate a CRC_A.
 */
bool CRC_A_Calculation(uint8_t *SendValues, uint8_t SendCount)
{

        RegisterWrite(CommandReg, PCD_Idle);      // Stop any active command.
        RegisterWrite(DivIrqReg, 0x04);           // Clear the CRCIRq interrupt request bit
        SetRegisterBits(FIFOLevelReg, 0x80);      // FlushBuffer = 1, FIFO initialization
        RegisterWriteMultiple(FIFODataReg, SendCount, &SendValues[0]); // Write data to the FIFO
        RegisterWrite(CommandReg, PCD_CalcCRC);   // Start the calculation

        // Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73us.
        uint16_t index = 5000;
        uint8_t RegisterReturnValue;

        while (true)
        {
                RegisterReturnValue = RegisterRead(DivIrqReg);  // DivIrqReg[7..0] bits are: Set2 reserved reserved MfinActIRq   reserved CRCIRq reserved reserved
                if (RegisterReturnValue & 0x04)
                {
                        // CRCIRq bit set - calculation done
                        break;
                }

                if (--index == 0)
                {
                        // The emergency break. We will eventually terminate on this one after 89ms.
                        // Communication with the MFRC522 might be down.
                        return STATUS_Timeout;
                }
        }

        // Stop calculating CRC for new content in the FIFO.
        RegisterWrite(CommandReg, PCD_Idle);

        // Transfer the result from the registers to the result buffer
        CRC_A_Result[0] = RegisterRead(CRCResultRegL);
        CRC_A_Result[1] = RegisterRead(CRCResultRegH);

        return STATUS_Ok;

} // End PCD_CalculateCRC()

/*
* Checks if the received UID block is correct or not via BCC
*/
bool BCC_CheckOk(void)
{
/*
* Rx_Global[1] -> CT or UID(n)
* Rx_Global[2] -> UID(n+1)
* Rx_Global[3] -> UID(n+2)
* Rx_Global[4] -> UID(n+3)
*
* Rx_Global[5] -> BCC
*/
        if( Rx_Global[1] == (Rx_Global[5] ^ Rx_Global[4] ^ Rx_Global[3] ^ Rx_Global[2]) )
                return true; //All UID's received correctly
        else
                return false; //Receive error

}//End BCC_Check

/*
* Transmits SELECT/ANTICOLLISION commands to select a single PICC.
*/
uint8_t SelectPICC(void)
{

        //UDI Completeness Status
        bool UID_Complete = false;
        // Prepare MFRC522
        // ValuesAfterColl=1 => Bits received after collision are cleared.
        ClearRegisterBits(CollReg, 0x80);

        //Position of first collision
        uint8_t Coll;

        //Communicate via transceive
        uint8_t Command = PCD_Transceive;

        // RxIRq and IdleIRq
        uint8_t InterruptRequestBits = 0x30;

        //Whole byte will transmit
        uint8_t ValidBits = 0x00;

        //Figure 10 --> Anticollision loop, flowchart for PCD
        //Cascade Level 1
        uint8_t Sel = (PICC_CMD_SEL_Cl1 - 0x02); //1


        while( (!UID_Complete) )
        {

                Sel += 0x02; //Increase cascade level for every loop (CL1 -> CL2 -> CL3) //1
                if( (Sel > PICC_CMD_SEL_Cl3) || (Sel < PICC_CMD_SEL_Cl1) )
                        return STATUS_Error;

                uint8_t Nvb = 0x20;             //2

                uint8_t SendCount = 0x02; //SEL + NVB
                uint8_t ReturnCount = 0x05; //UID CLn

                uint8_t SendValues[9] = {Sel, Nvb};

                CommunicateWithPICC(Command, ValidBits, InterruptRequestBits, SendCount, &SendValues[0], ReturnCount, &UID_Value[0]); //3 (ANTICOLLISION)

                //Continue if there is no further collision

                // More than one PICC in the field => collision.
                //If there is a collision go one cascade level deeper, max 32 iterations
                uint8_t RegisterReturnValue = RegisterRead(CollReg);     //5
                uint8_t AnticollisionLoopIndex = 0; //Maximum of 32 loops

                //6 - 9
                //Loop until there is no collision detected
                while( !(RegisterReturnValue & 0x20) && (AnticollisionLoopIndex < 32) )
                {
                        //Collision Detected
                        RegisterReturnValue = RegisterRead(CollReg);

                        //6
                        //CollPos[4:0]
                        RegisterReturnValue = (RegisterReturnValue & 0x1F);
                        if(RegisterReturnValue == 0x00)
                                //Indicates a bit-collision in the 32nd bit
                                Coll = RegisterReturnValue;
                        else if(RegisterReturnValue == 0x01)
                                //Indicates a bit-collision in the 1st bit
                                Coll = RegisterReturnValue;
                        else if(RegisterReturnValue == 0x08)
                                //Indicates a bit-collision in the 8th bit
                                Coll = RegisterReturnValue;
                        else
                                //Not compliant with the standard
                                return STATUS_Error;

                        //7
                        Nvb = 0x20 + Coll;

                        SendCount = 0x07; //SEL + NVB + UID CLn (Anticollision Command)
                        ReturnCount = 0x05; //UID CLn

                        //Re-assign values for SELECT command
                        SendValues[0] = Sel;
                        SendValues[1] = Nvb;
                        SendValues[2] = UID_Value[0];
                        SendValues[3] = UID_Value[1];
                        SendValues[4] = UID_Value[2];
                        SendValues[5] = UID_Value[3];
                        SendValues[6] = UID_Value[4]; //BCC

                        CommunicateWithPICC(Command, ValidBits, InterruptRequestBits, SendCount, &SendValues[0], ReturnCount, &UID_Value[0]); //8 (ANTICOLLISION)

                        AnticollisionLoopIndex++; //Anticollision Loop Counter (max 32 iterations)

                }//AnticollisionLoop

                Nvb = 0x70;             //11

                //Re-assign values for SELECT command
                SendValues[0] = Sel;
                SendValues[1] = Nvb;
                SendValues[2] = UID_Value[0];
                SendValues[3] = UID_Value[1];
                SendValues[4] = UID_Value[2];
                SendValues[5] = UID_Value[3];
                SendValues[6] = UID_Value[4]; //BCC

                SendCount = 0x09;   //SEL + NVB + UID CLn + CRC_A (Select Command)
                ReturnCount = 0x03; //SAK

                //Append a CRC_A calculation for SELECT command
                RegisterReturnValue = CRC_A_Calculation(&SendValues[0], 7);
                if ( RegisterReturnValue == STATUS_Ok )
                {
                        //Send the transceive command if CRC_A check is successfull
                        SendValues[7] = CRC_A_Result[0];
                        SendValues[8] = CRC_A_Result[1];

                        //If CRC_A calculation is successfull, send the SELECT command
                        CommunicateWithPICC(Command, ValidBits, InterruptRequestBits, SendCount, &SendValues[0], ReturnCount, &SAK_Response[0]); //12 (SELECT)

                }


                //Calculate the CRC_A for return value
                RegisterReturnValue = CRC_A_Calculation(&SAK_Response[0], 1);

                //CRC Read Ok
                if ( (RegisterReturnValue == STATUS_Ok) && (CRC_A_Result[0] == SAK_Response[1]) && (CRC_A_Result[1] == SAK_Response[2]) )
                {

                        

                        if( (SAK_Response[0] & 0x04) )
                                printf("\n Debug: UID not complete..\n");

                        else if ( (SAK_Response[0] & 0x20) )
                        {
                                
                                UID_Complete = true; //UID complete, exit cascade level loop
                                break;
                        }

                        else if ( !(SAK_Response[0] & 0x24) )
                        {
                                
                                UID_Complete = true; //UID complete, exit cascade level loop
                                break;
                        }

                        else
                        {
                                printf("\n Debug: PCD behaving differently, it is not compliant with ISO/IEC 14443 standard..\n");
                                return STATUS_Error;
                        }
                }

        }//Cascade Level Loop (while)

        return STATUS_Ok;
}// End SelectPICC()


/*
 * Transfers data to the MFRC522 FIFO, executes a command, waits for completion and transfers data back from the FIFO.
 * CRC validation can only be done if backData and backLen are specified.
 */
uint8_t CommunicateWithPICC(uint8_t Command, uint8_t ValidBits, uint8_t InterruptRequestBits, uint8_t SendCount, uint8_t *SendValues,
                            uint8_t ReturnCount, uint8_t *ReturnValues)
{
/*
* @Parameters :
* Command -> Command to communicate with PICC
* ValidBits -> Defines the number of bits of the last byte that will be transmitted
* InterruptRequestBits -> To look which interrupt flag is set
* SendCount -> FIFO Data Register's byte count (Transmit)
* *SendValues -> FIFO Data Register's data array (Transmit)
* ReturnCount -> FIFO Data Register's byte count (Receive)
* Rx_Global[1] -> Rx_Global[ReturnCount] contains FIFO Buffer's values
*/

        uint8_t RegisterReturnValue, RegisterReturnValue2;
        uint8_t bitFraming = ValidBits; //To BitFramingReg

        RegisterWrite(CommandReg, PCD_Idle);            // Stop any active command.
        RegisterWrite(ComIrqReg, 0x7F);                 // Clear all seven interrupt request bits
        SetRegisterBits(FIFOLevelReg, 0x80);            // FlushBuffer = 1, FIFO initialization
        RegisterWriteMultiple(FIFODataReg, SendCount, &SendValues[0]);  // Write sendData to the FIFO
        RegisterWrite(BitFramingReg, bitFraming);       // Bit adjustments
        RegisterWrite(CommandReg, Command);             // Execute the command

        if (Command == PCD_Transceive)
        {

                SetRegisterBits(BitFramingReg, 0x80);      // StartSend=1, transmission of data starts

        }
        // Wait for the command to complete.
        // In PCD_Init() we set the TAuto flag in TModeReg. This means the timer automatically starts when the PCD stops transmitting.
        // Each iteration of the do-while-loop takes 17.86us.
        uint32_t index = 2000;
        while (true)
        {

                RegisterReturnValue = RegisterRead(ComIrqReg);  // ComIrqReg[7..0] bits are: Set1 TxIRq RxIRq IdleIRq   HiAlertIRq LoAlertIRq ErrIRq TimerIRq

                if (RegisterReturnValue & InterruptRequestBits)
                {
                        // One of the interrupts that signal success has been set.
                        break;
                }

                if (RegisterReturnValue & 0x01)
                {
                        // Timer interrupt - nothing received in 25ms
                        return STATUS_Timeout;
                }

                if (--index == 0)
                {
                        // The emergency break. If all other condions fail we will eventually terminate on this one after 35.7ms. Communication with the MFRC522 might be down.
                        return STATUS_Timeout;
                }
        }


        // Stop now if any errors except collisions were detected.
        RegisterReturnValue = RegisterRead(ErrorReg); // ErrorReg[7..0] bits are: WrErr TempErr reserved BufferOvfl   CollErr CRCErr ParityErr ProtocolErr
        if (RegisterReturnValue & 0x13)
        {
                // BufferOvfl ParityErr ProtocolErr
                return STATUS_Error;
        }


        //If the RxModeReg register's RxNoErr bit is set to logic 1, the RxIRq bit is only set to logic 1 when data bytes are available in the FIFO
        RegisterReturnValue = RegisterRead(RxModeReg);
        RegisterReturnValue2 = RegisterRead(ComIrqReg);

        if( (!(RegisterReturnValue & 0x08)) && (RegisterReturnValue2 & 0x20) )
        {

                //Remember that PICC sends the data LSB first !
                RegisterReadMultiple(FIFODataReg, ReturnCount);

                //Check for collision
                RegisterReturnValue = RegisterRead(ErrorReg);
        }


        for(index = 1; index <= ReturnCount; index ++)
                ReturnValues[index-1] = Rx_Global[index];

        return STATUS_Ok;
}

/*
* Transmits REQA or WUPA commands
*/
uint8_t REQA_or_WUPA_Request(uint8_t Request)
{

        // ValuesAfterColl=1 => Bits received after collision are cleared.
        ClearRegisterBits(CollReg, 0x80);

        // For REQA and WUPA we need the short frame format
        // - transmit only 7 bits of the last (and only) byte. TxLastBits = BitFramingReg[2..0]
        uint8_t ValidBits = 7;                  //Short Frame Format
        uint8_t InterruptRequestBits = 0x30;    // RxIRq and IdleIRq

        uint8_t SendCount = 0x01;               //1 Byte
        uint8_t SendValues[1] = {Request};      //REQA command to FIFO buffer to send it to the PICC via antenna

        uint8_t ReturnCount = 0x02;               //2 Byte (ATQA)

        uint8_t ReturnValue = CommunicateWithPICC(PCD_Transceive, ValidBits, InterruptRequestBits, SendCount, &SendValues[0], ReturnCount, &ATQA_Buffer[0]);

        return ReturnValue;
}

/*
* Performs a digital self test
*/
void DigitalSelfTestOperation (void)
{

        int index;
        uint8_t Empty_Array[25] = {0};

        RegisterWrite(CommandReg, 0x0F);                        //Perform a soft reset

        RegisterWriteMultiple(FIFODataReg, 25, Empty_Array);    //Clear the internal buffer
        RegisterWrite(CommandReg, 0x01);

        RegisterWrite(AutoTestReg, 0x09);                       //Enable the self test
        RegisterWrite(FIFODataReg, 0x00);                       //Write 00h to the FIFO buffer
        RegisterWrite(CommandReg, 0x03);                        //Start the self test with the CalcCRC command

        RegisterReadMultiple(FIFODataReg, 64);                  //Read the results from the internal FIFO buffer


        //Debug Information
        printf("\n\n ___Digital Self Test___\n\n");
        for(index = 1; index < 65; index++)
        {

                printf("Debug: FIFODataRegister[%d] = %.2X h \n", index, Rx_Global[index]);
                if( !(index % 8) )
                        printf("\n");
        }
        printf("\n\n ___Digital Self Test___\n\n");

} //End DigitalSelfTestOperation()

/*
* Read multiple bytes from the internal FIFO buffer of MFRC522 IC
*/
void RegisterReadMultiple(uint8_t RegisterAddress, uint8_t Count)
{

        if (Count == 0) { return; }

        int index;
        int ret;
        uint8_t tx_temp[Count + 1];

        /*Dynamic memory allocation for RX buffer depends on the TX size*/
        uint8_t *rx = (uint8_t *) malloc(sizeof(uint8_t) * sizeof(tx_temp) );

        for(index = 0; index < Count; index++)
        {

          tx_temp[index] = ((RegisterAddress << 1) | 0x80); //Read Address

        }

          tx_temp[Count] = 0x00; //Last byte is always 0x00 for read operation

        /*SPI Configurations for GPIO*/
        struct spi_ioc_transfer tr={
               .tx_buf = (unsigned long)tx_temp,
               .rx_buf = (unsigned long)rx,
               .len = sizeof(tx_temp),
               .delay_usecs = delay,
               .speed_hz = speed,
               .bits_per_word = bits};

        /*Send the data*/
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
                pabort("Can't send spi message");
        /*Else, SPI Rx is available from now on*/

        for(index = 1; index < (Count + 1); index++)
        {

          Rx_Global[index] = rx[index];

        }

        free(rx);

} // End RegisterReadMultiple()

/*
* Write multiple bytes to the internal FIFO buffer of MFRC522 IC
*/
void RegisterWriteMultiple(uint8_t RegisterAddress, uint8_t Count, uint8_t *RegisterValues)
{

        uint8_t index;
        int ret;
        uint8_t tx_temp[Count+1];

        //Address information for write operation
        // MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
        tx_temp[0] = (RegisterAddress  << 1);

        for (index = 0; index < Count; index++)
        {

                tx_temp[index+1] = RegisterValues[index];

        }

        /*Dynamic memory allocation for RX buffer depends on the TX size*/
        uint8_t *rx = (uint8_t *) malloc(sizeof(uint8_t) * sizeof(tx_temp) );

        /*SPI Configurations for GPIO*/
        struct spi_ioc_transfer tr={
               .tx_buf = (unsigned long)tx_temp,
               .rx_buf = (unsigned long)rx,
               .len = sizeof(tx_temp),
               .delay_usecs = delay,
               .speed_hz = speed,
               .bits_per_word = bits};


        /*Send the data*/
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
                pabort("Can't send spi message");
        /*Else, SPI Rx is available from now on*/

        free(rx);

} // End PCD_WriteRegister()

/*
 * Returns true if a PICC responds to PICC_CMD_REQA.
 * Only "new" cards in state IDLE are invited. Sleeping cards in state HALT are ignored.
 */
bool IsNewCardPresent(void)
{

  uint8_t Result = REQA_or_WUPA_Request(PICC_CMD_Reqa); //Send a REQA Request

  return ((Result == STATUS_Ok) || (Result == STATUS_Collision) );

} // End IsNewCardPresent()

/**
 * Turns the antenna on by enabling pins TX1 and TX2.
 * After a reset these pins disabled.
 */
void AntennaOn(void)
{

  uint8_t ReadRegisterReturnValue = RegisterRead(TxControlReg);

  if ((ReadRegisterReturnValue & 0x03) != 0x03)
  {

        RegisterWrite(TxControlReg, (ReadRegisterReturnValue | 0x03) );

  }

} // End AntennaOn()

/*
* Initialize the MFRC522 IC
*/
void MFRC522_Initialize(void)
{

  // When communicating with a PICC we need a timeout if something goes wrong.
  // f_timer = 13.56 MHz / (2*TPreScaler+1) where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
  // TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.

  RegisterWrite(CommandReg, 0x0F);    //Perform a soft reset
  RegisterWrite(TModeReg, 0x80);      // TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
  RegisterWrite(TPrescalerReg, 0xA9); // TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25us.
  RegisterWrite(TReloadRegH, 0x03);   // Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
  RegisterWrite(TReloadRegL, 0xE8);

  RegisterWrite(TxASKReg, 0x40);      // Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
  RegisterWrite(ModeReg, 0x3D);       // Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)

  RegisterWrite(RFCfgReg, (0x07<<4)); // Set Rx Gain to max

  AntennaOn();                        // Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)

} // End MFRC522_Initialize()

/*
* Clear the given bits in RegisterAddress with Mask variable
*/
void ClearRegisterBits(uint8_t RegisterAddress, uint8_t Mask)
{

        uint8_t ReadRegisterReturnValue = RegisterRead(RegisterAddress);
        RegisterWrite(RegisterAddress, ((~Mask) & ReadRegisterReturnValue) );

}

/*
* Set the given bits in RegisterAddress with Mask variable
*/

void SetRegisterBits(uint8_t RegisterAddress, uint8_t Mask)
{

        uint8_t ReadRegisterReturnValue =  RegisterRead(RegisterAddress);
        RegisterWrite(RegisterAddress, (Mask | ReadRegisterReturnValue) );

}

/*
* Write 1 Byte RegisterValue value to RegisterAddress
*/
void RegisterWrite(uint8_t RegisterAddress, uint8_t RegisterValue)
{

        int ret;
        uint8_t tx_temp[2] = { (RegisterAddress << 1), ( RegisterValue ) };

        /*Dynamic memory allocation for RX buffer depends on the TX size*/
        uint8_t *rx = (uint8_t *) malloc(sizeof(uint8_t) * 2);

        /*SPI Configurations for GPIO*/
 struct spi_ioc_transfer tr={
               .tx_buf = (unsigned long)tx_temp,
               .rx_buf = (unsigned long)rx,
               .len = sizeof(tx_temp),
               .delay_usecs = delay,
               .speed_hz = speed,
               .bits_per_word = bits};


        /*Send the data*/
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
                pabort("Can't send spi message");
        /*Else, SPI Rx is available from now on*/

        free(rx);

}

/*
* Read 1 Byte RegisterValue value to ReadRegisterReturnValue
*/
uint8_t RegisterRead(uint8_t RegisterAddress)
{

        int ret;
        uint8_t tx_temp[2] = { ((RegisterAddress << 1) | 0x80) , 0x00 };

        /*Dynamic memory allocation for RX buffer depends on the TX size*/
        uint8_t *rx = (uint8_t *) malloc(sizeof(uint8_t) * 2);

        /*SPI Configurations for GPIO*/
 struct spi_ioc_transfer tr={
               .tx_buf = (unsigned long)tx_temp,
               .rx_buf = (unsigned long)rx,
               .len = sizeof(tx_temp),
               .delay_usecs = delay,
               .speed_hz = speed,
               .bits_per_word = bits};


        /*Send the data*/
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
                pabort("Can't send spi message");
        /*Else, SPI Rx is available from now on*/

        //Return Value
        uint8_t ReadRegisterReturnValue  = rx[1];

        free(rx);

        return ReadRegisterReturnValue;
}


static void print_usage(const char *prog)
{
        printf("Usage: %s [-DsbdlHOLC3]\n", prog);
        puts("  -D --device   device to use (default /dev/spidev1.0)\n"
             "  -s --speed    max speed (Hz)\n"
             "  -d --delay    delay (usec)\n"
             "  -b --bpw      bits per word \n"
             "  -l --loop     loopback\n"
             "  -H --cpha     clock phase\n"
             "  -O --cpol     clock polarity\n"
             "  -L --lsb      least significant bit first\n"
             "  -C --cs-high  chip select active high\n"
             "  -3 --3wire    SI/SO signals shared\n");
        exit(1);
}

static void parse_opts(int argc, char *argv[])
{
        while (1) {
                static const struct option lopts[] = {
                        { "device",  1, 0, 'D' },
                        { "speed",   1, 0, 's' },
                        { "delay",   1, 0, 'd' },
                        { "bpw",     1, 0, 'b' },
                        { "loop",    0, 0, 'l' },
                        { "cpha",    0, 0, 'H' },
                        { "cpol",    0, 0, 'O' },
                        { "lsb",     0, 0, 'L' },
                        { "cs-high", 0, 0, 'C' },
                        { "3wire",   0, 0, '3' },
                        { "no-cs",   0, 0, 'N' },
                        { "ready",   0, 0, 'R' },
                        { NULL, 0, 0, 0 },
                };
                int c;

                c = getopt_long(argc, argv, "D:s:d:b:lHOLC3NR", lopts, NULL);

                if (c == -1)
                        break;

                switch (c) {
                case 'D':
                        device = optarg;
                        break;
                case 's':
                        speed = atoi(optarg);
                        break;
                case 'd':
                        delay = atoi(optarg);
                        break;
                case 'b':
                        bits = atoi(optarg);
                        break;
                case 'l':
                        mode |= SPI_LOOP;
                        break;
                case 'H':
                        mode |= SPI_CPHA;
                        break;
                case 'O':
                        mode |= SPI_CPOL;
                        break;
                case 'L':
                        mode |= SPI_LSB_FIRST;
                        break;
                case 'C':
                        mode |= SPI_CS_HIGH;
                        break;
                case '3':
                        mode |= SPI_3WIRE;
                        break;
                case 'N':
                        mode |= SPI_NO_CS;
                        break;
                case 'R':
                        mode |= SPI_READY;
                        break;
                default:
                        print_usage(argv[0]);
                        break;
                }
        }
}

int main(int argc, char *argv[])
{

        int ret = 0;
//      int temp = 0;
        parse_opts(argc, argv);

        fd = open(device, O_RDWR);
        if (fd < 0)
                pabort("can't open device");

        /*
         * spi mode
         */
        ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
        if (ret == -1)
                pabort("can't set spi mode");

        ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
        if (ret == -1)
                pabort("can't get spi mode");

        /*
         * bits per word
         */
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
        if (ret == -1)
                pabort("can't set bits per word");

        ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
        if (ret == -1)
                pabort("can't get bits per word");

        /*
         * max speed hz
         */
        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        if (ret == -1)
                pabort("can't set max speed hz");

        ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
        if (ret == -1)
                pabort("can't get max speed hz");


        //Initialize the MFRC522
        MFRC522_Initialize();
	printf("Initializing Done\n");
        while(true)
        {
               if( IsNewCardPresent()){
			SelectPICC();
			if(ISNewUID_Detected()==1){
			printf("New Card \n");
			printf("UID:");
			for(char i =0;i<5;i++){
				printf("%X",UID_Value[i]);
			}//convert -size 320x100 xc:lightblue  -font  -pointsize 72           -fill blue  -draw "text 25,65 'Anthony'" text_draw.gif
			printf("\n");
			char str [150];
			
			
			sprintf(str,"convert -size 320x240 xc:lightblue -fill blue -font Times-Roman -pointsize 32 -draw \"text 0,100 'New UID:%x%x%x%x%x'\" /tmp/frame.png",UID_Value[0],UID_Value[1],UID_Value[2],UID_Value[3],UID_Value[4]);
			usleep(10000);
			system(str);
                        usleep(10000);
			system("sudo fbi -noverbose -T 1 /tmp/frame.png");
			char str2 [50];
			sprintf(str2,"./logidtosheets.py %X%X%X%X%X 0",UID_Value[0],UID_Value[1],UID_Value[2],UID_Value[3],UID_Value[4]);
			system(str2);
			char str3 [50];
			usleep(1000000);
			sprintf(str3,"./BellPWM.py %X%X%X%X%X",UID_Value[0],UID_Value[1],UID_Value[2],UID_Value[3],UID_Value[4]);
			system(str3);
}
}
                
        }

        close(fd); //Close File

        return ret;
}
