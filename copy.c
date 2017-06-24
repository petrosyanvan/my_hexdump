 /**
 * @file   copy.c
 * @author Van Petrosyan
 * @date   23 June 2017
 * @version 0.1
 * @brief   This module registers /dev/copy node into Linux filesystem. Any file written 
 * into /dev/copy is catched by driver and saved into /tmp/output file in ASCII charachter
 * format.
 */
 
#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <asm/uaccess.h>          // Required for the copy to user function
#define  DEVICE_NAME "copy"    ///< The device will appear at /dev/copy using this value
#define  CLASS_NAME  "copy"        ///< The device class -- this is a character device driver
#define  OUTPUT_DIRECTORY "/tmp/output"
 
MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Van Petrosyan");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux char driver");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static struct class*  copyClass  = NULL; ///< The device-driver class struct pointer
static struct device* copyDevice = NULL; ///< The device-driver device struct pointer
static char dev_write_flag = 0;              // identifier for device write state
 
// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
 
/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
   .open = dev_open,
   .write = dev_write,
   .release = dev_release,
};
 
/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init copy_init(void){
   printk(KERN_INFO "Copy: Initializing the Copy LKM\n");
 
   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "Copy: failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "Copy: registered correctly with major number %d\n", majorNumber);
 
   // Register the device class
   copyClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(copyClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(copyClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "Copy: device class registered correctly\n");
 
   // Register the device driver
   copyDevice = device_create(copyClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(copyDevice)){               // Clean up if there is an error
      class_destroy(copyClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(copyDevice);
   }
   printk(KERN_INFO "Copy: device class created correctly\n"); // Made it! device was initialized
   return 0;
}
 
/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit copy_exit(void){
   device_destroy(copyClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(copyClass);                          // unregister the device class
   class_destroy(copyClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "Copy: Goodbye from the LKM!\n");
}
 
/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "Copy: Device has been opened\n");
   dev_write_flag = 0;
   return 0;
}
 

/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is saved into a /tmp/output file in the 
 *  ASCII character format.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   struct file *filp = NULL;
   mm_segment_t old_fs ;
   static loff_t pos = 0;
   char dmp[4];
   int i;
   int flags = O_WRONLY|O_CREAT;

   // Check if this is first part of file then delete previous outputs from /tmp/output 
   if(dev_write_flag == 0){
      pos = 0;
      dev_write_flag = 1;
      flags |= O_TRUNC;
   }
   // open or create file, erase previous content
   filp = filp_open(OUTPUT_DIRECTORY, flags, 0644);
   if(IS_ERR(filp)){
      printk(KERN_INFO "Copy: Couldn't open output file\n");
      return -EFAULT;
   }
   
   old_fs = get_fs();
   set_fs(KERNEL_DS);
   // hexdump implementation
   for (i = 0; i < len; i++){      
      if(((i + 1) % 8) == 0)
         sprintf(dmp, "%02x\n", buffer[i]);
      else
         sprintf(dmp, "%02x ", buffer[i]);
      vfs_write(filp, dmp, strlen(dmp), &pos);
   }  
   set_fs(old_fs);
   filp_close(filp, NULL); 
   return len;
}
 
/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "Copy: Device successfully closed\n");
   return 0;
}
 
/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(copy_init);
module_exit(copy_exit);