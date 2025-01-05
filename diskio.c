/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include <stdlib.h>  // 用于动态内存分配
#include <stdint.h>  // 用于定义 uint8_t 等类型
#include<co_fifo.h>

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/


// 假设 RAM 磁盘的状态由一个全局变量表示
static uint8_t ram_disk_status = 0;  // 0 表示正常，非 0 表示错误
// RAM 磁盘的内存缓冲区
static uint8_t *ram_disk_buffer = NULL;
static size_t ram_disk_size = 0;  // RAM 磁盘的大小

// 初始化 RAM 磁盘
int RAM_disk_initialize(size_t size) {
    if (ram_disk_buffer != NULL) {
        free(ram_disk_buffer);  // 如果已经分配内存，先释放
    }

    ram_disk_buffer = (uint8_t *)malloc(size);
    if (ram_disk_buffer == NULL) {
        ram_disk_status = 1;  // 内存分配失败，设置错误状态
        return 1;
    }

    ram_disk_size = size;
    ram_disk_status = 0;  // 初始化成功，清除错误状态
    return 0;
}

// RAM 磁盘的状态函数
int RAM_disk_status() {
    if (ram_disk_status != 0) {
        return ram_disk_status;  // 返回错误状态
    }

    // 检查 RAM 磁盘的内存是否有效分配
    if (ram_disk_buffer == NULL || ram_disk_size == 0) {
        ram_disk_status = 2;  // 内存未分配或大小为 0，设置错误状态
        return ram_disk_status;
    }

    // 如果没有错误，返回 0 表示正常
    return 0;
}




// 假设 MMC/SD 卡的状态由一个全局变量表示
static uint8_t mmc_disk_status = 0;  // 0 表示正常，非 0 表示错误

// 假设 MMC/SD 卡的写保护状态由一个全局变量表示
static uint8_t mmc_write_protected = 0;  // 0 表示未写保护，1 表示写保护


// 检查卡是否插入
int MMC_is_present() {
    // 假设通过 GPIO 检测卡是否插入
    // 返回 1 表示卡已插入，0 表示未插入
    return 1;  // 假设卡已插入
}

// 检查卡是否初始化
int MMC_is_initialized() {
    // 假设通过全局变量或标志位检查卡是否初始化
    return 1;  // 假设卡已初始化
}

// 检查卡是否写保护
int MMC_is_write_protected() {
    // 假设通过 GPIO 检测写保护状态
    return 0;  // 假设未写保护
}

// 检查与卡的通信是否正常
int MMC_check_communication() {
    // 假设发送一个简单的命令（如 CMD0）并检查响应
    return 0;  // 假设通信正常
}

// MMC/SD 卡的状态函数
int MMC_disk_status() {
    // 检查卡是否插入
    if (!MMC_is_present()) {
        mmc_disk_status = 1;  // 卡未插入，设置错误状态
        return mmc_disk_status;
    }

    // 检查卡是否初始化
    if (!MMC_is_initialized()) {
        mmc_disk_status = 2;  // 卡未初始化，设置错误状态
        return mmc_disk_status;
    }

    // 检查写保护状态
    if (MMC_is_write_protected()) {
        mmc_write_protected = 1;  // 设置写保护状态
    } else {
        mmc_write_protected = 0;  // 清除写保护状态
    }

    // 检查通信是否正常
    if (MMC_check_communication() != 0) {
        mmc_disk_status = 3;  // 通信错误，设置错误状态
        return mmc_disk_status;
    }

    // 如果没有错误，返回 0 表示正常
    mmc_disk_status = 0;
    return mmc_disk_status;
}


// 假设 USB 存储设备的状态由一个全局变量表示
static uint8_t usb_disk_status = 0;  // 0 表示正常，非 0 表示错误
// 检查设备是否连接
int USB_is_connected() {
    // 假设通过 USB 控制器检测设备是否连接
    return 1;  // 假设设备已连接
}

// 检查设备是否初始化
int USB_is_initialized() {
    // 假设通过全局变量或标志位检查设备是否初始化
    return 1;  // 假设设备已初始化
}

// 检查设备是否就绪
int USB_is_ready() {
    // 假设通过 USB 控制器的状态寄存器检查设备是否就绪
    return 1;  // 假设设备已就绪
}

// 检查与设备的通信是否正常
int USB_check_communication() {
    // 假设发送一个简单的命令并检查响应
    return 0;  // 假设通信正常
}

