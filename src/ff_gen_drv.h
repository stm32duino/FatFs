/**
  ******************************************************************************
  * @file    ff_gen_drv.h
  * @author  MCD Application Team
  * @brief   Header for ff_gen_drv.c module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FF_GEN_DRV_H
#define __FF_GEN_DRV_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "diskio.h"
#include "ff.h"
#include "stdint.h"


/* Exported types ------------------------------------------------------------*/

/**
  * @brief  Disk IO Driver structure definition
  */
typedef struct
{
  DSTATUS (*disk_initialize) (BYTE);                           /*!< Initialize Disk Drive*/
  DSTATUS (*disk_status)     (BYTE);                           /*!< Get Disk Status*/
  DRESULT (*disk_read)       (BYTE, BYTE*, LBA_t, UINT);       /*!< Read Sector(s)*/
  DRESULT (*disk_write)      (BYTE, const BYTE*, LBA_t, UINT); /*!< Write Sector(s)*/
  DRESULT (*disk_ioctl)      (BYTE, BYTE, void*);              /*!< I/O control operation*/
}Diskio_drvTypeDef;

/**
  * @brief  Global Disk IO Drivers structure definition
  */
typedef struct
{
  uint8_t                 is_initialized[FF_VOLUMES];
  const Diskio_drvTypeDef *drv[FF_VOLUMES];
  uint8_t                 lun[FF_VOLUMES];
  volatile uint8_t        nbr;

}Disk_drvTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint8_t FATFS_LinkDriver(const Diskio_drvTypeDef *drv, char *path);
uint8_t FATFS_UnLinkDriver(char *path);
uint8_t FATFS_LinkDriverEx(const Diskio_drvTypeDef *drv, char *path, BYTE lun);
uint8_t FATFS_UnLinkDriverEx(char *path, BYTE lun);
uint8_t FATFS_GetAttachedDriversNbr(void);

#ifdef __cplusplus
}
#endif

#endif /* __FF_GEN_DRV_H */
