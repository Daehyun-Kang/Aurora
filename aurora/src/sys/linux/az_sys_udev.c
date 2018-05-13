/**
 * @file   az_sys_udev.c
 * @brief  
 * @date   12/05/18
 * @author dhkang
 *
 * @copyright 
 * Copyright (c) Fware, 2013-2017 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.\n\n
 * 
 * You should have received a copy of  the LICENSE with this file.\n\n
 * 
 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History:
 *   - 2018-05-12 dhkang initial create
 */

/* include header files */

#include "az_def.h"
#include "az_err.h"
#include "az_assert.h"
#include "az_malloc.h"

#ifdef  CONFIG_AZ_UDEV
#include <libudev.h>
#include <mntent.h>

/* declare global variables */

/* declare static variables */
static az_sys_xu_entity_t az_sys_udev_xu_entity = {
  .state = 0,
};
static az_sys_xu_t az_sys_udev_xu = &az_sys_udev_xu_entity;
static int az_sys_udevThreadOn = 0;


/* implement static functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
static struct udev_device *
get_child(struct udev *udev, struct udev_device *parent, const char *subsystem,
    const char *property, const char *value)
{
  struct udev_device *child = NULL;
  struct udev_enumerate *enumerate = udev_enumerate_new(udev);
  udev_enumerate_add_match_parent(enumerate, parent);
  if (subsystem) {
    udev_enumerate_add_match_subsystem(enumerate, subsystem);
  }
  if (property && value) {
    udev_enumerate_add_match_property(enumerate, property, value);
  }
  udev_enumerate_scan_devices(enumerate);

  struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
  struct udev_list_entry *entry;

  udev_list_entry_foreach(entry, devices) {
    const char *path = udev_list_entry_get_name(entry);
    //az_sys_eprintf("path:%s\n", path);
    child = udev_device_new_from_syspath(udev, path);
    break;
  }

  udev_enumerate_unref(enumerate);
  return child;
}

static struct udev_device *
find_usb_mass_storage(struct udev *udev, char *idVendor, char *idProduct)
{
  struct udev_enumerate *enumerate = udev_enumerate_new(udev);

  udev_enumerate_add_match_subsystem(enumerate, "scsi");
  udev_enumerate_add_match_property(enumerate, "DEVTYPE", "scsi_device");
  udev_enumerate_scan_devices(enumerate);

  struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
  struct udev_list_entry *entry;
  
  udev_list_entry_foreach(entry, devices) {
    const char *path = udev_list_entry_get_name(entry);
    struct udev_device *scsi = udev_device_new_from_syspath(udev, path); 
    if (NULL == scsi) continue;
    struct udev_device *scsi_disk = get_child(udev, scsi, "scsi_disk", NULL, NULL); 
    if (NULL == scsi_disk) {
      udev_device_unref(scsi);
      continue;
    }
    struct udev_device *block = get_child(udev, scsi, "block", NULL, NULL); 
    if (NULL == block) {
      az_sys_eprintf("path:%s block_device not found\n", path);
      udev_device_unref(scsi_disk);
      udev_device_unref(scsi);
      continue;
    }
    struct udev_device *usb = udev_device_get_parent_with_subsystem_devtype(scsi, "usb", "usb_device"); 
    if (NULL == usb) {
      az_sys_eprintf("path:%s usb_device not found\n", path);
      udev_device_unref(scsi_disk);
      udev_device_unref(block);
      udev_device_unref(scsi);
      continue;
    }
    char *_idVendor = udev_device_get_sysattr_value(usb, "idVendor"); 
    char *_idProduct = udev_device_get_sysattr_value(usb, "idProduct"); 
    if (strcmp(idVendor, _idVendor) || strcmp(idProduct, _idProduct)) { 
      udev_device_unref(scsi_disk);
      udev_device_unref(block);
      udev_device_unref(scsi);
      continue; 
    }
    udev_device_unref(scsi_disk);
    //udev_device_unref(block);
    udev_device_unref(scsi);
    udev_enumerate_unref(enumerate);
    //return scsi_disk;
    return block;
    //return scsi;
    break;
  }
  udev_enumerate_unref(enumerate);
  return NULL;
}

static inline az_bool_t az_udev_equal_path(struct udev_device *d, struct udev_device *e)
{
  return (strcmp(udev_device_get_syspath(d), udev_device_get_syspath(e)))? AZ_FALSE:AZ_TRUE; 
}
static inline az_bool_t az_udev_equal_node(struct udev_device *d, struct udev_device *e)
{
  return (strcmp(udev_device_get_devnode(d), udev_device_get_devnode(e)))? AZ_FALSE:AZ_TRUE; 
}

static inline az_bool_t az_is_mounted(const char *dev_path, char *path, int pathlen)
{
  FILE *mtab = NULL;
  struct mntent *part = NULL;
  az_bool_t is_mounted = AZ_FALSE;

  do {
    mtab = setmntent("/etc/mtab", "r");
    if (mtab == NULL) break;
    do {
      part = getmntent(mtab);
      if (part == NULL) break;
      if (part->mnt_fsname == NULL) continue;
      if (!strcmp(part->mnt_fsname, dev_path)) {
        is_mounted = AZ_TRUE;
        if (path) strncpy(path, part->mnt_dir, pathlen);
        break;
      }
    } while (1);
  } while (0);

  return is_mounted;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static void *az_sys_udevThread(void *arg)
{
  struct udev *udev;
  struct udev_monitor *mon;
  az_r_t r;

  do {
    udev = udev_new();
    if (NULL == udev) {
      az_sys_eprintf("%s", "udev NULL\n");
      break;
    }

    mon = udev_monitor_new_from_netlink(udev, "udev");
    if (NULL == mon) {
      az_sys_eprintf("%s", "udev mon NULL\n");
      udev_unref(udev);
      break;
    }
    udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", NULL);
    udev_monitor_filter_add_match_subsystem_devtype(mon, "scsi", NULL);
    //udev_monitor_filter_add_match_subsystem_devtype(mon, "block", "disk");
    //udev_monitor_filter_add_match_subsystem_devtype(mon, "block", "partition");
    udev_monitor_enable_receiving(mon);
    az_sys_io_t fd = udev_monitor_get_fd(mon);

    r = az_sys_xu_open_iomux();
    if (r < 0) {
      az_sys_rprintf(r, "%s", "open iomux fail\n");
      break;
    }
    az_sys_iomux_t iomux = az_sys_xu_iomux();
    az_sys_iomux_add(iomux, fd,  AZ_SYS_IO_IN); 

    az_sys_io_event_t io_evt[5]; 
    struct udev_device *dev = NULL;
    char *idVendor = NULL;
    char *idProduct = NULL;
    char *subsystem_str, *devtype_str;
    char *driver_str, *action_str;
    char *devnode_str;

    struct udev_device *usb = NULL; 
    struct udev_device *device = NULL; 
    struct udev_device *disk = NULL; 
    struct udev_device *partition = NULL; 

    char mount_path[CONFIG_AZ_PATH_MAX];

    int timeout = 2000;
    while (az_sys_udevThreadOn) {
      r = az_sys_iomux_wait(iomux, io_evt, 5, timeout);
      if (r == 0 && disk) {
        if (partition == NULL) { 
            partition = get_child(udev, disk, NULL, "DEVTYPE", "partition");
            if (partition) {
              devnode_str = udev_device_get_devnode(partition);
              az_bool_t is_mounted = az_is_mounted(devnode_str, mount_path, sizeof(mount_path));
              az_sys_printf("partition Node: %s mounted=%c path=%s\n", 
                  devnode_str, is_mounted? 'Y':'N',
                  is_mounted? mount_path:"NULL"); 
              timeout = 2000;
            }
        }
      }
      while (r > 0) {
        dev = udev_monitor_receive_device(mon);
        if (NULL == dev) {
          r--;
          continue;
        }
        subsystem_str = udev_device_get_subsystem(dev);
        devtype_str = udev_device_get_devtype(dev);
        action_str = udev_device_get_action(dev);
        driver_str = udev_device_get_driver(dev);
        devnode_str = udev_device_get_devnode(dev);
        az_sys_printf("Node: %s\n", devnode_str);
        az_sys_printf("Subsystem: %s\n", subsystem_str); 
        az_sys_printf("Devtype: %s\n", devtype_str);
        az_sys_printf("Driver: %s\n", driver_str); 
        az_sys_printf("Action: %s\n", action_str); 
        az_sys_printf("Serial: %s\n", udev_device_get_sysattr_value(dev, "serial")); 
        if (!strcmp(subsystem_str, "usb")) {
          if (!strcmp(action_str, "add")) {
            if (!strcmp(driver_str, "usb-storage")) { 
              usb = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device"); 
              if (usb) {
                idVendor = udev_device_get_sysattr_value(usb, "idVendor");
                idProduct =  udev_device_get_sysattr_value(usb, "idProduct");
                az_sys_printf("Vendor:Product %s:%s\n", idVendor, idProduct); 
              }
              dev = NULL;
            }
          } 
          if (!strcmp(action_str, "remove") && !strcmp(devtype_str, "usb_device")) { 
            if (usb && !strcmp(devnode_str,udev_device_get_devnode(usb))) {
              udev_device_unref(usb);
              usb = NULL;
              idProduct = idVendor = NULL;
            }
          }
          r--;
          if (dev) {
            udev_device_unref(dev);
            dev = NULL;
          }
          continue;
        } 
        if (!strcmp(devtype_str, "scsi_device")) {
          do {
            if (!strcmp(action_str, "add")) {
              if (!idVendor || !idProduct) {
                udev_device_unref(dev);
                dev = NULL;
                break;
              }
              az_sys_xu_sleep(3000000000);
              disk = find_usb_mass_storage(udev, idVendor, idProduct);
              if (!disk) {
                az_sys_eprintf("disk not found for %s:%s\n", idVendor, idProduct);
                udev_device_unref(dev);
                dev = NULL;
                break;
              }
              // save the current dev
              device = dev;
              dev = NULL;
              az_sys_eprintf("dev:%s\ndisk:%s\n", 
                  udev_device_get_syspath(device), 
                  udev_device_get_syspath(disk));
              /*
              devnode_str = udev_device_get_devnode(disk);
              struct stat st_buf;
              stat(devnode_str, &st_buf);
              az_sys_printf("Node: %s:%s:%d:%d\n", 
                devnode_str, udev_device_get_sysattr_value(disk, "dev"),
                major(st_buf.st_rdev), minor(st_buf.st_rdev));
              dev_t devnum = makedev(major(st_buf.st_rdev), minor(st_buf.st_rdev)+1);
              az_sys_printf("Sub: :%d:%d\n", major(devnum), minor(devnum));
              char sub_path[CONFIG_AZ_PATH_MAX];
              snprintf(sub_path, sizeof(sub_path), "%s%d", devnode_str, 1);
              if (az_sys_fs_exist(sub_path)) {
                az_sys_printf("Sub Node: %s\n", sub_path); 
              }
              */
              timeout = 100;
              break;
            }
            if (!strcmp(action_str, "remove") && device) {
              if (!az_udev_equal_path(dev, device)) {
                udev_device_unref(dev);
                dev = NULL;
                break;
              }
              if (partition) {
                udev_device_unref(partition); 
                partition = NULL;
              }
              if (disk) {
                az_sys_eprintf("remove disk:%p\n", udev_device_get_devnode(disk));
                udev_device_unref(disk); 
                disk = NULL;
              }
              udev_device_unref(device); 
              device = NULL;
              timeout = 2000;
              break;
            }
          } while (0);
          // default
          if (dev) {
            udev_device_unref(dev);
            dev = NULL;
          }
          r--;
          continue;
        }
        // default
        if (dev) {
          udev_device_unref(dev);
          dev = NULL;
        }
        r--;
      } // while (r > 0)
    } //while (az_sys_udevThreadOn) 

    az_sys_iomux_del(iomux, fd); 
    az_sys_xu_close_iomux();
    
    udev_monitor_unref(mon);
    udev_unref(udev);
  } while (0);
}


/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
az_r_t az_sys_udev_init()
{ 
  az_r_t result = AZ_ERR_L(AGAIN, 0);
  
  if (!az_sys_udevThreadOn) {
    az_sys_udevThreadOn = 1;
    result = az_sys_xu_create("sys_udev", az_sys_udevThread, NULL, NULL, &az_sys_udev_xu);
  }

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_sys_udev_deinit()
{
  az_r_t result = AZ_ERR(AGAIN);
  if (az_sys_udevThreadOn) {
    az_sys_udevThreadOn = 0;
    result = az_sys_xu_delete(az_sys_udev_xu);
  }
  return result;
}
#else
az_r_t az_sys_udev_init()
{
  return AZ_ERR(NOT_SUPPORT);
}
az_r_t az_sys_udev_deinit()
{
  return AZ_ERR(NOT_SUPPORT);
}

#endif
/* === end of AZ_SYS_UDEV_C === */