// USB 存储设备的状态函数
int USB_disk_status() {
    // 检查设备是否连接
    if (!USB_is_connected()) {
        usb_disk_status = 1;  // 设备未连接，设置错误状态
        return usb_disk_status;
    }

    // 检查设备是否初始化
    if (!USB_is_initialized()) {
        usb_disk_status = 2;  // 设备未初始化，设置错误状态
        return usb_disk_status;
    }

    // 检查设备是否就绪
    if (!USB_is_ready()) {
        usb_disk_status = 3;  // 设备未就绪，设置错误状态
        return usb_disk_status;
    }

    // 检查通信是否正常
    if (USB_check_communication() != 0) {
        usb_disk_status = 4;  // 通信错误，设置错误状态
        return usb_disk_status;
    }

    // 如果没有错误，返回 0 表示正常
    usb_disk_status = 0;
    return usb_disk_status;
}
//********************************************************************************
//用DeepSeek修订
//********************************************************************************
DSTATUS disk_status (
    BYTE pdrv        /* Physical drive number to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT; /* Default status */

    switch (pdrv) {
    case DEV_RAM:
        if (RAM_disk_status() != 0) {
            stat = STA_NOINIT; /* 或其他适当的错误状态 */
        } else {
            stat = 0; /* 磁盘正常 */
        }
        break;

    case DEV_MMC:
        if (MMC_disk_status() != 0) {
            stat = STA_NOINIT; /* 或其他适当的错误状态 */
        } else {
            stat = 0; /* 磁盘正常 */
        }
        break;

    case DEV_USB:
        if (USB_disk_status() != 0) {
            stat = STA_NOINIT; /* 或其他适当的错误状态 */
        } else {
            stat = 0; /* 磁盘正常 */
        }
        break;

    default:
        stat = STA_NOINIT;
        break;
    }

    return stat;
}


// 假设底层硬件接口为 mmc_read_sectors
int mmc_read_sectors(LBA_t sector, UINT count, BYTE *buff) {
    // 实现具体的 MMC/SD 卡读取逻辑
    // 例如通过 SPI 或 SDIO 接口读取数据
    // 返回 0 表示成功，非 0 表示失败
}

// 假设底层硬件接口为 usb_read_sectors
int usb_read_sectors(LBA_t sector, UINT count, BYTE *buff) {
    // 实现具体的 USB 存储设备读取逻辑
    // 例如通过 USB 大容量存储协议读取数据
    // 返回 0 表示成功，非 0 表示失败
}
//********************************************************************************
int RAM_disk_read(BYTE *buff, LBA_t sector, UINT count) {
    // 检查 RAM 磁盘是否已初始化
    if (ram_disk_buffer == NULL || ram_disk_size == 0) {
        return 0; // 磁盘未初始化，读取失败
    }

    // 计算起始偏移量
    size_t start_offset = sector * FF_MAX_SS; // FF_MAX_SS 是 FatFs 定义的扇区大小（通常为 512 字节）
    size_t read_size = count * FF_MAX_SS;

    // 检查读取范围是否有效
    if (start_offset + read_size > ram_disk_size) {
        return 0; // 读取范围超出磁盘大小，读取失败
    }

    // 从 RAM 磁盘复制数据到目标缓冲区
    memcpy(buff, ram_disk_buffer + start_offset, read_size);

    return 1; // 读取成功
}

int MMC_disk_read(BYTE *buff, LBA_t sector, UINT count) {
    // 检查 MMC/SD 卡是否已初始化
    if (!MMC_is_initialized()) {
        return 0; // 卡未初始化，读取失败
    }

    // 检查卡是否插入
    if (!MMC_is_present()) {
        return 0; // 卡未插入，读取失败
    }

    // 调用底层硬件接口读取数据
    // 假设底层接口为 mmc_read_sectors(sector, count, buff)
    if (mmc_read_sectors(sector, count, buff) != 0) {
        return 0; // 读取失败
    }

    return 1; // 读取成功
}

