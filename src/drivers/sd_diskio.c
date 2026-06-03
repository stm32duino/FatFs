/**
  ******************************************************************************
  * @file    sd_diskio.c
  * @author  MCD Application Team
  * @brief   SD Disk I/O driver based on BSP v1 api.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the st_license.txt
  * file in the root directory of this software component.
  * If no st_license.txt file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "sd_diskio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* use the default SD timout as defined in the platform BSP driver*/
#if defined(SDMMC_DATATIMEOUT)
#define SD_TIMEOUT SDMMC_DATATIMEOUT
#elif defined(SD_DATATIMEOUT)
#define SD_TIMEOUT SD_DATATIMEOUT
#else
#define SD_TIMEOUT 30 * 1000
#endif

#define SD_DEFAULT_BLOCK_SIZE 512

/*
 * Depending on the usecase, the SD card initialization could be done at the
 * application level, if it is the case define the flag below to disable
 * the BSP_SD_Init() call in the SD_Initialize().
 */

/* #define DISABLE_SD_INIT */

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/* Private function prototypes -----------------------------------------------*/
static DSTATUS SD_check_status (BYTE);
static DSTATUS SD_initialize (BYTE);
static DSTATUS SD_status (BYTE);
static DRESULT SD_read (BYTE, BYTE*, LBA_t, UINT);
static DRESULT SD_write (BYTE, const BYTE*, LBA_t, UINT);
static DRESULT SD_ioctl (BYTE, BYTE, void*);

const Diskio_drvTypeDef  SD_Driver =
{
  SD_initialize,
  SD_status,
  SD_read,
  SD_write,
  SD_ioctl,
};

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Check the status of the sd card
 * @param  lun : not used
 * @retval DSTATUS: return 0 if the sd card is ready and 1 otherwise
 */
static DSTATUS SD_check_status(BYTE lun)
{
  (void)lun;
  Stat = STA_NOINIT;

  if(BSP_SD_GetCardState() == MSD_OK)
  {
    Stat &= ~STA_NOINIT;
  }

  return Stat;
}

/**
 * @brief  Initialize the SD Diskio lowlevel driver
 * @param  lun : not used
 * @retval DSTATUS: return 0 on Success STA_NOINIT otherwise
 */
static DSTATUS SD_initialize(BYTE lun)
{
  Stat = STA_NOINIT;
#if !defined(DISABLE_SD_INIT)
  if(BSP_SD_Init() == MSD_OK)
  {
    Stat = SD_check_status(lun);
  }
#else
  Stat = SD_check_status(lun);
#endif
  return Stat;
}

/**
 * @brief  Get the Disk Status
 * @param  lun : not used
 * @retval DSTATUS: Operation status
 */
static DSTATUS SD_status(BYTE lun)
{
  return SD_check_status(lun);
}

/**
 * @brief  Reads Sector(s)
 * @param  lun : not used
 * @param  *buff: Data buffer to store read data
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to read (1..128)
 * @retval DRESULT: return RES_OK otherwise
 */
static DRESULT SD_read(BYTE lun, BYTE *buff, LBA_t sector, UINT count)
{
  (void)lun;
  DRESULT res = RES_ERROR;

  if(BSP_SD_ReadBlocks((uint32_t*)buff,
                       (uint32_t) (sector),
                       count, SD_TIMEOUT) == MSD_OK)
  {
    /* Wait until the card state is ready */
    while(BSP_SD_GetCardState()!= MSD_OK)
    {
    }
    res = RES_OK;
  }
  return res;
}

/**
 * @brief  Write data from sd card into a buffer
 * @param  lun : not used
 * @param  *buff: Data to be written
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to write (1..128)
 * @retval DRESULT: return RES_OK otherwise
 */
static DRESULT SD_write(BYTE lun, const BYTE *buff, LBA_t sector, UINT count)
{
  (void)lun;
  DRESULT res = RES_ERROR;

  if(BSP_SD_WriteBlocks((uint32_t*)buff,
                        (uint32_t)(sector),
                        count, SD_TIMEOUT) == MSD_OK)
  {
    /* Wait until the card state is ready */
    while(BSP_SD_GetCardState() != MSD_OK)
    {
    }
    res = RES_OK;
  }
  return res;
}

/**
 * @brief  I/O control operation
 * @param  lun : not used
 * @param  cmd: Control code
 * @param  *buff: Buffer to send/receive control data
 * @retval DRESULT: return RES_OK otherwise
 */
static DRESULT SD_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  (void)lun;
  DRESULT res = RES_ERROR;
  BSP_SD_CardInfo CardInfo;

  if (Stat & STA_NOINIT) return RES_NOTRDY;

  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC:
    res = RES_OK;
    break;

  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
    BSP_SD_GetCardInfo(&CardInfo);
    *(DWORD*)buff = CardInfo.LogBlockNbr;
    res = RES_OK;
    break;

  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
    BSP_SD_GetCardInfo(&CardInfo);
    *(WORD*)buff = CardInfo.LogBlockSize;
    res = RES_OK;
    break;

  /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
    BSP_SD_GetCardInfo(&CardInfo);
    *(DWORD*)buff = CardInfo.LogBlockSize / SD_DEFAULT_BLOCK_SIZE;
    res = RES_OK;
    break;

  default:
    res = RES_PARERR;
  }

  return res;
}
