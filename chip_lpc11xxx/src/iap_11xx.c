/*****************************************************************************
 * $Id$
 *
 * Project: 	NXP LPC11U6x In Application Programming
 *
 * Description: Provides access to In-Application Programming (IAP) routines
 * 			    contained within the bootROM sector of LPC11U6x devices.
 *
 * Copyright(C) 2010, NXP Semiconductor
 * All rights reserved.
 *
 *****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 *****************************************************************************/
#include "chip.h"

/* IAP Command Definitions */
#define	IAP_CMD_PREPARE_SECTORS			  50
#define	IAP_CMD_COPY_RAM_TO_FLASH		  51
#define	IAP_CMD_ERASE_SECTORS			  52
#define	IAP_CMD_BLANK_CHECK_SECTORS		  53
#define	IAP_CMD_READ_PART_ID			  54
#define	IAP_CMD_READ_BOOT_ROM_VERSION	  55
#define	IAP_CMD_COMPARE					  56
#define	IAP_CMD_REINVOKE_ISP			  57
#define IAP_CMD_READ_UID              	  58

#define IAP_CMD_ERASE_PAGE                59    //new

/* IAP boot ROM location and access function */
#define IAP_ROM_LOCATION				0x1FFF1FF1UL

/*****************************************************************************
** Function name:	Chip_IAP_PrepareSectors
**
** Description:		Prepares sector(s) for erasing or write operations. This
** 								command must be executed before executing the "Copy RAM to
** 								Flash" or "Erase Sector(s)" commands.
**
** Parameters:		startSector - Number of first sector to prepare.
** 								endSector - Number of last sector to prepare.
**
** Returned value:	Status code returned by IAP ROM function.
**
******************************************************************************/
uint32_t Chip_IAP_PrepareSectors(uint32_t startSector, uint32_t endSector)
{
	uint32_t status;
	uint32_t result[3];
	uint32_t command[5];

	if (endSector < startSector)
	{
		status = IAP_STA_INVALD_PARAM;
	}
	else
	{
		command[0] = IAP_CMD_PREPARE_SECTORS;
		command[1] = startSector;
		command[2] = endSector;
		__disable_irq();
		iap_entry(command, result);
		__enable_irq();
		status = result[0];
	}
	return status;
}

/*****************************************************************************
** Function name:	Chip_IAP_CopyRAMToFlash
**
** Description:		Program the flash memory with data stored in RAM.
**
** Parameters:	   	dstAddr - Destination Flash address, should be a 256
**                               byte boundary.
**			 		srcAddr - Source RAM address, should be a word boundary
**			 		len     - Number of 8-bit bytes to write, must be a
**			 					 multiple of 256.
*
** Returned value:	Status code returned by IAP ROM function.
**
******************************************************************************/
uint32_t Chip_IAP_CopyRAMToFlash(uint32_t dstAddr, uint32_t srcAddr, uint32_t len)
{
	uint32_t result[3];
	uint32_t command[5];

	command[0] = IAP_CMD_COPY_RAM_TO_FLASH;
	command[1] = dstAddr;
	command[2] = srcAddr;
	command[3] = len;
	command[4] = SystemCoreClock / 1000UL;	/* Core clock frequency in kHz */

	iap_entry(command, result);

	return result[0];
}

/*****************************************************************************
** Function name:	Chip_IAP_EraseSectors
**
** Description:		Erase a sector or multiple sectors of on-chip Flash memory.
**
** Parameters:		startSector - Number of first sector to erase.
** 					endSector - Number of last sector to erase.
*
** Returned value:	Status code returned by IAP ROM function.
**
******************************************************************************/
uint32_t Chip_IAP_EraseSectors(uint32_t startSector, uint32_t endSector)
{
	uint32_t status;
	uint32_t result[3];
	uint32_t command[5];

	if (endSector < startSector)
	{
		status = IAP_STA_INVALD_PARAM;
	}
	else
	{
		command[0] = IAP_CMD_ERASE_SECTORS;
		command[1] = startSector;
		command[2] = endSector;
		command[3] = SystemCoreClock / 1000UL;	/* Core clock frequency in kHz */

		iap_entry(command, result);

		status = result[0];
	}
	return status;
}


