#ifndef __QRNG_PCIE_H__
#define __QRNG_PCIE_H__

#include <stdint.h>

/**
  Structure representing an handle on a qrng pcie device.
*/
struct _qrng_pcie_handle_;
typedef struct _qrng_pcie_handle_ qrng_pcie_handle;

#ifdef __cplusplus
extern "C" {
#endif

/**
  Return the number of the qrng pcie devices that have been detected.

  @param count a pointer to the number
  @return 0 on success, -1 on failure.
 */
int qrng_pcie_count(uint32_t* count);

/**
  Return a numeric representation of the driver's version. The format uses one byte each 
  for the major, minor, and micro parts of the version number. The low-order byte 
  is unused. For example, version 2.0.1 has a numeric representation of 0x02000100.

  @param version a pointer to the driver version
  @return 0 on success, -1 on failure.
 */
int qrng_pcie_get_driver_version(uint32_t* version);

/**
  Return a numeric representation of the library's version. The format uses one byte each 
  for the major, minor, and micro parts of the version number. The low-order byte 
  is unused. For example, version 2.0.1 has a numeric representation of 0x02000100.

  @param version a pointer to the library version
  @return 0 on success, -1 on failure.
 */
int qrng_pcie_get_library_version(uint32_t* version);

/**
  Open the qrng pcie device.

  @param device_index the index of the qrng pcie device
  @param handle a pointer to a pointer to a handle of the device
  @return 0 on success, -1 on failure.
 */
int qrng_pcie_open(uint32_t device_index, qrng_pcie_handle** handle);

/**
  Close the qrng pcie device.

  @param handle a pointer to a handle of the device
  @return 0 on success, -1 on failure.
 */
int qrng_pcie_close(qrng_pcie_handle* handle);

/**
  Read random data from qrng pcie.

  @param handle a pointer to a handle of the device
  @param data the destination buffer to store the random data
  @param datlen the number of bytes to be read from qrng
  @return 0 on success, -1 on failure.
 */
int qrng_pcie_read_random(qrng_pcie_handle* handle, uint8_t* data, uint32_t datlen);

/**
  Read random data from qrng pcie by device.

  @param device_index the index of the qrng pcie device
  @param data the destination buffer to store the random data
  @param datlen the number of bytes to be read from qrng
  @return 0 on success, -1 on failure.
 */
int qrng_pcie_read_random_by_device(uint32_t device_index, uint8_t* data, uint32_t datlen);

/**
  Reset qrng pcie.

  @param device_index the index of the qrng pcie device
  @return 0 on success, -1 on failure.
 */
int qrng_pcie_reset(uint32_t device_index);

/**
  Ctrl qrng pcie output mode.

  @param device_index the index of the qrng pcie device
  @param mode the output mode of qrng(0x00:NONE,0x01:NET,0x02:USB,0x03:PCIE)
  @return 0 on success, -1 on failure.
 */
int qrng_pcie_ctrl_output(uint32_t device_index, uint8_t mode);

/**
  Get qrng pcie output mode.

  @param device_index the index of the qrng pcie device
  @param mode a pointer to the mode
  @return 0 on success, -1 on failure.
 */
int qrng_pcie_get_output(uint32_t device_index, uint8_t* mode);

/**
  Get qrng pcie laser diode temp.

  @param device_index the index of the qrng pcie device
  @param ld_temp a pointer to the laser diode temp
  @return 0 on success, -1 on failure.
 */
int qrng_pcie_get_ld_temp(uint32_t device_index, float* ld_temp);

/**
  Get qrng pcie boad temp.

  @param device_index the index of the qrng pcie device
  @param bd_temp a pointer to the boad temp
  @return 0 on success, -1 on failure.
 */
int qrng_pcie_get_bd_temp(uint32_t device_index, float* bd_temp);

/**
  Get qrng pcie state in bits.
  (if the bit is 0, it is normal;if the bit is 1,it is abnormal.)
  -------------------------------------------------------------------
  bit                  description
  -------------------------------------------------------------------
  bit0                repeat count status
  -------------------------------------------------------------------
  bit1                adapt scale status
  -------------------------------------------------------------------
  bit2                EEPROM parameter status
  -------------------------------------------------------------------
  bit3                parameter status
  -------------------------------------------------------------------
  bit4                check status
  -------------------------------------------------------------------
  bit5                EEPROM read and write status
  -------------------------------------------------------------------
  bit6                laser diode temp status                 
  -------------------------------------------------------------------
  bit7                boad temp status
  -------------------------------------------------------------------
  bit8                link status
  -------------------------------------------------------------------
  bit9-bit31        reserved
  -------------------------------------------------------------------
  @param device_index the index of the qrng pcie device
  @param state a pointer to the state
  @return 0 on success, -1 on failure.
 */
int qrng_pcie_get_state(uint32_t device_index, uint32_t* state);

/**
  Get qrng pcie entropy.

  @param device_index the index of the qrng pcie device
  @param entropy a pointer to the entropy
  @return 0 on success, -1 on failure.
 */
int qrng_pcie_get_entropy(uint32_t device_index, float* entropy);

#ifdef __cplusplus
}
#endif

#endif