int USB_disk_read(BYTE *buff, LBA_t sector, UINT count) {
    // 检查 USB 设备是否已初始化
    if (!USB_is_initialized()) {
        return 0; // 设备未初始化，读取失败
    }

    // 检查设备是否连接
    if (!USB_is_connected()) {
        return 0; // 设备未连接，读取失败
    }

    // 调用底层硬件接口读取数据
    // 假设底层接口为 usb_read_sectors(sector, count, buff)
    if (usb_read_sectors(sector, count, buff) != 0) {
        return 0; // 读取失败
    }

    return 1; // 读取成功
}
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,        /* Physical drive number to identify the drive */
    BYTE *buff,       /* Data buffer to store read data */
    LBA_t sector,     /* Start sector in LBA */
    UINT count        /* Number of sectors to read */
)
{
    DRESULT res = RES_ERROR;

    switch (pdrv) {
    case DEV_RAM:
        /* 调用实际的RAM磁盘读取函数 */
        if (RAM_disk_read(buff, sector, count)) {
            res = RES_OK;
        } else {
            res = RES_ERROR;
        }
        break;

    case DEV_MMC:
        /* 调用实际的MMC/SD卡读取函数 */
        if (MMC_disk_read(buff, sector, count)) {
            res = RES_OK;
        } else {
            res = RES_ERROR;
        }
        break;

    case DEV_USB:
        /* 调用实际的USB磁盘读取函数 */
        if (USB_disk_read(buff, sector, count)) {
            res = RES_OK;
        } else {
            res = RES_ERROR;
        }
        break;

    default:
        res = RES_PARERR;
        break;
    }

    return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

//#include <stdint.h>

// 假设 RAM 磁盘的写保护状态由一个全局变量表示
static uint8_t ram_write_protected = 0; // 0 表示未写保护，1 表示写保护

// 检查 RAM 磁盘是否写保护
int RAM_is_write_protected() {
    // RAM 磁盘没有硬件写保护机制，可以通过软件标志位模拟
    return ram_write_protected;
}

// 设置 RAM 磁盘的写保护状态
void RAM_set_write_protected(int protected) {
    ram_write_protected = protected ? 1 : 0;
}

int RAM_disk_write(const BYTE *buff, LBA_t sector, UINT count) {
    // 检查 RAM 磁盘是否已初始化
    if (ram_disk_buffer == NULL || ram_disk_size == 0) {
        return RES_NOTRDY; // 磁盘未初始化
    }

    // 计算起始偏移量
    size_t start_offset = sector * FF_MAX_SS; // FF_MAX_SS 是 FatFs 定义的扇区大小（通常为 512 字节）
    size_t write_size = count * FF_MAX_SS;

    // 检查写入范围是否有效
    if (start_offset + write_size > ram_disk_size) {
        return RES_PARERR; // 写入范围超出磁盘大小
    }

    // 将数据从源缓冲区复制到 RAM 磁盘
    memcpy(ram_disk_buffer + start_offset, buff, write_size);

    return RES_OK; // 写入成功
}

//#include <stdint.h>

// 假设 MMC/SD 卡的写保护状态由一个全局变量表示
//static uint8_t mmc_write_protected = 0; // 0 表示未写保护，1 表示写保护

// 检查 MMC/SD 卡是否写保护
/*
int MMC_is_write_protected() {
    // 如果支持硬件写保护，可以通过 GPIO 检测
    // 例如：
    // if (gpio_read(MMC_WRITE_PROTECT_PIN) == 1) {
    //     mmc_write_protected = 1; // 写保护
    // } else {
    //     mmc_write_protected = 0; // 未写保护
    // }

    // 如果不支持硬件写保护，可以通过卡的状态寄存器检测
    // 例如发送 CMD13 (SEND_STATUS) 命令查询卡的状态
    // 以下是一个伪代码示例：
    
    uint32_t card_status = 0;
    if (mmc_send_command(CMD13, 0, &card_status) == 0) {
        // 检查卡状态中的写保护位（假设位 12 表示写保护）
        if (card_status & (1 << 12)) {
            mmc_write_protected = 1; // 写保护
        } else {
            mmc_write_protected = 0; // 未写保护
        }
    } else {
        // 命令执行失败，默认未写保护
        mmc_write_protected = 0;
    }
    

    // 如果无法检测写保护状态，默认返回未写保护
    return mmc_write_protected;
}
*/

// 假设底层硬件接口为 mmc_write_sectors
int mmc_write_sectors(LBA_t sector, UINT count, const BYTE *buff) {
    // 实现具体的 MMC/SD 卡写入逻辑
    // 例如通过 SPI 或 SDIO 接口写入数据
    // 返回 0 表示成功，非 0 表示失败
}

int MMC_disk_write(const BYTE *buff, LBA_t sector, UINT count) {
    // 检查 MMC/SD 卡是否已初始化
    if (!MMC_is_initialized()) {
        return RES_NOTRDY; // 卡未初始化
    }

    // 检查卡是否插入
    if (!MMC_is_present()) {
        return RES_NOTRDY; // 卡未插入
    }

    // 检查卡是否写保护
    if (MMC_is_write_protected()) {
        return RES_WRPRT; // 卡写保护
    }

    // 调用底层硬件接口写入数据
    // 假设底层接口为 mmc_write_sectors(sector, count, buff)
    if (mmc_write_sectors(sector, count, buff) != 0) {
        return RES_ERROR; // 写入失败
    }

    return RES_OK; // 写入成功
}
// 假设底层硬件接口为 usb_write_sectors
int usb_write_sectors(LBA_t sector, UINT count, const BYTE *buff) {
    // 实现具体的 USB 存储设备写入逻辑
    // 例如通过 USB 大容量存储协议写入数据
    // 返回 0 表示成功，非 0 表示失败
}

#include <stdint.h>

// 假设 USB 存储设备的写保护状态由一个全局变量表示
static uint8_t usb_write_protected = 0; // 0 表示未写保护，1 表示写保护

// 检查 USB 存储设备是否写保护
int USB_is_write_protected() {
    // 如果支持硬件写保护，可以通过 GPIO 检测
    // 例如：
    // if (gpio_read(USB_WRITE_PROTECT_PIN) == 1) {
    //     usb_write_protected = 1; // 写保护
    // } else {
    //     usb_write_protected = 0; // 未写保护
    // }

    // 如果不支持硬件写保护，可以通过 SCSI 命令查询
    // 例如发送 MODE SENSE 命令查询写保护状态
    // 以下是一个伪代码示例：
    /*
    uint8_t mode_sense_data[64];
    if (usb_scsi_mode_sense(mode_sense_data, sizeof(mode_sense_data)) == 0) {
        // 解析 mode_sense_data，检查写保护位
        if (mode_sense_data[2] & 0x80) { // 假设写保护位在 mode_sense_data[2] 的最高位
            usb_write_protected = 1; // 写保护
        } else {
            usb_write_protected = 0; // 未写保护
        }
    } else {
        // 命令执行失败，默认未写保护
        usb_write_protected = 0;
    }
    */

    // 如果无法检测写保护状态，默认返回未写保护
    return usb_write_protected;
}
int USB_disk_write(const BYTE *buff, LBA_t sector, UINT count) {
    // 检查 USB 设备是否已初始化
    if (!USB_is_initialized()) {
        return RES_NOTRDY; // 设备未初始化
    }

    // 检查设备是否连接
    if (!USB_is_connected()) {
        return RES_NOTRDY; // 设备未连接
    }

    // 检查设备是否写保护
    if (USB_is_write_protected()) {
        return RES_WRPRT; // 设备写保护
    }

    // 调用底层硬件接口写入数据
    // 假设底层接口为 usb_write_sectors(sector, count, buff)
    if (usb_write_sectors(sector, count, buff) != 0) {
        return RES_ERROR; // 写入失败
    }

    return RES_OK; // 写入成功
}

#if FF_FS_READONLY == 0

DRESULT disk_write (
    BYTE pdrv,            /* Physical drive number to identify the drive */
    const BYTE *buff,     /* Data to be written */
    LBA_t sector,         /* Start sector in LBA */
    UINT count            /* Number of sectors to write */
)
{
    DRESULT res = RES_ERROR;

    switch (pdrv) {
    case DEV_RAM:
        /* 检查写保护状态 */
        if (RAM_is_write_protected()) {
            res = RES_WRPRT;
        } else if (RAM_disk_write(buff, sector, count)) {
            res = RES_OK;
        } else {
            res = RES_ERROR;
        }
        break;

    case DEV_MMC:
        /* 检查写保护状态 */
        if (MMC_is_write_protected()) {
            res = RES_WRPRT;
        } else if (MMC_disk_write(buff, sector, count)) {
            res = RES_OK;
        } else {
            res = RES_ERROR;
        }
        break;

    case DEV_USB:
        /* 检查写保护状态 */
        if (USB_is_write_protected()) {
            res = RES_WRPRT;
        } else if (USB_disk_write(buff, sector, count)) {
            res = RES_OK;
        } else {
            res = RES_ERROR;
        }
        break;

    default:
        res = RES_PARERR;
        break;
    }

    return res;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,    /* Physical drive number (0..) */
    BYTE cmd,     /* Control code */
    void *buff    /* Buffer to send/receive control data */
)
{
    DRESULT res = RES_ERROR;

    switch (pdrv) {
    case DEV_RAM:
        /* 处理RAM磁盘的命令 */
        switch (cmd) {
        case CTRL_SYNC:
            /* 确保所有磁盘写操作完成 */
            res = RES_OK;
            break;
        /* 其他命令处理 */
        default:
            res = RES_PARERR;
            break;
        }
        break;

    case DEV_MMC:
        /* 处理MMC/SD卡的命令 */
        switch (cmd) {
        case GET_SECTOR_SIZE:
            *(uint32_t*)buff = 512; /* 扇区大小 */
            res = RES_OK;
            break;
        /* 其他命令处理 */
        default:
            res = RES_PARERR;
            break;
        }
        break;

    case DEV_USB:
        /* 处理USB磁盘的命令 */
        switch (cmd) {
        case CTRL_POWER:
            /* 控制USB磁盘电源 */
            res = RES_OK;
            break;
        /* 其他命令处理 */
        default:
            res = RES_PARERR;
            break;
        }
        break;

    default:
        res = RES_PARERR;
        break;
    }

    return res;
}