/*****************************************************************************
** Function name:	Chip_IAP_BlankCheckSectors
**
** Description:		Blank check a sector or multiple sectors of on-chip flash
** 					memory.
**
** Parameters:		startSector - Number of first sector to check.
** 					endSector - Number of last sector to check.
** 					result[0] - Offset of the first non blank word location
**                  if the Status Code is IAP_STA_SECTOR_NOT_BLANK.
** 					result[1] - Contents of non blank word location.
**
** Returned value:	Status code returned by IAP ROM function.
**
******************************************************************************/
uint32_t Chip_IAP_BlankCheckSectors(uint32_t startSector, uint32_t endSector, uint32_t *result)
{
	uint32_t status;
	uint32_t iresult[3];
	uint32_t command[5];

	if (endSector < startSector)
	{
		status = IAP_STA_INVALD_PARAM;
	}
	else
	{
		command[0] = IAP_CMD_BLANK_CHECK_SECTORS;
		command[1] = startSector;
		command[2] = endSector;

		iap_entry(command, iresult);

		if (iresult[0] == IAP_STA_SECTOR_NOT_BLANK)
		{
			*result       = iresult[0];
			*(result + 1) = iresult[1];
		}
		status = iresult[0];
	}
	return status;
}

/*****************************************************************************
** Function name:	Chip_IAP_ReadPartID
**
** Description:		Read the part identification number.
**
** Parameters:		partID - Pointer to storage for part ID number.
*
** Returned value:	Status code returned by IAP ROM function.
**
******************************************************************************/
uint32_t Chip_IAP_ReadPartID(uint32_t *partID)
{
	uint32_t result[3];
	uint32_t command[5];

	command[0] = IAP_CMD_READ_PART_ID;
	__disable_irq();
	iap_entry(command, result);
	__enable_irq();
	*partID = result[1];

	return result[0];
}

/*****************************************************************************
** Function name:	Chip_IAP_ReadBootVersion
**
** Description:		Read the boot code version number.
**
** Parameters:		major - Major version number in ASCII format.
** 					minor - Minor version number in ASCII format.
**
** Returned value:	Status code returned by IAP ROM function.
**
******************************************************************************/
uint32_t Chip_IAP_ReadBootVersion(uint32_t *major, uint32_t *minor)
//uint32_t Chip_IAP_ReadBootVersion(uint32_t *major)
{
	uint32_t result[3];
	uint32_t command[5];

	command[0] = IAP_CMD_READ_BOOT_ROM_VERSION;

	iap_entry(command, result);


	*major = (result[1] & 0x0000FF00UL) >> 8;
	*minor = result[1] & 0x000000FFUL;

	return result[0];
}

/*****************************************************************************
** Function name:	Chip_IAP_Compare
**
** Description:		Compares the memory contents at two locations.
**
** Parameters:		len - Number of bytes to compare, must be a multiple of 4.
**					offset - Offset of the first mismatch if the Status Code is COMPARE_ERROR
**
** Returned value:	Status code returned by IAP ROM function.
**
******************************************************************************/
uint32_t Chip_IAP_Compare(uint32_t dstAddr, uint32_t srcAddr, uint32_t len, uint32_t *offset)
{
	uint32_t result[3];
	uint32_t command[5];

	command[0] = IAP_CMD_COMPARE;
	command[1] = dstAddr;
	command[2] = srcAddr;
	command[3] = len;

	iap_entry(command, result);

	if (result[0] == IAP_STA_COMPARE_ERROR)
	{
		if (offset != 0)
		{
			*offset = result[1];
		}
	}
	return result[0];
}

/*****************************************************************************
** Function name:	Chip_IAP_ReinvokeISP
**
** Description:		Invoke the bootloader in ISP mode.
**
** Parameters:		None.
*
** Returned value:	None.
**
******************************************************************************/
void Chip_IAP_ReinvokeISP(void)
{
	uint32_t result[3];
	uint32_t command[5];

	command[0] = IAP_CMD_REINVOKE_ISP;

	iap_entry(command, result);
}

// read UID
uint32_t Chip_IAP_ReadUID(uint32_t * UID)
{
	uint32_t result[5];
	uint32_t command[5];

	command[0] = IAP_CMD_READ_UID;

	iap_entry(command, result);
//	*UID++ =  result[1];
//	*UID++ =  result[2];
//	*UID++ =  result[3];
//	*UID =  result[4];

	*UID =  result[1];
	*UID++ =  result[2];
	*UID++ =  result[3];
	*UID++ =  result[4];

	return result[0];

}

//IAP erase	Page  256B	 64K have 0-255 pages, page0-15 in sector 0,	32K have 0-127 pages, 128k have 0-511 pages,
uint32_t Chip_IAP_ErasePage(uint32_t startPage, uint32_t endPage)
{
	uint32_t status;
	uint32_t result[3];
	uint32_t command[5];

	if (endPage < startPage)
	{
		status = IAP_STA_INVALD_PARAM;
	}
	else
	{
		command[0] = IAP_CMD_ERASE_PAGE;
		command[1] = startPage;
		command[2] = endPage;
		command[3] = SystemCoreClock / 1000UL;	/* Core clock frequency in kHz */

		iap_entry(command, result);

		status = result[0];
	}
	return status;
}
